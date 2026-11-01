# AlashCar: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **машинки-робота** на драйверах **L298N / TB6612** (в том числе робот **Фобо** Alash Electronics): `car.forward(70)`, повороты, управление джойстиком `car.drive(скорость, поворот)` и **команды Bluetooth** от приложения BLE Controller (F, B, L, R, S). Плавный разгон и **защита: пропала связь, машинка остановилась сама**. Без зависимостей. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashCar** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| L298N | Phobo (Uno) | Назначение |
|---|---|---|
| IN1 | D4 | правая сторона, направление |
| IN2 | D2 | правая сторона, направление |
| ENA | D5 (ШИМ) | правая сторона, скорость |
| IN3 | D8 | левая сторона, направление |
| IN4 | D12 | левая сторона, направление |
| ENB | D6 (ШИМ) | левая сторона, скорость |
| питание | 7-12 В (батареи 18650) | GND общий с платой |

Перемычки ENA и ENB на плате L298N должны быть **сняты**, иначе скорость регулировать нельзя. Bluetooth HM-10: TX модуля к D10, RX к D11 (через делитель напряжения, модуль 3.3 В).

### Быстрый старт
```cpp
#include <AlashCar.h>
AlashCar car(AlashCar::PHOBO);      // пины робота Фобо / Phobo pins

void setup() { car.begin(); }
void loop() {
  car.forward(70);  delay(1500);    // скорость -100..100 / speed
  car.stop();       delay(500);
  car.turnRight(60); delay(600);
  car.stop();       delay(2000);
}
```

### Что умеет
- **Простые команды:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`, `brake()`; скорость от -100 до 100.
- **Джойстик:** `drive(скорость, поворот)` смешивает левую и правую стороны, при перегрузке сам уменьшает обе пропорционально.
- **Bluetooth:** `car.handleStream(bt)` понимает F, B, L, R, S, Y (режим RC Car приложения BLE Controller), W A S D и цифры 0-9 как скорость.
- **Защита:** `setFailsafe(1000)`: нет команд дольше секунды, и машинка останавливается сама (потеряна связь или забыли отпустить кнопку).
- **Плавный старт:** `setRamp(300)` убирает рывки и срыв колёс; `setMinPwm(80)` помогает слабым батареям, когда мотор на малой скорости гудит и не едет.
- **Монтаж без перепайки:** `reverseRight()` и `reverseLeft()`, если сторона едет наоборот.
- **Фобо одной строкой:** `AlashCar car(AlashCar::PHOBO)` с пинами набора; свои пины в конструкторе.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)` по очереди крутит стороны.
- Работает на **Arduino AVR и ESP32** (ядро ESP32 3.x с `analogWrite`).

### Справочник по методам
| Метод | Что делает |
|---|---|
| `AlashCar(AlashCar::PHOBO) / AlashCar(r1,r2,re,l1,l2,le)` | Пины робота Фобо / свои пины |
| `forward(s) backward(s) turnLeft(s) turnRight(s)` | Движение со скоростью -100..100 |
| `stop() / brake()` | Остановка свободная / торможением |
| `drive(speed, turn) / tank(l, r)` | Джойстик / по сторонам |
| `setRamp(ms) / setMinPwm(pwm) / setSpeedLimit(%)` | Разгон / мёртвая зона / предел скорости |
| `reverseRight() / reverseLeft()` | Поменять направление стороны |
| `handleCommand(c) / handleStream(bt)` | Команды Bluetooth |
| `setFailsafe(ms)` | Стоп при потере связи |
| `update()` | Вызывать в loop() (разгон, защита) |
| `diagnose(Serial)` | Крутит стороны по очереди |

### Если что-то не работает
**Моторы гудят, но не едут**

Слабое питание или малая скорость. Подайте 7-12 В на драйвер и вызовите `car.setMinPwm(80)`. Перемычки ENA и ENB на L298N должны быть сняты.

**Машинка едет назад, когда надо вперёд, или поворачивает наоборот**

Поменяйте направление стороны: `car.reverseRight()` или `car.reverseLeft()`. Если перепутаны сами стороны, поменяйте тройки пинов в конструкторе.

**Машинка продолжает ехать, когда пропала связь Bluetooth**

Включите защиту: `car.setFailsafe(1000)` и вызывайте `car.update()` в loop().

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`

---

## Қазақша

**L298N / TB6612** драйверлері бар **робот-машинаға** (Alash Electronics **Фобо** роботын қоса) арналған қарапайым кітапхана: `car.forward(70)`, бұрылыстар, джойстикпен `car.drive(жылдамдық, бұрылыс)` және BLE Controller қосымшасынан **Bluetooth командалары**. Жайлап жүру және **қорғаныс: байланыс үзілсе, машина өзі тоқтайды**. Тәуелділіксіз.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashCar** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| L298N | Phobo (Uno) | Мақсаты |
|---|---|---|
| IN1 | D4 | правая сторона, направление |
| IN2 | D2 | правая сторона, направление |
| ENA | D5 (ШИМ) | правая сторона, скорость |
| IN3 | D8 | левая сторона, направление |
| IN4 | D12 | левая сторона, направление |
| ENB | D6 (ШИМ) | левая сторона, скорость |
| питание | 7-12 В (батареи 18650) | GND общий с платой |

L298N тақтасындағы ENA және ENB перемычкалары **алынуы** керек. HM-10: TX D10-ға, RX D11-ге.

### Жылдам бастау
```cpp
#include <AlashCar.h>
AlashCar car(AlashCar::PHOBO);      // пины робота Фобо / Phobo pins

void setup() { car.begin(); }
void loop() {
  car.forward(70);  delay(1500);    // скорость -100..100 / speed
  car.stop();       delay(500);
  car.turnRight(60); delay(600);
  car.stop();       delay(2000);
}
```

### Мүмкіндіктері
- **Қарапайым командалар:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`; жылдамдық -100...100.
- **Джойстик:** `drive(жылдамдық, бұрылыс)`.
- **Bluetooth:** `car.handleStream(bt)` F, B, L, R, S, Y және W A S D, 0-9 цифрларын түсінеді.
- **Қорғаныс:** `setFailsafe(1000)`: команда келмесе, машина өзі тоқтайды.
- **Жайлап бастау:** `setRamp(300)`; `setMinPwm(80)`.
- **Қайта дәнекерлеусіз:** `reverseRight()`, `reverseLeft()`.
- **Фобо бір жолмен:** `AlashCar car(AlashCar::PHOBO)`.
- **Түсінікті кеңестер** үш тілде.
- **Arduino AVR және ESP32**.

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `AlashCar(AlashCar::PHOBO) / AlashCar(r1,r2,re,l1,l2,le)` | Фобо пиндері / өз пиндері |
| `forward(s) backward(s) turnLeft(s) turnRight(s)` | Қозғалыс |
| `stop() / brake()` | Тоқтау |
| `drive(speed, turn) / tank(l, r)` | Джойстик / жақтар бойынша |
| `setRamp(ms) / setMinPwm(pwm) / setSpeedLimit(%)` | Жүру / өлі аймақ / шек |
| `reverseRight() / reverseLeft()` | Жақ бағытын ауыстыру |
| `handleCommand(c) / handleStream(bt)` | Bluetooth командалары |
| `setFailsafe(ms)` | Байланыс үзілсе тоқтау |
| `update()` | loop() ішінде шақыру |
| `diagnose(Serial)` | Жақтарды кезекпен айналдыру |

### Бірдеңе жұмыс істемесе
**Моторлар ыңылдайды, бірақ жүрмейді**

Қуат әлсіз. Драйверге 7-12 В беріңіз, `car.setMinPwm(80)`.

**Машина артқа жүреді**

`car.reverseRight()` не `car.reverseLeft()`.

**Bluetooth үзілгенде машина жүре береді**

`car.setFailsafe(1000)` қосыңыз.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`

---

## English

A simple library for a **robot car** on **L298N / TB6612** drivers (including the Alash **Phobo** robot): `car.forward(70)`, turns, joystick control with `car.drive(speed, turn)` and **Bluetooth commands** from the BLE Controller app (F, B, L, R, S). Smooth acceleration and a **failsafe: if the link is lost the car stops by itself**. No dependencies. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashCar** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| L298N | Phobo (Uno) | Purpose |
|---|---|---|
| IN1 | D4 | правая сторона, направление |
| IN2 | D2 | правая сторона, направление |
| ENA | D5 (ШИМ) | правая сторона, скорость |
| IN3 | D8 | левая сторона, направление |
| IN4 | D12 | левая сторона, направление |
| ENB | D6 (ШИМ) | левая сторона, скорость |
| питание | 7-12 В (батареи 18650) | GND общий с платой |

The ENA and ENB jumpers on the L298N board must be **removed**, otherwise the speed cannot be controlled. HM-10 Bluetooth: module TX to D10, RX to D11 (via a voltage divider, the module is 3.3 V).

### Quick start
```cpp
#include <AlashCar.h>
AlashCar car(AlashCar::PHOBO);      // пины робота Фобо / Phobo pins

void setup() { car.begin(); }
void loop() {
  car.forward(70);  delay(1500);    // скорость -100..100 / speed
  car.stop();       delay(500);
  car.turnRight(60); delay(600);
  car.stop();       delay(2000);
}
```

### Features
- **Simple commands:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`, `brake()`; speed from -100 to 100.
- **Joystick:** `drive(speed, turn)` mixes the left and right sides and scales both down proportionally on overflow.
- **Bluetooth:** `car.handleStream(bt)` understands F, B, L, R, S, Y (the RC Car mode of the BLE Controller app), W A S D and digits 0-9 as speed.
- **Failsafe:** `setFailsafe(1000)`: no commands for a second and the car stops by itself (lost link or a forgotten button).
- **Smooth start:** `setRamp(300)` removes jerks and wheel slip; `setMinPwm(80)` helps weak batteries when the motor hums and does not move at low speed.
- **Mounting without rewiring:** `reverseRight()` and `reverseLeft()` when a side goes the wrong way.
- **Phobo in one line:** `AlashCar car(AlashCar::PHOBO)` with the kit pins; your own pins in the constructor.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)` spins the sides one by one.
- Works on **Arduino AVR and ESP32** (ESP32 core 3.x with `analogWrite`).

### Method reference
| Method | What it does |
|---|---|
| `AlashCar(AlashCar::PHOBO) / AlashCar(r1,r2,re,l1,l2,le)` | Phobo pins / your own pins |
| `forward(s) backward(s) turnLeft(s) turnRight(s)` | Movement at speed -100..100 |
| `stop() / brake()` | Stop coasting / braking |
| `drive(speed, turn) / tank(l, r)` | Joystick / per side |
| `setRamp(ms) / setMinPwm(pwm) / setSpeedLimit(%)` | Ramp / dead zone / speed limit |
| `reverseRight() / reverseLeft()` | Reverse a side |
| `handleCommand(c) / handleStream(bt)` | Bluetooth commands |
| `setFailsafe(ms)` | Stop on lost link |
| `update()` | Call in loop() (ramp, failsafe) |
| `diagnose(Serial)` | Spins the sides one by one |

### If something does not work
**Motors hum but do not move**

Weak power or low speed. Feed 7-12 V to the driver and call `car.setMinPwm(80)`. The ENA and ENB jumpers on the L298N must be removed.

**The car goes backward when it should go forward, or turns the other way**

Reverse a side: `car.reverseRight()` or `car.reverseLeft()`. If the sides themselves are swapped, swap the pin triples in the constructor.

**The car keeps driving when Bluetooth disconnects**

Enable the failsafe: `car.setFailsafe(1000)` and call `car.update()` in loop().

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashCar
