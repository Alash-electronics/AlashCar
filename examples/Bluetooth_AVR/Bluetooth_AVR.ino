// AlashCar: управление по Bluetooth (HM-10 или HC-05) из приложения BLE Controller (режим RC Car) или с терминала телефона.
// Команды: F вперёд, B назад, L влево, R вправо, S или Y стоп, цифры 0-9 это скорость (9 = максимум).
// Модуль Bluetooth: TX модуля -> D10, RX модуля -> D11 (через делитель 1 кОм / 2 кОм: модуль 3.3 В).
// Защита: если команд нет дольше 1 секунды (пропала связь), машинка останавливается сама.
#include <AlashCar.h>
#include <SoftwareSerial.h>

AlashCar car(AlashCar::PHOBO);
SoftwareSerial bt(10, 11);                 // RX, TX платы

void setup() {
  Serial.begin(115200);
  bt.begin(9600);                          // скорость модуля HM-10 / HC-05 по умолчанию
  car.begin();
  car.setRamp(300);                        // плавный разгон за 0.3 с: машинка не дёргается и не отрывается колёсами
  car.setFailsafe(1000);                   // потеря связи: стоп
}

void loop() {
  car.handleStream(bt);                    // читает все пришедшие символы и выполняет команды
  car.update();                            // разгон и защита по времени
}
