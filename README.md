# AlashCar

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **машинки-робота** на драйверах **L298N / TB6612** (в том числе робот **Фобо** Alash Electronics): `car.forward(70)`, повороты, управление джойстиком `car.drive(скорость, поворот)` и **команды Bluetooth** от приложения BLE Controller (F, B, L, R, S). Плавный разгон и **защита: пропала связь, машинка остановилась сама**. Без зависимостей. Сделана для уроков и наборов Alash Electronics.

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
- **Любой драйвер моторов:** L298N, L293D, TB6612 (три пина на мотор), L9110S, HG7881, DRV8833, MX1508, BTS7960 (два ШИМ-пина), Cytron (DIR и ШИМ): `AlashMotor(AlashMotor::DRIVER_PWM_PWM, 5, 6)`.
- **2WD и 4WD:** `addLeft()` / `addRight()` добавляют второй мотор на сторону; левая и правая стороны могут быть на разных драйверах.
- **Особенности драйверов:** `setEnablePin()` для STBY / R_EN / L_EN, `setInverted()` для оптопар, `setPwmMax()` для 10 и 12 бит ШИМ и ESP8266 с ядром 2.x (диапазон 1023).
- **Простые команды:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`, `brake()`; скорость от -100 до 100.
- **Джойстик:** `drive(скорость, поворот)` смешивает левую и правую стороны, при перегрузке сам уменьшает обе пропорционально.
- **Bluetooth:** `car.handleStream(bt)` понимает F, B, L, R, S, Y (режим RC Car приложения BLE Controller), W A S D и цифры 0-9 как скорость.
- **Защита:** `setFailsafe(1000)`: нет команд дольше секунды, и машинка останавливается сама (потеряна связь или забыли отпустить кнопку).
- **Плавный старт:** `setRamp(300)` убирает рывки и срыв колёс; `setMinPwm(80)` помогает слабым батареям, когда мотор на малой скорости гудит и не едет.
- **Монтаж без перепайки:** `reverseRight()` и `reverseLeft()`, если сторона едет наоборот.
- **Фобо одной строкой:** `AlashCar car(AlashCar::PHOBO)` с пинами набора; свои пины в конструкторе.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)` по очереди крутит стороны.
- Работает на **Arduino AVR и ESP32** (ядро ESP32 3.x с `analogWrite`).

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

### Чем отличается от других библиотек
Мы посмотрели AlashMotorControlLite (наша), L298N (yohendry), Cytron Motor Driver и примеры из курса Фобо:
- AlashMotorControlLite управляет одним мотором; машинка — это два на сторону, смешивание поворота, команды и защита, которые приходилось писать в каждом проекте. Здесь это готово.
- В примерах нет защиты при потере Bluetooth: машинка продолжает ехать. У нас `setFailsafe()`.
- Нет плавного старта и обхода «мёртвой зоны» моторов; у нас `setRamp()` и `setMinPwm()`.
- Версия 1.1.0: не привязана к L298N: `AlashMotor` описывает L9110S, DRV8833, BTS7960, Cytron и другие драйверы, плюс 4WD и инверсия сигналов.

### Примеры
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`, `Driver_L9110`, `FourWheelDrive`

### Тесты
Уровни пинов и ШИМ для всех движений, торможение, смешивание `drive()` с нормировкой при переполнении, ограничение скорости, минимальный ШИМ, реверс сторон, плавный разгон по времени, команды Bluetooth (RC Car и W A S D, скорость цифрами), защита при потере связи проверены без железа: `extras/test`. Версия 1.1.0: драйверы с двумя ШИМ-пинами и DIR + ШИМ, вывод EN активный HIGH и LOW, инверсия сигналов, ШИМ на 10 бит и пересчёт minPwm, четыре мотора на разных драйверах, торможение и выбег каждого типа, разгон с любым драйвером.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**L298N / TB6612** драйверлері бар **робот-машинаға** (Alash Electronics **Фобо** роботын қоса) арналған қарапайым кітапхана: `car.forward(70)`, бұрылыстар, джойстикпен `car.drive(жылдамдық, бұрылыс)` және BLE Controller қосымшасынан **Bluetooth командалары**. Жайлап жүру және **қорғаныс: байланыс үзілсе, машина өзі тоқтайды**. Тәуелділіксіз.

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
- **Кез келген мотор драйвері:** L298N, L293D, TB6612 (мотор басына үш пин), L9110S, HG7881, DRV8833, MX1508, BTS7960 (екі ШИМ пині), Cytron (DIR және ШИМ): `AlashMotor(AlashMotor::DRIVER_PWM_PWM, 5, 6)`.
- **2WD және 4WD:** `addLeft()` / `addRight()` жаққа екінші моторды қосады; сол және оң жақ әртүрлі драйверде болуы мүмкін.
- **Драйвер ерекшеліктері:** STBY / R_EN / L_EN үшін `setEnablePin()`, оптопаралар үшін `setInverted()`, 10 және 12 бит ШИМ және 2.x ядролы ESP8266 (диапазон 1023) үшін `setPwmMax()`.
- **Қарапайым командалар:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`; жылдамдық -100...100.
- **Джойстик:** `drive(жылдамдық, бұрылыс)`.
- **Bluetooth:** `car.handleStream(bt)` F, B, L, R, S, Y және W A S D, 0-9 цифрларын түсінеді.
- **Қорғаныс:** `setFailsafe(1000)`: команда келмесе, машина өзі тоқтайды.
- **Жайлап бастау:** `setRamp(300)`; `setMinPwm(80)`.
- **Қайта дәнекерлеусіз:** `reverseRight()`, `reverseLeft()`.
- **Фобо бір жолмен:** `AlashCar car(AlashCar::PHOBO)`.
- **Түсінікті кеңестер** үш тілде.
- **Arduino AVR және ESP32**.

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

### Басқа кітапханалардан айырмашылығы
AlashMotorControlLite, L298N (yohendry), Cytron Motor Driver және Фобо курс мысалдарын қарадық:
- AlashMotorControlLite бір моторды басқарады; машина үшін қосымша қажет нәрселер осында дайын.
- Мысалдарда Bluetooth үзілгенде қорғаныс жоқ; бізде `setFailsafe()`.
- Жайлап бастау және `setMinPwm()` бізде бар.
- 1.1.0 нұсқасы L298N-ге байланбаған: `AlashMotor` L9110S, DRV8833, BTS7960, Cytron және басқа драйверлерді сипаттайды, 4WD және сигнал инверсиясы бар.

### Мысалдар
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`, `Driver_L9110`, `FourWheelDrive`

### Тесттер
Пин деңгейлері мен ШИМ, тежеу, `drive()` араластыруы, жылдамдықты шектеу, жайлап жүру, Bluetooth командалары, қорғаныс тексерілген: `extras/test`. 1.1.0 нұсқасы: екі ШИМ пинді және DIR + ШИМ драйверлері, EN шықпасы, сигнал инверсиясы, 10 биттік ШИМ, әртүрлі драйверлердегі төрт мотор, әр түрдің тежелуі.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for a **robot car** on **L298N / TB6612** drivers (including the Alash **Phobo** robot): `car.forward(70)`, turns, joystick control with `car.drive(speed, turn)` and **Bluetooth commands** from the BLE Controller app (F, B, L, R, S). Smooth acceleration and a **failsafe: if the link is lost the car stops by itself**. No dependencies. Made for classes and Alash Electronics kits.

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
- **Any motor driver:** L298N, L293D, TB6612 (three pins per motor), L9110S, HG7881, DRV8833, MX1508, BTS7960 (two PWM pins), Cytron (DIR and PWM): `AlashMotor(AlashMotor::DRIVER_PWM_PWM, 5, 6)`.
- **2WD and 4WD:** `addLeft()` / `addRight()` add a second motor to a side; the left and right sides can use different drivers.
- **Driver quirks:** `setEnablePin()` for STBY / R_EN / L_EN, `setInverted()` for optocouplers, `setPwmMax()` for 10 and 12 bit PWM and ESP8266 with core 2.x (range 1023).
- **Simple commands:** `forward()`, `backward()`, `turnLeft()`, `turnRight()`, `stop()`, `brake()`; speed from -100 to 100.
- **Joystick:** `drive(speed, turn)` mixes the left and right sides and scales both down proportionally on overflow.
- **Bluetooth:** `car.handleStream(bt)` understands F, B, L, R, S, Y (the RC Car mode of the BLE Controller app), W A S D and digits 0-9 as speed.
- **Failsafe:** `setFailsafe(1000)`: no commands for a second and the car stops by itself (lost link or a forgotten button).
- **Smooth start:** `setRamp(300)` removes jerks and wheel slip; `setMinPwm(80)` helps weak batteries when the motor hums and does not move at low speed.
- **Mounting without rewiring:** `reverseRight()` and `reverseLeft()` when a side goes the wrong way.
- **Phobo in one line:** `AlashCar car(AlashCar::PHOBO)` with the kit pins; your own pins in the constructor.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)` spins the sides one by one.
- Works on **Arduino AVR and ESP32** (ESP32 core 3.x with `analogWrite`).

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

### How it differs from other libraries
We looked at AlashMotorControlLite (ours), L298N (yohendry), Cytron Motor Driver and the Phobo course examples:
- AlashMotorControlLite drives one motor; a car needs two sides, turn mixing, commands and a failsafe, which every project had to write again. It is ready here.
- Examples have no protection against a lost Bluetooth link: the car keeps driving. We have `setFailsafe()`.
- No smooth start or dead-zone handling; we have `setRamp()` and `setMinPwm()`.
- Version 1.1.0 is not tied to the L298N: `AlashMotor` describes L9110S, DRV8833, BTS7960, Cytron and other drivers, plus 4WD and signal inversion.

### Examples
`Basic`, `Bluetooth_AVR`, `Joystick`, `Diagnose`, `Driver_L9110`, `FourWheelDrive`

### Tests
Pin levels and PWM for every movement, braking, `drive()` mixing with normalization, speed limit, minimum PWM, side reversal, time-based ramp, Bluetooth commands (RC Car and W A S D, digit speeds) and the failsafe are checked without hardware: `extras/test`. Version 1.1.0: drivers with two PWM pins and DIR + PWM, the EN pin active HIGH and LOW, signal inversion, 10-bit PWM and minPwm rescaling, four motors on different drivers, braking and coasting of each type, ramp with any driver.

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
