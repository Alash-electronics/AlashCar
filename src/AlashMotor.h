// AlashMotor: один коллекторный мотор с любым популярным драйвером (L298N, L293D, TB6612, L9110S / HG7881, DRV8833, MX1508, BTS7960, Cytron):
// знаковая скорость, торможение и свободный выбег, вывод включения EN / STBY, инверсия сигналов, разрядность ШИМ. Без зависимостей.
// MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashCar
#ifndef ALASH_MOTOR_H
#define ALASH_MOTOR_H

#include <Arduino.h>

class AlashMotor {
 public:
  // IN_IN_PWM: два пина направления и пин скорости EN (ШИМ): L298N, L293D, TB6612 (AIN1, AIN2, PWMA), BTS7960 с ШИМ на EN.
  // PWM_PWM: два ШИМ-пина, вперёд и назад: L9110S, HG7881, DRV8833, MX1508, BTS7960 (RPWM и LPWM). Пины скорости должны уметь ШИМ.
  // DIR_PWM: пин направления и пин ШИМ: Cytron MD10 / MD13, многие платы «DIR + PWM».
  enum Driver : uint8_t { DRIVER_IN_IN_PWM = 0, DRIVER_PWM_PWM, DRIVER_DIR_PWM };

  AlashMotor() {}                                          // пустой мотор (пины не заданы), для внутреннего использования
  // pinA, pinB, pinC: IN_IN_PWM: IN1, IN2, EN; PWM_PWM: вперёд, назад; DIR_PWM: DIR, PWM (pinC не нужен)
  AlashMotor(Driver driver, uint8_t pinA, uint8_t pinB, uint8_t pinC = 255) : _driver(driver), _a(pinA), _b(pinB), _c(pinC) {}

  void begin();                                            // настроить пины: сначала «стоп», потом выходы
  void set(int pwm);                                       // скорость со знаком в единицах ШИМ: от -pwmMax() (назад) до pwmMax() (вперёд)
  void coast();                                            // выключить, мотор вращается свободно
  void brake();                                            // замкнуть обмотку: мотор тормозит (у DIR_PWM то же, что coast)
  void setEnablePin(uint8_t pin, bool activeHigh = true);  // общий вывод EN / STBY / R_EN+L_EN: включён, пока работает (begin() его включает)
  void enable(bool on);                                    // включить или выключить драйвер выводом EN
  void setPwmMax(uint16_t maxValue) { _max = maxValue ? maxValue : 255; }   // максимум ШИМ: 255 по умолчанию; 1023 для 10 бит, 4095 для 12 (analogWriteResolution) или ESP8266 с ядром 2.x
  uint16_t pwmMax() const { return _max; }
  void setInverted(bool on) { _inv = on; }                 // инвертировать уровни и ШИМ: для драйверов на оптопарах и с инвертирующим входом
  int current() const { return _cur; }
  Driver driver() const { return _driver; }

 private:
  void out(uint8_t pin, bool high) { digitalWrite(pin, (high != _inv) ? HIGH : LOW); }
  void pwm(uint8_t pin, int v) { analogWrite(pin, _inv ? (int)_max - v : v); }
  Driver _driver = DRIVER_IN_IN_PWM;
  uint8_t _a = 255, _b = 255, _c = 255, _en = 255;
  bool _enHigh = true, _inv = false;
  uint16_t _max = 255;
  int _cur = 0;
};

#endif
