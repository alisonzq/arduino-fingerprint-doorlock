#include <Servo.h>

#define SERVO_PIN 9

Servo servo;
void setup() {
  // put your setup code here, to run once:
  servo.attach(SERVO_PIN);
  delay(50);
  servo.write(25);
  delay(1000);
  servo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:

}
