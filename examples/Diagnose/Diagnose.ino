// [ru] AlashCar: проверка подключения. ПОДНИМИТЕ машинку колёсами в воздух: скетч по очереди крутит правую и левую стороны вперёд.
//      Сторона крутится назад: reverseRight() / reverseLeft(). Стороны перепутаны: поменяйте тройки пинов. Не крутится: см. подсказку в мониторе порта.
// [kk] AlashCar: қосуды тексеру. Машинаны дөңгелектерін ауаға КӨТЕРІҢІЗ: скетч оң жағын, сосын сол жағын кезекпен алға айналдырады.
//      Бір жағы артқа айналады: reverseRight() / reverseLeft(). Жақтар ауысып кеткен: пин үштіктерін ауыстырыңыз. Айналмайды: порт мониторындағы кеңесті қараңыз.
// [en] AlashCar: wiring check. LIFT the car with its wheels in the air: the sketch spins the right and then the left side forward in turn.
//      A side spins backward: reverseRight() / reverseLeft(). The sides are swapped: swap the pin triples. Does not spin: see the hint in the serial monitor.
#include <AlashCar.h>

AlashCar car(AlashCar::PHOBO);

void setup() {
  Serial.begin(115200);
  delay(500);
  car.setLanguage(AlashCar::LANG_RU);      // LANG_KK: қазақша, LANG_EN: English
  car.begin();
  car.diagnose(Serial);
}

void loop() {}
