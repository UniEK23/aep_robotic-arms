#include <Servo.h>

Servo base, shoulder, grip;

const int POT=A1, TRIG=10, ECHO=9, BUZ=11;

int mode = 0;            // 0-2 manual (base/shoulder/gripper), 3 auto
int ang = 0, dir = 1;
unsigned long nearSince = 0;
bool armed = true;

long readCm() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return d == 0 ? 999 : d * 0.034 / 2;
}

void checkGesture(long cm) {
  if (cm < 8) {
    if (nearSince == 0) nearSince = millis();
    if (armed && millis() - nearSince >= 1000) {
      mode = (mode + 1) % 4;
      tone(BUZ, 800 + mode * 300, 150);
      Serial.println(mode == 3 ? "AUTO" : "MANUAL " + String(mode));
      armed = false;
    }
  } else {
    nearSince = 0;
    if (cm > 12) armed = true;   // re-arm once hand moves away
  }
}

void pickAndPlace() {
  tone(BUZ, 1000, 200); delay(300);
  grip.write(90);      delay(300);  // open
  shoulder.write(120); delay(600);  // lower
  grip.write(30);      delay(500);  // close
  shoulder.write(60);  delay(600);  // lift
  base.write(0);       delay(800);  // go to drop zone
  grip.write(90);      delay(400);  // release
  tone(BUZ, 1500, 300);
  ang = 0; dir = 1;
}

void setup() {
  base.attach(5); shoulder.attach(4); grip.attach(3);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  pinMode(BUZ, OUTPUT);
  Serial.begin(9600);
  
  tone(BUZ, 1000, 500);
}

void loop() {
  long cm = readCm();
  // Serial.println(cm);
  checkGesture(cm);

  if (mode < 3) {
    int a = map(analogRead(POT), 0, 1023, 0, 180);
    if (mode == 0) base.write(a);
    if (mode == 1) shoulder.write(a);
    if (mode == 2) grip.write(a);
    delay(20);
  } else {
    base.write(ang); delay(15);
    if (cm >= 12 && cm < 40) pickAndPlace();
    ang += dir * 2;
    if (ang >= 180 || ang <= 0) dir = -dir;
  }
}