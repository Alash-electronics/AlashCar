// [ru] AlashCar: самый простой пример. Машинка едет вперёд, назад и поворачивает. Для робота Фобо (Alash Electronics) и любой машинки на L298N.
//
//      Поднимите машинку колёсами в воздух при первом запуске! Подключение L298N (для Uno):
//        правая сторона: IN1 -> D4, IN2 -> D2, ENA -> D5;  левая сторона: IN3 -> D8, IN4 -> D12, ENB -> D6.
//        Питание драйвера 7-12 В (две батареи 18650), общий GND с платой. Перемычки ENA / ENB на L298N сняты.
// [kk] AlashCar: ең қарапайым мысал. Машина алға, артқа жүреді және бұрылады. Фобо роботы (Alash Electronics) және L298N-дегі кез келген машина үшін.
//
//      Бірінші іске қосуда машинаны дөңгелектерін ауаға көтеріңіз! L298N қосу (Uno үшін):
//        оң жағы: IN1 -> D4, IN2 -> D2, ENA -> D5;  сол жағы: IN3 -> D8, IN4 -> D12, ENB -> D6.
//        Драйвер қуаты 7-12 В (екі 18650 батарея), тақтамен ортақ GND. L298N-дегі ENA / ENB перемычкалары алынған.
// [en] AlashCar: the simplest example. The car drives forward, backward and turns. For the Phobo robot (Alash Electronics) and any car on an L298N.
//
//      Lift the car with its wheels in the air on the first run! L298N wiring (for Uno):
//        right side: IN1 -> D4, IN2 -> D2, ENA -> D5;  left side: IN3 -> D8, IN4 -> D12, ENB -> D6.
//        Driver power 7-12 V (two 18650 batteries), common GND with the board. The ENA / ENB jumpers on the L298N are removed.
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
