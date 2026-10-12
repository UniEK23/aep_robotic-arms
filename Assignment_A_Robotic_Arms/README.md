# 6-DOF Robotic Arm Joint Simulation (Arduino + Tinkercad)

An Arduino Uno simulation of the joint controls of a 6-degree-of-freedom robotic arm. Six potentiometers act as the "operator controls", and each one drives its own servo, which stands in for one joint of the arm. Turn a knob and the matching joint moves between 0° and 180°.

Built and tested in [Tinkercad Circuits](https://www.tinkercad.com/circuits).

---

## What it does

- Reads 6 analog inputs (potentiometers) on every pass of the main loop
- Converts each reading (0-1023) to a servo angle (0-180°)
- Writes each angle to its matching servo, so all 6 joints move independently and at the same time
- Powers the servos from a separate battery pack so the Arduino's 5 V rail isn't overloaded

## Hardware

| Component | Qty | Role |
|---|---|---|
| Arduino Uno R3 | 1 | Reads the pots, generates the servo signals |
| Potentiometer | 6 | One control knob per joint |
| Micro servo | 6 | One servo per joint |
| 4 x AAA battery pack (6 V) | 1 | Separate power for the servos |
| Breadboard | 1 | Shared power and ground rails |

## Pin mapping

| Joint | Potentiometer pin | Servo pin |
|---|---|---|
| 1 | A0 | D8 |
| 2 | A1 | D9 |
| 3 | A2 | D10 |
| 4 | A3 | D11 |
| 5 | A4 | D12 |
| 6 | A5 | D13 |

**Wiring notes**
- Each pot: one outer pin to 5 V, the other outer pin to GND, the middle (wiper) pin to its analog input.
- Each servo: signal wire to its digital pin, red to the battery pack's positive rail, brown/black to the negative rail.
- The battery pack's ground must be connected to the Arduino's GND so the servo signals share a common reference.

## How it works

```
potentiometer  ->  analogRead()  ->  map(0..1023 -> 0..180)  ->  servo.write()
   (knob)          10-bit ADC         rescale to degrees          hold that angle
```

The Servo library sends a pulse to each servo about every 20 ms. The pulse width (roughly 0.5-2.4 ms) tells the servo which angle to hold. `servo.write()` only updates the target angle, and the servo moves there on its own.

## The problem I solved

My first working version controlled each joint with its own copy-pasted lines:

- 6 separate `Servo` objects
- 6 pin constants
- 6 `value` variables and 6 `angle` variables
- 18 near-identical lines in `loop()` (read, map, write, six times each)

It worked, but it didn't scale. Adding a 7th joint meant editing the code in six different places, and one typo in a single digit could break a single joint without any obvious error.

**The fix:** I replaced the repeated variables with arrays and handled each joint in a `for` loop.

| | Version 1 | Version 2 |
|---|---|---|
| Lines of code | 68 | 23 |
| Separate declarations | 24 | 4 |
| Lines to add a joint | ~6 edits in different places | Change `NUM_JOINTS`, add one entry to each pin array |

The behavior is identical, but the code is shorter, easier to read and much harder to get wrong.

```cpp
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
```

## How I built it

I built the circuit up in stages instead of wiring everything at once:

1. **1 pot, 1 servo:** confirmed the basic read, map and write chain.
2. **2 pots, 2 servos:** confirmed that two joints could run independently.
3. **6 pots, 6 servos:** full 6-DOF layout, first with the long version of the code.
4. **Refactor:** rewrote the code with arrays and loops (v2).

## How to run it

1. Create a new circuit in Tinkercad and build the layout from the tables above.
2. Open the **Code** panel, switch to text mode, and paste in the sketch.
3. Click **Start Simulation**.
4. Click and drag the potentiometer knobs. Each one moves its matching servo.

To run it on real hardware, upload the same sketch to an Arduino Uno and power the servos from an external supply rather than the Uno's 5 V pin, with the grounds joined.

## Limitations

- This simulates the **control electronics only**. The servos spin in place, and there are no arm links, weight or physics.
- There is no smoothing, so fast knob movements make the servos jump straight to the new angle.
- Every joint uses the full 0-180° range, which a real arm would limit per joint.

## Ideas for next steps

- Add per-joint minimum and maximum angle limits
- Smooth the readings to reduce jitter
- Stream the six angles over serial to a 3D arm model so the joints move real links

## Author

**Ekan Etop Anaikot**: web developer, 3D artist and game developer, learning robotics and robotic arms. Open to collaborations, internships and freelance work.
