// [ru] AlashCar: полный привод 4WD. Два мотора на каждой стороне, драйверы могут быть разные: здесь левая сторона на двух L298N-каналах, правая на двух
//      каналах L9110S. addLeft() и addRight() добавляют второй мотор на сторону, он повторяет первый. Подключение пинов см. в комментариях ниже.
//      Для мекануму или независимого управления каждым колесом используйте AlashMotorControlLite.
// [kk] AlashCar: толық 4WD жетегі. Әр жақта екі мотор, драйверлер әртүрлі болуы мүмкін: мұнда сол жағы L298N-дің екі арнасында, оң жағы L9110S-тің екі арнасында.
//      addLeft() және addRight() жаққа екінші моторды қосады, ол біріншіні қайталайды. Пиндерді қосу төмендегі түсініктемелерде.
//      Меканум немесе әр дөңгелекті бөлек басқару үшін AlashMotorControlLite қолданыңыз.
// [en] AlashCar: a 4WD drive. Two motors on each side, the drivers can differ: here the left side is on two L298N channels, the right side on two
//      L9110S channels. addLeft() and addRight() add a second motor to a side, it repeats the first. The pin wiring is in the comments below.
//      For mecanum wheels or independent control of every wheel use AlashMotorControlLite.
#include <AlashCar.h>

// Левая сторона: L298N (IN1, IN2, EN): передний на пинах 2, 4, 3; задний на пинах 7, 8, 5
AlashMotor leftFront(AlashMotor::DRIVER_IN_IN_PWM, 2, 4, 3);
AlashMotor leftRear(AlashMotor::DRIVER_IN_IN_PWM, 7, 8, 5);
// Правая сторона: L9110S (два ШИМ-пина): передний на пинах 6, 9; задний на пинах 10, 11
AlashMotor rightFront(AlashMotor::DRIVER_PWM_PWM, 6, 9);
AlashMotor rightRear(AlashMotor::DRIVER_PWM_PWM, 10, 11);

AlashCar car(leftFront, rightFront);

void setup() {
  car.addLeft(leftRear);                          // второй мотор слева
  car.addRight(rightRear);                        // второй мотор справа
  car.begin();
}

void loop() {
  car.forward(80);  delay(2000);
  car.turnRight(70); delay(600);
  car.stop();       delay(1000);
}
