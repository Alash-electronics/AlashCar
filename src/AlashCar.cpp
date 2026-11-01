// MIT License. Alash Electronics.
#include "AlashCar.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

AlashCar::AlashCar(uint8_t rightIn1, uint8_t rightIn2, uint8_t rightEn, uint8_t leftIn1, uint8_t leftIn2, uint8_t leftEn)
    : _pr1(rightIn1), _pr2(rightIn2), _pre(rightEn), _pl1(leftIn1), _pl2(leftIn2), _ple(leftEn) {}

// Робот Фобо: L298N, правая сторона IN1=4, IN2=2, ENA=5; левая IN3=8, IN4=12, ENB=6
AlashCar::AlashCar(Preset) : _pr1(4), _pr2(2), _pre(5), _pl1(8), _pl2(12), _ple(6) {}

void AlashCar::begin() {
  const uint8_t pins[6] = {_pr1, _pr2, _pre, _pl1, _pl2, _ple};
  for (uint8_t i = 0; i < 6; i++) {
    digitalWrite(pins[i], LOW);                // сначала «стоп», потом выходы: моторы не дёрнутся при запуске
    pinMode(pins[i], OUTPUT);
  }
  _started = true;
  _lastCmd = millis();
  _lastRamp = millis();
}

// Скорость -100..100 одной стороны -> два пина направления и ШИМ. minPwm убирает «мёртвую зону» моторов.
void AlashCar::apply(uint8_t in1, uint8_t in2, uint8_t en, int8_t speed, bool reversed) {
  if (reversed) speed = -speed;
  int pwm = 0;
  if (speed != 0) {
    int a = speed < 0 ? -speed : speed;
    if (a > _limit) a = _limit;
    pwm = _minPwm + (255 - _minPwm) * a / 100;      // 1 % уже даёт minPwm, 100 % даёт 255
    if (pwm > 255) pwm = 255;
  }
  if (speed > 0) { digitalWrite(in1, HIGH); digitalWrite(in2, LOW); }
  else if (speed < 0) { digitalWrite(in1, LOW); digitalWrite(in2, HIGH); }
  else { digitalWrite(in1, LOW); digitalWrite(in2, LOW); }
  analogWrite(en, pwm);
}

void AlashCar::setTargets(int l, int r) {
  if (!_started) begin();
  _tgtL = clamp(l);
  _tgtR = clamp(r);
  _lastCmd = millis();
  _err = ERR_OK;
  if (_ramp == 0) {
    _curL = _tgtL; _curR = _tgtR; _fL = _curL; _fR = _curR;
    apply(_pl1, _pl2, _ple, _curL, _revL);
    apply(_pr1, _pr2, _pre, _curR, _revR);
  }
}

void AlashCar::forward(int8_t s) { setTargets(s, s); }
void AlashCar::backward(int8_t s) { setTargets(-s, -s); }
void AlashCar::turnLeft(int8_t s) { setTargets(-s, s); }
void AlashCar::turnRight(int8_t s) { setTargets(s, -s); }
void AlashCar::tank(int8_t l, int8_t r) { setTargets(l, r); }

void AlashCar::stop() {
  setTargets(0, 0);
  _fL = _fR = 0;
  _curL = _curR = 0;                                      // остановка всегда мгновенная, без плавного снижения
  apply(_pl1, _pl2, _ple, 0, false);
  apply(_pr1, _pr2, _pre, 0, false);
}

void AlashCar::brake() {
  setTargets(0, 0);
  _fL = _fR = 0;
  _curL = _curR = 0;
  digitalWrite(_pl1, HIGH); digitalWrite(_pl2, HIGH); analogWrite(_ple, 255);     // обе ножки моста на HIGH: мотор тормозит
  digitalWrite(_pr1, HIGH); digitalWrite(_pr2, HIGH); analogWrite(_pre, 255);
}

// "Джойстик": левая = speed + turn, правая = speed - turn. Если сумма выходит за 100, обе стороны пропорционально уменьшаются
void AlashCar::drive(int8_t speed, int8_t turn) {
  int l = speed + turn, r = speed - turn;
  int m = max(abs(l), abs(r));
  if (m > 100) { l = l * 100 / m; r = r * 100 / m; }
  setTargets(l, r);
}

// ---- Команды -----------------------------------------------------------------------------------

void AlashCar::setCommandStyle(bool wasd) { _wasd = wasd; }

bool AlashCar::handleCommand(char c) {
  _lastCmd = millis();
  if (c >= '0' && c <= '9') {
    _cmdSpeed = (c - '0') * 100 / 9;                      // 0..9 это 0..100 %
    return true;
  }
  switch (c) {
    case 'F': case 'f': case 'W': case 'w': forward(_cmdSpeed); return true;
    case 'B': case 'b': backward(_cmdSpeed); return true;
    case 'L': case 'l': case 'A': case 'a': turnLeft(_cmdSpeed); return true;
    case 'R': case 'r': case 'D': case 'd': turnRight(_cmdSpeed); return true;
    case 'S': case 's':
      if (_wasd) backward(_cmdSpeed); else stop();       // в W A S D клавиша S это «назад»
      return true;
    case 'X': case 'x': case 'Y': case 'y': case ' ': stop(); return true;
    default: return false;
  }
}

void AlashCar::handleStream(Stream &s) {
  while (s.available()) handleCommand((char)s.read());
}

// ---- Опрос --------------------------------------------------------------------------------------------

void AlashCar::update() {
  if (!_started) return;
  uint32_t now = millis();
  // Защита: нет команд дольше failsafe, а машинка едет: останавливаемся
  if (_failsafe && isMoving() && (uint32_t)(now - _lastCmd) > _failsafe) {
    stop();
    _err = ERR_FAILSAFE;
    return;
  }
  if (_ramp == 0) return;
  uint32_t dt = now - _lastRamp;
  if (dt == 0) return;
  _lastRamp = now;
  float step = 100.0f * dt / _ramp;                       // на сколько процентов можно изменить скорость за прошедшее время
  auto approach = [step](float cur, float tgt) {
    if (cur < tgt) return cur + step > tgt ? tgt : cur + step;
    return cur - step < tgt ? tgt : cur - step;
  };
  float nL = approach(_fL, _tgtL), nR = approach(_fR, _tgtR);
  _fL = nL; _fR = nR;
  int8_t l = (int8_t)(nL + (nL >= 0 ? 0.5f : -0.5f)), r = (int8_t)(nR + (nR >= 0 ? 0.5f : -0.5f));
  if (l != _curL) { _curL = l; apply(_pl1, _pl2, _ple, _curL, _revL); }
  if (r != _curR) { _curR = r; apply(_pr1, _pr2, _pre, _curR, _revR); }
}

// ---- Диагностика ------------------------------------------------------------------------------------------

const __FlashStringHelper *AlashCar::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_FAILSAFE:
      return T("Машинка остановлена: команды не приходили дольше времени защиты (потеряна связь Bluetooth или пульт отпущен)",
               "Машина тоқтатылды: қорғаныс уақытынан ұзақ команда келмеді (Bluetooth байланысы үзілді)",
               "The car was stopped: no commands arrived within the failsafe time (Bluetooth lost or the controller released)");
  }
  return F("");
}

void AlashCar::diagnose(Print &out) {
  out.println(T("AlashCar: диагностика", "AlashCar: диагностика", "AlashCar: diagnostics"));
  out.println(T("Поднимите машинку колёсами в воздух! По очереди крутятся правая и левая стороны вперёд по 2 секунды.",
                "Машинаны дөңгелектерін ауаға көтеріңіз! Оң және сол жақ кезекпен алға 2 секундтан айналады.",
                "Lift the car so the wheels are in the air! The right and then the left side spin forward for 2 seconds each."));
  out.println(T("Правая сторона вперёд...", "Оң жақ алға...", "Right side forward..."));
  tank(0, 70); delay(2000); stop(); delay(500);
  out.println(T("Левая сторона вперёд...", "Сол жақ алға...", "Left side forward..."));
  tank(70, 0); delay(2000); stop();
  out.println(T("Если сторона вращается назад: reverseRight() или reverseLeft(). Если стороны перепутаны: поменяйте местами тройки пинов в конструкторе. Если не крутится: проверьте питание драйвера (7-12 В), общий GND с платой, перемычки ENA / ENB на L298N (должны быть сняты при управлении ШИМ) и setMinPwm(80) для слабых батарей.",
                "Бір жақ артқа айналса: reverseRight() не reverseLeft(). Жақтар шатасса: конструкторда пин үштіктерін ауыстырыңыз. Айналмаса: драйвер қуатын (7-12 В), ортақ GND, L298N-дегі ENA / ENB перемычкаларын (ШИМ кезінде алынуы керек) тексеріңіз; әлсіз батарея үшін setMinPwm(80).",
                "If a side turns backward: reverseRight() or reverseLeft(). If the sides are swapped: swap the pin triples in the constructor. If nothing turns: check the driver power (7-12 V), a common GND with the board, the ENA / ENB jumpers on the L298N (must be removed for PWM control) and setMinPwm(80) for weak batteries."));
}
