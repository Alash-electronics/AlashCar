// [ru] AlashCar: драйверы с двумя ШИМ-пинами на мотор: L9110S, HG7881, DRV8833, MX1508 (очень дешёвые модули из наборов). У мотора два входа A и B:
//      ШИМ на A и LOW на B едет вперёд, наоборот назад. Оба пина мотора должны уметь ШИМ (на Uno 3, 5, 6, 9, 10, 11).
//      Подключение: мотор A: IN-A1 на пин 5, IN-A2 на пин 6; мотор B: IN-B1 на пин 9, IN-B2 на пин 10; питание драйвера 3-10 В, общий GND. Поднимите машинку колёсами вверх!
//      Для других драйверов замените тип: DRIVER_IN_IN_PWM (L298N, TB6612: IN1, IN2, EN) или DRIVER_DIR_PWM (Cytron: DIR, PWM).
// [kk] AlashCar: әр моторға екі ШИМ пині бар драйверлер: L9110S, HG7881, DRV8833, MX1508 (жиынтықтардағы өте арзан модульдер). Мотордың A және B екі кірісі бар:
//      A-ға ШИМ және B-ға LOW алға жүреді, керісінше артқа. Мотордың екі пині де ШИМ білуі керек (Uno-да 3, 5, 6, 9, 10, 11).
//      Қосу: A моторы: IN-A1 5 пинге, IN-A2 6 пинге; B моторы: IN-B1 9 пинге, IN-B2 10 пинге; драйвер қуаты 3-10 В, ортақ GND. Машинаны дөңгелектерін жоғары қаратып ұстаңыз!
//      Басқа драйверлер үшін түрін ауыстырыңыз: DRIVER_IN_IN_PWM (L298N, TB6612: IN1, IN2, EN) немесе DRIVER_DIR_PWM (Cytron: DIR, PWM).
// [en] AlashCar: drivers with two PWM pins per motor: L9110S, HG7881, DRV8833, MX1508 (very cheap modules from kits). A motor has two inputs A and B:
//      PWM on A and LOW on B drives forward, the other way round backward. Both motor pins must support PWM (on Uno 3, 5, 6, 9, 10, 11).
//      Wiring: motor A: IN-A1 to pin 5, IN-A2 to pin 6; motor B: IN-B1 to pin 9, IN-B2 to pin 10; driver power 3-10 V, common GND. Hold the car with its wheels up!
//      For other drivers change the type: DRIVER_IN_IN_PWM (L298N, TB6612: IN1, IN2, EN) or DRIVER_DIR_PWM (Cytron: DIR, PWM).
#include <AlashCar.h>

AlashMotor leftMotor(AlashMotor::DRIVER_PWM_PWM, 5, 6);      // вперёд, назад
AlashMotor rightMotor(AlashMotor::DRIVER_PWM_PWM, 9, 10);
AlashCar car(leftMotor, rightMotor);

void setup() {
  car.begin();
  // rightMotor.setEnablePin(8);                             // вывод EN / STBY, если у драйвера он есть (BTS7960, TB6612)
  // car.reverseRight();                                     // если правая сторона едет наоборот
}

void loop() {
  car.forward(70);  delay(1500);
  car.stop();       delay(500);
  car.turnLeft(60); delay(700);
  car.backward(70); delay(1500);
  car.brake();      delay(1000);
}
