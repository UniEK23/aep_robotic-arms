#include <Servo.h>

// ---------- Pins ----------
const int FORCE_PIN  = A0;
const int BUTTON_PIN = 2;
const int SERVO_PIN  = 9;

// ------- Settings -------
const int OPEN_ANGLE     = 0;    // servo angle with the gripper fully open
const int CLOSED_ANGLE   = 90;   // servo angle with the gripper fully closed
const int STEP_DEG       = 1;    // degrees to close between force checks
const int GRIP_THRESHOLD = 300;  // force reading that counts as a firm grip
const int MAX_FORCE      = 800;  // force reading that is too much
const int NUM_SAMPLES    = 5;    // readings averaged for each force check
const int LOOP_DELAY_MS  = 30;   // pause per loop; this sets the closing speed

// ---------- State ----------
enum State { ST_OPEN, ST_CLOSING, ST_HOLDING };
State state = ST_OPEN;

Servo gripper;
int angle = OPEN_ANGLE;          // current servo position
bool wasPressed = false;         // button state from the previous loop
unsigned long lastPrint = 0;     // when the last status line was printed

// Average several readings so one noisy sample can't trigger a decision
int readForce() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(FORCE_PIN);
    delay(2);
  }
  return sum / NUM_SAMPLES;
}

// Move the servo, never outside the open-to-closed range
void setAngle(int newAngle) {
  angle = constrain(newAngle, OPEN_ANGLE, CLOSED_ANGLE);
  gripper.write(angle);
}

// Open fully and go back to the waiting state
void release() {
  setAngle(OPEN_ANGLE);
  state = ST_OPEN;
}

// Print a status line at most every 300 ms
void printStatus(const char* label, int force) {
  if (millis() - lastPrint >= 300) {
    lastPrint = millis();
    Serial.print(label);
    Serial.print("  angle=");
    Serial.print(angle);
    Serial.print("  force=");
    Serial.println(force);
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);   // wired to GND, so pressed = LOW
  gripper.attach(SERVO_PIN);
  gripper.write(OPEN_ANGLE);
  Serial.println("Gripper ready. Press the button to grip.");
}

void loop() {
  // Detect the moment the button goes down, not the whole time it is held
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);
  bool justPressed = pressed && !wasPressed;
  wasPressed = pressed;

  int force = readForce();

  switch (state) {

    case ST_OPEN:
      printStatus("OPEN   ", force);
      if (justPressed) {
        Serial.println("Closing...");
        state = ST_CLOSING;
      }
      break;

    case ST_CLOSING:
      if (justPressed) {                       // button pressed again: cancel
        Serial.println("Cancelled. Reopening.");
        release();
      } else if (force > MAX_FORCE) {          // squeezing too hard: protect the object
        Serial.println("Too much force! Releasing.");
        release();
      } else if (force >= GRIP_THRESHOLD) {    // firm enough: stop closing
        Serial.println("Firm grip reached. Holding.");
        state = ST_HOLDING;
      } else if (angle >= CLOSED_ANGLE) {      // fully closed but no force: nothing inside
        Serial.println("Nothing gripped. Reopening.");
        release();
      } else {                                 // not firm yet: close one more step
        setAngle(angle + STEP_DEG);
        printStatus("CLOSING", force);
      }
      break;

    case ST_HOLDING:
      printStatus("HOLDING", force);
      if (justPressed) {
        Serial.println("Released.");
        release();
      } else if (force > MAX_FORCE) {
        Serial.println("Too much force! Releasing.");
        release();
      }
      break;
  }

  delay(LOOP_DELAY_MS);
}
