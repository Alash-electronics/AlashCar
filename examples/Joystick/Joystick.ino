// [ru] AlashCar: управление аналоговым джойстиком (модуль KY-023): рычаг вперёд едет вперёд, вправо поворачивает вправо,
//      по диагонали едет дугой. drive(скорость, поворот) смешивает левую и правую стороны сам.
//      Подключение джойстика: VRx -> A0 (поворот), VRy -> A1 (газ), +5 В, GND (ESP32: GPIO34 и GPIO35, питание 3.3 В). При запуске рычаг отпущен: центр определяется автоматически.
// [kk] AlashCar: аналогтық джойстикпен (KY-023 модулі) басқару: сызғыш алға болса алға жүреді, оңға болса оңға бұрылады,
//      диагональ бойынша доға жасап жүреді. drive(жылдамдық, бұрылыс) сол және оң жақтарды өзі араластырады.
//      Джойстикті қосу: VRx -> A0 (бұрылыс), VRy -> A1 (газ), +5 В, GND (ESP32: GPIO34 және GPIO35, қуат 3.3 В). Іске қосқанда сызғыш босатылған болуы керек: орталық автоматты анықталады.
// [en] AlashCar: control with an analog joystick (KY-023 module): lever forward drives forward, right turns right,
//      diagonal drives in an arc. drive(speed, turn) mixes the left and right sides itself.
//      Joystick wiring: VRx -> A0 (turn), VRy -> A1 (throttle), +5 V, GND (ESP32: GPIO34 and GPIO35, 3.3 V supply). At start the lever must be released: the center is found automatically.
#if defined(ARDUINO_ARCH_ESP8266)
// У ESP8266 один аналоговый вход (A0), а джойстику нужны два: возьмите Uno, ESP32 или добавьте АЦП ADS1115 (библиотека AlashADS1115).
void setup() {
  Serial.begin(115200);
  Serial.println(F("ESP8266 has one analog input; a joystick needs two. Use Uno or ESP32."));
}
void loop() {}
#else
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
  if (abs(d) < ADC_RANGE / 25) return 0;   // мёртвая зона около центра (4 % шкалы)
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
#endif
