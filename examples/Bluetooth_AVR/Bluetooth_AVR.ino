// [ru] AlashCar: управление по Bluetooth (HM-10 или HC-05) из приложения BLE Controller (режим RC Car) или с терминала телефона.
//      Команды: F вперёд, B назад, L влево, R вправо, S или Y стоп, цифры 0-9 это скорость (9 = максимум).
//      Модуль Bluetooth: TX модуля -> D10, RX модуля -> D11 (через делитель 1 кОм / 2 кОм: модуль 3.3 В).
//      Защита: если команд нет дольше 1 секунды (пропала связь), машинка останавливается сама.
// [kk] AlashCar: Bluetooth (HM-10 немесе HC-05) арқылы BLE Controller қолданбасынан (RC Car режимі) немесе телефон терминалынан басқару.
//      Командалар: F алға, B артқа, L солға, R оңға, S немесе Y тоқтау, 0-9 цифрлары жылдамдық (9 = максимум).
//      Bluetooth модулі: модуль TX -> D10, модуль RX -> D11 (1 кОм / 2 кОм бөлгіш арқылы: модуль 3.3 В).
//      Қорғаныс: 1 секундтан артық команда болмаса (байланыс үзілсе), машина өзі тоқтайды.
// [en] AlashCar: control over Bluetooth (HM-10 or HC-05) from the BLE Controller app (RC Car mode) or a phone terminal.
//      Commands: F forward, B backward, L left, R right, S or Y stop, digits 0-9 are the speed (9 = maximum).
//      Bluetooth module: module TX -> D10, module RX -> D11 (through a 1 kOhm / 2 kOhm divider: the module is 3.3 V).
//      Safety: if there are no commands for more than 1 second (the link is lost), the car stops by itself.
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
