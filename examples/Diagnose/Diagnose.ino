// AlashCar: проверка подключения. ПОДНИМИТЕ машинку колёсами в воздух: скетч по очереди крутит правую и левую стороны вперёд.
// Сторона крутится назад: reverseRight() / reverseLeft(). Стороны перепутаны: поменяйте тройки пинов. Не крутится: см. подсказку в мониторе порта.
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
