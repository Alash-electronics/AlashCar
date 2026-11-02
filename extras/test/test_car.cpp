// Тесты AlashCar без железа (уровни пинов и ШИМ записываются имитацией):
//   g++ -std=c++17 -Wall -I mock -I ../../src test_car.cpp ../../src/AlashCar.cpp ../../src/AlashMotor.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "AlashCar.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
static void reset() { memset(g_level, 0, sizeof g_level); memset(g_pwm, 0, sizeof g_pwm); g_ms = 1000; }
// Пины Фобо: правая IN1=4 IN2=2 EN=5; левая IN3=8 IN4=12 EN=6
static bool right(int dir, int pwm) { return dir == 0 ? (g_level[4] == 0 && g_level[2] == 0 && g_pwm[5] == pwm) : dir > 0 ? (g_level[4] == 1 && g_level[2] == 0 && g_pwm[5] == pwm) : (g_level[4] == 0 && g_level[2] == 1 && g_pwm[5] == pwm); }
static bool left(int dir, int pwm) { return dir == 0 ? (g_level[8] == 0 && g_level[12] == 0 && g_pwm[6] == pwm) : dir > 0 ? (g_level[8] == 1 && g_level[12] == 0 && g_pwm[6] == pwm) : (g_level[8] == 0 && g_level[12] == 1 && g_pwm[6] == pwm); }
static void run(AlashCar &c, uint32_t ms) { for (uint32_t i = 0; i < ms; i++) { g_ms++; c.update(); } }

int main() {
  // 1) движения: вперёд, назад, повороты на месте, стоп; скорость 100 это ШИМ 255
  reset(); AlashCar c(AlashCar::PHOBO); c.begin();
  c.forward(100);  CHECK(right(1, 255) && left(1, 255));
  c.backward(50);  CHECK(right(-1, 127) && left(-1, 127));
  c.turnLeft(100); CHECK(right(1, 255) && left(-1, 255));
  c.turnRight(100); CHECK(right(-1, 255) && left(1, 255));
  c.stop();        CHECK(right(0, 0) && left(0, 0) && !c.isMoving());
  c.forward(70); CHECK(c.isMoving() && c.leftSpeed() == 70 && c.rightSpeed() == 70);

  // 2) тормоз: обе ножки HIGH
  c.brake(); CHECK(g_level[4] == 1 && g_level[2] == 1 && g_level[8] == 1 && g_level[12] == 1 && g_pwm[5] == 255);

  // 3) tank и drive: смешивание сторон и нормировка при переполнении
  c.tank(100, 30); CHECK(left(1, 255) && right(1, 76));
  c.drive(60, 0);  CHECK(c.leftSpeed() == 60 && c.rightSpeed() == 60);
  c.drive(0, 50);  CHECK(c.leftSpeed() == 50 && c.rightSpeed() == -50);          // поворот вправо на месте
  c.drive(80, 40); CHECK(c.leftSpeed() == 100 && c.rightSpeed() == 33);          // 120 и 40 нормируются к 100 и 33
  c.drive(-70, -70); CHECK(c.leftSpeed() == -100 && c.rightSpeed() == 0);        // 140 и 0 -> 100 и 0, знаки сохраняются
  c.drive(0, 0); CHECK(!c.isMoving() || (c.leftSpeed() == 0 && c.rightSpeed() == 0));

  // 4) ограничение скорости, мёртвая зона (minPwm), реверс стороны
  reset(); AlashCar l(AlashCar::PHOBO); l.begin(); l.setSpeedLimit(50);
  l.forward(100); CHECK(right(1, 127) && left(1, 127));
  l.setSpeedLimit(100); l.setMinPwm(80);
  l.forward(1);   CHECK(right(1, 80 + 175 * 1 / 100));                          // даже 1 % уже заставляет мотор крутиться
  l.forward(100); CHECK(right(1, 255));
  l.stop();       CHECK(right(0, 0));                                             // ноль остаётся нулём
  l.reverseRight(); l.forward(100); CHECK(right(-1, 255) && left(1, 255));
  l.reverseRight(false); l.reverseLeft(); l.forward(100); CHECK(right(1, 255) && left(-1, 255));

  // 5) плавный разгон: за 1000 мс от 0 до 100; остановка мгновенная
  reset(); AlashCar r(AlashCar::PHOBO); r.begin(); r.setRamp(1000);
  r.forward(100); CHECK(r.leftSpeed() == 0);
  run(r, 500);  CHECK(r.leftSpeed() >= 48 && r.leftSpeed() <= 52);
  run(r, 600);  CHECK(r.leftSpeed() == 100 && right(1, 255));
  r.stop();     CHECK(r.leftSpeed() == 0 && right(0, 0));
  r.forward(100); run(r, 200); r.backward(100); run(r, 100);                      // смена направления тоже плавная через ноль
  CHECK(r.leftSpeed() < 20);

  // 6) команды Bluetooth: F B L R S, WASD, цифры скорости
  reset(); AlashCar b(AlashCar::PHOBO); b.begin();
  CHECK(b.handleCommand('F') && b.leftSpeed() == 70);                             // 70 % по умолчанию
  CHECK(b.handleCommand('9') && b.handleCommand('F') && b.leftSpeed() == 100);
  CHECK(b.handleCommand('0') && b.handleCommand('F') && b.leftSpeed() == 0);
  b.handleCommand('5'); b.handleCommand('B'); CHECK(b.leftSpeed() == -55 && b.rightSpeed() == -55);
  b.handleCommand('L'); CHECK(b.leftSpeed() == -55 && b.rightSpeed() == 55);
  b.handleCommand('R'); CHECK(b.leftSpeed() == 55 && b.rightSpeed() == -55);
  b.handleCommand('S'); CHECK(!b.isMoving());                                     // в режиме RC Car S это стоп
  b.handleCommand('F'); b.handleCommand('Y'); CHECK(!b.isMoving());
  b.setCommandStyle(true);
  b.handleCommand('S'); CHECK(b.leftSpeed() == -55);                              // в W A S D клавиша S это назад
  b.handleCommand('X'); CHECK(!b.isMoving());
  CHECK(!b.handleCommand('?') && !b.handleCommand('z'));
  Stream bt; bt.buf = "9FL"; b.setCommandStyle(false); b.handleStream(bt);
  CHECK(bt.available() == 0 && b.leftSpeed() == -100 && b.rightSpeed() == 100);

  // 7) защита при потере связи: нет команд дольше failsafe, машинка стоит
  reset(); AlashCar f(AlashCar::PHOBO); f.begin(); f.setFailsafe(500);
  f.handleCommand('F'); run(f, 400); CHECK(f.isMoving());
  f.handleCommand('F'); run(f, 400); CHECK(f.isMoving());                          // команда обновила таймер
  run(f, 200); CHECK(!f.isMoving() && f.error() == AlashCar::ERR_FAILSAFE);
  f.handleCommand('F'); CHECK(f.isMoving() && f.error() == AlashCar::ERR_OK);
  reset(); AlashCar nf(AlashCar::PHOBO); nf.begin(); nf.forward(50); run(nf, 10000); CHECK(nf.isMoving());   // по умолчанию защита выключена

  // 8) свои пины
  reset(); AlashCar custom(10, 11, 3, 12, 13, 9); custom.begin(); custom.forward(100);
  CHECK(g_level[10] == 1 && g_level[11] == 0 && g_pwm[3] == 255 && g_level[12] == 1 && g_level[13] == 0 && g_pwm[9] == 255);

  // 9) диагностика на трёх языках
  reset(); AlashCar d(AlashCar::PHOBO); d.setLanguage(AlashCar::LANG_KK); const char *kk = (const char *)d.errorText();
  d.setLanguage(AlashCar::LANG_EN); d.diagnose(*(new Print));
  CHECK(kk != nullptr && !d.isMoving());

  // ===== Другие драйверы моторов (AlashMotor) =====
  {
    // L9110S / HG7881 / DRV8833: два ШИМ-пина на мотор (вперёд, назад)
    reset(); AlashMotor ml(AlashMotor::DRIVER_PWM_PWM, 14, 15), mr(AlashMotor::DRIVER_PWM_PWM, 16, 17);
    AlashCar c(ml, mr); c.begin();
    CHECK(g_pwm[14] == 0 && g_pwm[15] == 0 && g_pwm[16] == 0 && g_pwm[17] == 0);
    c.forward(100); CHECK(g_pwm[14] == 255 && g_pwm[15] == 0 && g_pwm[16] == 255 && g_pwm[17] == 0);
    c.backward(50); CHECK(g_pwm[14] == 0 && g_pwm[15] == 127 + 1 - 1 + 0 && g_pwm[17] == g_pwm[15]);     // 50 % от 255 это 127 (целая часть)
    c.stop(); CHECK(g_pwm[14] == 0 && g_pwm[15] == 0 && g_pwm[16] == 0 && g_pwm[17] == 0);
    c.brake(); CHECK(g_pwm[14] == 255 && g_pwm[15] == 255 && g_pwm[16] == 255 && g_pwm[17] == 255);       // обе ножки HIGH: торможение
    c.tank(100, -100); CHECK(g_pwm[14] == 255 && g_pwm[15] == 0 && g_pwm[16] == 0 && g_pwm[17] == 255);
    c.reverseRight(); c.tank(100, 100); CHECK(g_pwm[14] == 255 && g_pwm[15] == 0 && g_pwm[16] == 0 && g_pwm[17] == 255);   // реверс стороны
    c.reverseRight(false);
    c.setMinPwm(80); c.forward(1); CHECK(g_pwm[14] == 80 + (255 - 80) * 1 / 100);                       // minPwm работает и здесь
  }
  {
    // Cytron MD10 и платы DIR + PWM
    reset(); AlashMotor ml(AlashMotor::DRIVER_DIR_PWM, 18, 19), mr(AlashMotor::DRIVER_DIR_PWM, 20, 21);
    AlashCar c(ml, mr); c.begin();
    c.forward(100); CHECK(g_level[18] == 0 && g_pwm[19] == 255 && g_level[20] == 0 && g_pwm[21] == 255);
    c.backward(100); CHECK(g_level[18] == 1 && g_pwm[19] == 255 && g_level[20] == 1);
    c.stop(); CHECK(g_pwm[19] == 0 && g_pwm[21] == 0);
    c.brake(); CHECK(g_pwm[19] == 0);                                                                     // у DIR + PWM торможения нет: просто ноль
  }
  {
    // вывод EN / STBY (TB6612, BTS7960) включается в begin(); активный LOW тоже
    reset(); AlashMotor m(AlashMotor::DRIVER_PWM_PWM, 22, 23); m.setEnablePin(24, true); AlashMotor n(AlashMotor::DRIVER_PWM_PWM, 25, 26); n.setEnablePin(27, false);
    AlashCar c(m, n); c.begin(); CHECK(g_level[24] == 1 && g_level[27] == 0);
    m.enable(false); n.enable(false); CHECK(g_level[24] == 0 && g_level[27] == 1);
    m.enable(true); CHECK(g_level[24] == 1);
    AlashMotor free_(AlashMotor::DRIVER_PWM_PWM, 28, 29); free_.enable(true); CHECK(g_level[28] == 0);   // без вывода EN ничего не пишется
  }
  {
    // инверсия сигналов (оптопары): уровни и ШИМ наоборот
    reset(); AlashMotor ml(AlashMotor::DRIVER_IN_IN_PWM, 30, 31, 32), mr(AlashMotor::DRIVER_IN_IN_PWM, 33, 34, 35); ml.setInverted(true); mr.setInverted(true);
    AlashCar c(ml, mr); c.begin();
    CHECK(g_level[30] == 1 && g_level[31] == 1 && g_pwm[32] == 255);                                      // стоп = «всё высоким» при инверсии
    c.forward(100); CHECK(g_level[30] == 0 && g_level[31] == 1 && g_pwm[32] == 0);                         // полная скорость: IN1 низкий, ШИМ ноль (инвертирован)
    c.stop(); CHECK(g_level[30] == 1 && g_level[31] == 1 && g_pwm[32] == 255);
  }
  {
    // разрядность ШИМ: 10 бит; minPwm пересчитывается
    reset(); AlashCar c(4, 2, 5, 8, 12, 6); c.setPwmMax(1023); c.begin();
    c.forward(100); CHECK(g_pwm[5] == 1023 && g_pwm[6] == 1023);
    c.setMinPwm(80); c.forward(1); CHECK(g_pwm[5] == 80 * 1023 / 255 + (1023 - 80 * 1023 / 255) * 1 / 100);
    c.setMinPwm(0); c.forward(50); CHECK(g_pwm[5] == 511);
    AlashMotor m(AlashMotor::DRIVER_IN_IN_PWM, 1, 2, 3); m.setPwmMax(0); CHECK(m.pwmMax() == 255);          // 0 превращается в 255
    m.setPwmMax(100); m.set(500); CHECK(m.current() == 100); m.set(-500); CHECK(m.current() == -100);      // ограничивается максимумом
  }
  {
    // четыре мотора: по два на сторону, разные драйверы на сторонах
    reset(); AlashMotor lf(AlashMotor::DRIVER_IN_IN_PWM, 4, 2, 5), lr(AlashMotor::DRIVER_PWM_PWM, 14, 15), rf(AlashMotor::DRIVER_DIR_PWM, 18, 19), rr(AlashMotor::DRIVER_PWM_PWM, 22, 23);
    AlashCar c(lf, rf); c.addLeft(lr); c.addRight(rr); c.addLeft(lr); c.addRight(rr);                      // лишние игнорируются (максимум два)
    c.begin();
    c.forward(100);
    CHECK(g_level[4] == 1 && g_pwm[5] == 255 && g_pwm[14] == 255 && g_pwm[15] == 0 && g_level[18] == 0 && g_pwm[19] == 255 && g_pwm[22] == 255);
    c.turnLeft(100);
    CHECK(g_level[4] == 0 && g_level[2] == 1 && g_pwm[15] == 255 && g_pwm[14] == 0 && g_level[18] == 0 && g_pwm[22] == 255);   // левые назад, правые вперёд
    c.stop(); CHECK(g_pwm[5] == 0 && g_pwm[14] == 0 && g_pwm[19] == 0 && g_pwm[22] == 0);
    c.brake(); CHECK(g_level[4] == 1 && g_level[2] == 1 && g_pwm[14] == 255 && g_pwm[15] == 255 && g_pwm[23] == 255);
    c.setPwmMax(1023); c.forward(100); CHECK(g_pwm[5] == 1023 && g_pwm[14] == 1023 && g_pwm[19] == 1023);   // setPwmMax действует на все моторы
  }
  {
    // плавный разгон работает с любым драйвером
    reset(); AlashMotor ml(AlashMotor::DRIVER_PWM_PWM, 14, 15), mr(AlashMotor::DRIVER_PWM_PWM, 16, 17); AlashCar c(ml, mr); c.begin(); c.setRamp(1000);
    c.forward(100); CHECK(g_pwm[14] == 0); g_ms += 500; c.update(); CHECK(g_pwm[14] > 100 && g_pwm[14] < 160); g_ms += 600; c.update(); CHECK(g_pwm[14] == 255);
  }

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
