#define TRIG_PIN 10
#define ECHO_PIN 9

const float SOUND_CM_PER_US = 0.0343;        // speed of sound: 343 m/s = 0.0343 cm per microsecond
const float MIN_CM = 2.0;                    // HC-SR04 reliable range: 2 to 400 cm
const float MAX_CM = 400.0;
const unsigned long ECHO_TIMEOUT_US = 30000; // give up after 30 ms (about 5 m), instead of the 1 s default
const int NUM_SAMPLES = 5;

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) return -1;			// timed out: no echo came back

  float distance = duration * SOUND_CM_PER_US / 2;
  if (distance < MIN_CM || distance > MAX_CM) return -1;
  return distance;
}

float averageDistanceCm() {
  float sum = 0;
  int valid = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    float d = readDistanceCm();
    if (d >= 0) {
      sum += d;
      valid++;
    }
    delay(60);			// let the previous echo die out before pinging again
  }
  if (valid == 0) return -1;
  return sum / valid;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  float distance = averageDistanceCm();

  if (distance < 0) {
    Serial.println("Out of range");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance, 2);
    Serial.println(" cm");
  }
  delay(1000);
}
