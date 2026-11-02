// AlashCar: машинка на любых драйверах моторов (L298N, TB6612, L9110S, DRV8833, BTS7960, Cytron, ...) с двумя или четырьмя моторами (робот Фобо и подобные): вперёд, назад, повороты, скорость -100..100,
// плавный разгон, защита при потере связи и разбор команд Bluetooth. Без зависимостей. MIT License. Alash Electronics.
#ifndef ALASH_CAR_H
#define ALASH_CAR_H

#include <Arduino.h>
#include "AlashMotor.h"

class AlashCar {
 public:
  enum Error : uint8_t { ERR_OK = 0, ERR_FAILSAFE };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };
  enum Preset : uint8_t { PHOBO };               // схема подключения робота Фобо (Alash Electronics)

  // Правая и левая стороны, каждая: два пина направления (IN) и пин скорости (EN, ШИМ).
  // Для L298N: правая сторона IN1, IN2, ENA; левая IN3, IN4, ENB.
  AlashCar(uint8_t rightIn1, uint8_t rightIn2, uint8_t rightEn, uint8_t leftIn1, uint8_t leftIn2, uint8_t leftEn);
  explicit AlashCar(Preset preset);
  // Любые драйверы: два мотора (AlashMotor описывает драйвер: L298N, L9110S, DRV8833, BTS7960, Cytron ...). Моторы должны жить столько же, сколько машинка.
  AlashCar(AlashMotor &left, AlashMotor &right);
  void addLeft(AlashMotor &m);                 // ещё один мотор на левой стороне (4WD): ведёт себя так же, как первый (до 2 на сторону)
  void addRight(AlashMotor &m);
  AlashCar(const AlashCar &) = delete;
  AlashCar &operator=(const AlashCar &) = delete;

  void begin();

  // ---- Движение: скорость от -100 (назад) до 100 (вперёд) -----------------------------
  void forward(int8_t speed = 70);
  void backward(int8_t speed = 70);
  void turnLeft(int8_t speed = 60);          // на месте: левые назад, правые вперёд
  void turnRight(int8_t speed = 60);
  void stop();                               // остановиться (мотор свободно вращается)
  void brake();                              // остановиться торможением мотором
  void drive(int8_t speed, int8_t turn);     // "джойстик": speed вперёд / назад, turn вправо / влево (-100...100), стороны смешиваются сами
  void tank(int8_t left, int8_t right);      // отдельно левая и правая сторона

  // ---- Настройки -----------------------------------------------------------------
  void setSpeedLimit(uint8_t percent) { _limit = percent > 100 ? 100 : percent; }     // не быстрее этого процента (по умолчанию 100)
  void setMinPwm(uint8_t pwm) { _minPwm = pwm; }          // с какого ШИМ (0-255) моторы начинают крутиться (0 = как есть; обычно 60-90)
  void setPwmMax(uint16_t maxValue);                      // максимум ШИМ для всех моторов (255 по умолчанию; 1023 или 4095 при более точном ШИМ, ESP8266 с ядром 2.x)
  void setRamp(uint16_t ms) { _ramp = ms; }               // время разгона от нуля до максимума, мс (0 = мгновенно)
  void reverseRight(bool on = true) { _revR = on; }       // если правая сторона едет наоборот: вместо перепайки
  void reverseLeft(bool on = true) { _revL = on; }
  void setFailsafe(uint16_t ms) { _failsafe = ms; }       // если команд нет дольше ms, машинка останавливается (потеря Bluetooth); 0 = выключено

  // ---- Команды (из Bluetooth и терминала) ------------------------------------------------
  // F / W вперёд, B / S назад (S: в режиме RC Car это стоп, см. ниже), L / A влево, R / D вправо, S / X стоп, Y стоп, цифры 0-9 это скорость 0-100 %.
  bool handleCommand(char c);
  void handleStream(Stream &s);                           // прочитать все пришедшие символы и выполнить
  void setCommandStyle(bool wasdStop);                    // true: 'S' это назад (W A S D), false: 'S' это стоп (F B L R S, режим RC Car; по умолчанию)
  void update();                                          // вызывайте в loop(): разгон и защита при потере связи

  int8_t leftSpeed() const { return _curL; }
  int8_t rightSpeed() const { return _curR; }
  bool isMoving() const { return _curL != 0 || _curR != 0; }

  // ---- Диагностика -----------------------------------------------------------------
  Error error() const { return _err; }
  void setLanguage(Language lang) { _lang = lang; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);                 // по очереди крутит каждую сторону вперёд: проверьте направление и подключение

 private:
  AlashMotor _ownL, _ownR;                       // моторы, созданные конструктором с пинами
  AlashMotor *_mL[2] = {nullptr, nullptr}, *_mR[2] = {nullptr, nullptr};
  uint8_t _nL = 0, _nR = 0;
  bool _started = false, _revR = false, _revL = false, _wasd = false;
  uint8_t _limit = 100, _minPwm = 0;
  uint16_t _ramp = 0, _failsafe = 0;
  int8_t _tgtL = 0, _tgtR = 0, _curL = 0, _curR = 0;
  uint8_t _cmdSpeed = 70;
  uint32_t _lastCmd = 0, _lastRamp = 0;
  float _fL = 0, _fR = 0;
  Error _err = ERR_OK;
  Language _lang = LANG_RU;

  void setTargets(int l, int r);
  void apply(AlashMotor **ms, uint8_t n, int8_t speed, bool reversed);
  static int8_t clamp(int v) { return v > 100 ? 100 : v < -100 ? -100 : (int8_t)v; }
};

#endif  // ALASH_CAR_H
