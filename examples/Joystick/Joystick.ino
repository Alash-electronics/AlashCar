// AlashCar: управление аналоговым джойстиком (модуль KY-023): рычаг вперёд едет вперёд, вправо поворачивает вправо,
// по диагонали едет дугой. drive(скорость, поворот) смешивает левую и правую стороны сам.
// Подключение джойстика: VRx -> A0 (поворот), VRy -> A1 (газ), +5 В, GND (ESP32: GPIO34 и GPIO35, питание 3.3 В; делитель 1023 заменить на 4095 в axis()). При запуске рычаг отпущен: центр определяется автоматически.
#include <AlashCar.h>

#if defined(ESP32)
AlashCar car(26, 27, 14, 25, 33, 32);
const uint8_t PIN_X = 34, PIN_Y = 35;       // ESP32: пины АЦП1
#else
AlashCar car(AlashCar::PHOBO);
const uint8_t PIN_X = A0, PIN_Y = A1;
#endif
#if defined(ESP32)
const int ADC_RANGE = 4095;
#else
const int ADC_RANGE = 1023;
#endif
int centerX = 512, centerY = 512;

int axis(int pin, int center) {
  int d = analogRead(pin) - center;
  if (abs(d) < 40) return 0;               // мёртвая зона около центра
  int range = ADC_RANGE / 2;
  return constrain(d * 100 / range, -100, 100);
}

void setup() {
  car.begin();
  car.setRamp(200);
  delay(300);
  centerX = analogRead(PIN_X);             // центр рычага замеряем при запуске
  centerY = analogRead(PIN_Y);
}

void loop() {
  car.update();
  int turn = axis(PIN_X, centerX);
  int speed = -axis(PIN_Y, centerY);          // знак зависит от модуля: если вперёд едет назад, уберите минус
  car.drive(speed, turn);
}
