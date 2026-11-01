// AlashCar: управление аналоговым джойстиком (модуль KY-023): рычаг вперёд едет вперёд, вправо поворачивает вправо,
// по диагонали едет дугой. drive(скорость, поворот) смешивает левую и правую стороны сам.
// Подключение джойстика: VRx -> A0 (поворот), VRy -> A1 (газ), +5 В, GND. При запуске рычаг отпущен: центр определяется автоматически.
#include <AlashCar.h>

AlashCar car(AlashCar::PHOBO);
int centerX = 512, centerY = 512;

int axis(int pin, int center) {
  int d = analogRead(pin) - center;
  if (abs(d) < 40) return 0;               // мёртвая зона около центра
  return constrain(d * 100 / 500, -100, 100);
}

void setup() {
  car.begin();
  car.setRamp(200);
  delay(300);
  centerX = analogRead(A0);                // центр рычага замеряем при запуске
  centerY = analogRead(A1);
}

void loop() {
  car.update();
  int turn = axis(A0, centerX);
  int speed = -axis(A1, centerY);          // знак зависит от модуля: если вперёд едет назад, уберите минус
  car.drive(speed, turn);
}
