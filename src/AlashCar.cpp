// MIT License. Alash Electronics.
#include "AlashCar.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

AlashCar::AlashCar(uint8_t rightIn1, uint8_t rightIn2, uint8_t rightEn, uint8_t leftIn1, uint8_t leftIn2, uint8_t leftEn)
    : _ownL(AlashMotor::DRIVER_IN_IN_PWM, leftIn1, leftIn2, leftEn), _ownR(AlashMotor::DRIVER_IN_IN_PWM, rightIn1, rightIn2, rightEn) {
  _mL[0] = &_ownL; _nL = 1; _mR[0] = &_ownR; _nR = 1;
}

// Робот Фобо: L298N, правая сторона IN1=4, IN2=2, ENA=5; левая IN3=8, IN4=12, ENB=6
AlashCar::AlashCar(Preset) : AlashCar(4, 2, 5, 8, 12, 6) {}

AlashCar::AlashCar(AlashMotor &left, AlashMotor &right) {
  _mL[0] = &left; _nL = 1; _mR[0] = &right; _nR = 1;
}

void AlashCar::addLeft(AlashMotor &m) { if (_nL < 2) _mL[_nL++] = &m; }
void AlashCar::addRight(AlashMotor &m) { if (_nR < 2) _mR[_nR++] = &m; }

void AlashCar::setPwmMax(uint16_t maxValue) {
  for (uint8_t i = 0; i < _nL; i++) _mL[i]->setPwmMax(maxValue);
  for (uint8_t i = 0; i < _nR; i++) _mR[i]->setPwmMax(maxValue);
}

void AlashCar::begin() {
  for (uint8_t i = 0; i < _nL; i++) _mL[i]->begin();           // сначала «стоп», потом выходы: моторы не дёрнутся при запуске
  for (uint8_t i = 0; i < _nR; i++) _mR[i]->begin();
  _started = true;
  _lastCmd = millis();
  _lastRamp = millis();
}

// Скорость -100..100 одной стороны -> два пина направления и ШИМ. minPwm убирает «мёртвую зону» моторов.
void AlashCar::apply(AlashMotor **ms, uint8_t n, int8_t speed, bool reversed) {
  if (reversed) speed = -speed;
  for (uint8_t i = 0; i < n; i++) {
    int top = ms[i]->pwmMax();
    int pwm = 0;
    if (speed != 0) {
      int a = speed < 0 ? -speed : speed;
      if (a > _limit) a = _limit;
      int mn = (int)_minPwm * top / 255;               // минимальный ШИМ задан в шкале 0-255, пересчитываем под разрядность
      pwm = mn + (top - mn) * a / 100;                 // 1 % уже даёт minPwm, 100 % даёт максимум
      if (pwm > top) pwm = top;
    }
    ms[i]->set(speed < 0 ? -pwm : pwm);
  }
}

void AlashCar::setTargets(int l, int r) {
  if (!_started) begin();
  _tgtL = clamp(l);
  _tgtR = clamp(r);
  _lastCmd = millis();
  _err = ERR_OK;
  if (_ramp == 0) {
    _curL = _tgtL; _curR = _tgtR; _fL = _curL; _fR = _curR;
    apply(_mL, _nL, _curL, _revL);
    apply(_mR, _nR, _curR, _revR);
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
  for (uint8_t i = 0; i < _nL; i++) _mL[i]->coast();
  for (uint8_t i = 0; i < _nR; i++) _mR[i]->coast();
}

void AlashCar::brake() {
  setTargets(0, 0);
  _fL = _fR = 0;
  _curL = _curR = 0;
  for (uint8_t i = 0; i < _nL; i++) _mL[i]->brake();                              // замкнуть обмотки: мотор тормозит
  for (uint8_t i = 0; i < _nR; i++) _mR[i]->brake();
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
  if (l != _curL) { _curL = l; apply(_mL, _nL, _curL, _revL); }
  if (r != _curR) { _curR = r; apply(_mR, _nR, _curR, _revR); }
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
  out.println(T("Если сторона вращается назад: reverseRight() или reverseLeft(). Если стороны перепутаны: поменяйте местами тройки пинов в конструкторе (или левый и правый AlashMotor). Если не крутится: проверьте питание драйвера (7-12 В), общий GND с платой, перемычки ENA / ENB на L298N (должны быть сняты при управлении ШИМ), вывод STBY у TB6612 и R_EN / L_EN у BTS7960 (должны быть HIGH: setEnablePin()), что пины скорости умеют ШИМ, и setMinPwm(80) для слабых батарей. Для L9110S, HG7881 и DRV8833 оба пина мотора должны быть ШИМ-пинами. Если на ESP8266 с ядром 2.x мотор едва вращается, вызовите setPwmMax(1023).",
                "Бір жақ артқа айналса: reverseRight() не reverseLeft(). Жақтар шатасса: конструкторда пин үштіктерін (немесе сол және оң AlashMotor) ауыстырыңыз. Айналмаса: драйвер қуатын (7-12 В), ортақ GND, L298N-дегі ENA / ENB перемычкаларын (ШИМ кезінде алынуы керек), TB6612-дегі STBY және BTS7960-тағы R_EN / L_EN (HIGH болуы керек: setEnablePin()), жылдамдық пиндерінің ШИМ білуін тексеріңіз; әлсіз батарея үшін setMinPwm(80). L9110S, HG7881 және DRV8833 үшін мотордың екі пині де ШИМ пині болуы керек. ESP8266 2.x ядросында мотор әрең айналса, setPwmMax(1023) шақырыңыз.",
                "If a side turns backward: reverseRight() or reverseLeft(). If the sides are swapped: swap the pin triples in the constructor (or the left and right AlashMotor). If nothing turns: check the driver power (7-12 V), a common GND with the board, the ENA / ENB jumpers on the L298N (must be removed for PWM control), the STBY pin of a TB6612 and R_EN / L_EN of a BTS7960 (must be HIGH: setEnablePin()), that the speed pins can do PWM, and setMinPwm(80) for weak batteries. For the L9110S, HG7881 and DRV8833 both motor pins must be PWM pins. If on an ESP8266 with core 2.x the motor barely turns, call setPwmMax(1023)."));
}
