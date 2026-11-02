// MIT License. Alash Electronics.
#include "AlashMotor.h"

void AlashMotor::setEnablePin(uint8_t pin, bool activeHigh) {
  _en = pin; _enHigh = activeHigh;
}

void AlashMotor::enable(bool on) {
  if (_en == 255) return;
  pinMode(_en, OUTPUT);
  digitalWrite(_en, (on == _enHigh) ? HIGH : LOW);
}

void AlashMotor::begin() {
  uint8_t pins[3] = {_a, _b, _c};
  uint8_t n = _driver == DRIVER_IN_IN_PWM ? 3 : 2;
  for (uint8_t i = 0; i < n; i++) {
    if (pins[i] == 255) continue;
    if (_inv) digitalWrite(pins[i], HIGH); else digitalWrite(pins[i], LOW);     // сначала «стоп», потом выходы: мотор не дёрнется при запуске
    pinMode(pins[i], OUTPUT);
  }
  if (_en != 255) { digitalWrite(_en, _enHigh ? LOW : HIGH); pinMode(_en, OUTPUT); }
  coast();
  enable(true);
}

void AlashMotor::set(int v) {
  int a = v < 0 ? -v : v;
  if (a > (int)_max) a = _max;
  _cur = v < 0 ? -a : a;
  switch (_driver) {
    case DRIVER_IN_IN_PWM:
      if (a == 0) { out(_a, false); out(_b, false); }
      else if (v > 0) { out(_a, true); out(_b, false); }
      else { out(_a, false); out(_b, true); }
      pwm(_c, a);
      break;
    case DRIVER_PWM_PWM:
      if (v > 0) { pwm(_a, a); pwm(_b, 0); }
      else if (v < 0) { pwm(_a, 0); pwm(_b, a); }
      else { pwm(_a, 0); pwm(_b, 0); }
      break;
    case DRIVER_DIR_PWM:
      out(_a, v < 0);                                      // направление: HIGH = назад
      pwm(_b, a);
      break;
  }
}

void AlashMotor::coast() {
  _cur = 0;
  set(0);
}

void AlashMotor::brake() {
  _cur = 0;
  switch (_driver) {
    case DRIVER_IN_IN_PWM: out(_a, true); out(_b, true); pwm(_c, _max); break;     // обе ножки моста на HIGH: мотор тормозит
    case DRIVER_PWM_PWM: pwm(_a, _max); pwm(_b, _max); break;
    case DRIVER_DIR_PWM: set(0); break;
  }
}
