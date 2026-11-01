// AlashCar: самый простой пример. Машинка едет вперёд, назад и поворачивает. Для робота Фобо (Alash Electronics) и любой машинки на L298N.
//
// Поднимите машинку колёсами в воздух при первом запуске! Подключение L298N (для Uno):
//   правая сторона: IN1 -> D4, IN2 -> D2, ENA -> D5;  левая сторона: IN3 -> D8, IN4 -> D12, ENB -> D6.
//   Питание драйвера 7-12 В (две батареи 18650), общий GND с платой. Перемычки ENA / ENB на L298N сняты.
#include <AlashCar.h>

#if defined(ESP32)
AlashCar car(26, 27, 14, 25, 33, 32);      // правая IN1, IN2, EN; левая IN1, IN2, EN (подходят любые свободные пины)
#else
AlashCar car(AlashCar::PHOBO);             // пины робота Фобо; для своей схемы: AlashCar car(правая IN1, IN2, EN, левая IN1, IN2, EN)
#endif

void setup() {
  car.begin();
  // car.setMinPwm(80);                    // если на малой скорости моторы гудят и не едут
  // car.reverseRight();                   // если правая сторона едет наоборот (вместо перепайки проводов)
}

void loop() {
  car.forward(70);                         // скорость от -100 до 100
  delay(1500);
  car.stop();
  delay(500);
  car.turnRight(60);                       // разворот на месте
  delay(600);
  car.stop();
  delay(500);
  car.backward(60);
  delay(1500);
  car.stop();
  delay(2000);
}
