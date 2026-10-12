#include <Servo.h>

const int NUM_JOINTS = 6;
const int POT_PINS[NUM_JOINTS]   = {A0, A1, A2, A3, A4, A5};
const int SERVO_PINS[NUM_JOINTS] = {8, 9, 10, 11, 12, 13};

Servo servos[NUM_JOINTS];

void setup()
{
  for (int i = 0; i < NUM_JOINTS; i++) {
    servos[i].attach(SERVO_PINS[i]);
  }
}

void loop()
{
  for (int i = 0; i < NUM_JOINTS; i++) {
    int angle = map(analogRead(POT_PINS[i]), 0, 1023, 0, 180);
    servos[i].write(angle);
  }
  delay(15);
}