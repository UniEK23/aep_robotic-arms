# Robotic Arm Capstone: Manual + Autonomous Control (Tinkercad)

A simulated 3-joint robotic arm that you can drive by hand with a knob, or let run on its own using a distance sensor. Built in Tinkercad with an Arduino Uno as the capstone for the **Aurora Educators Program (AEP), Robotic Arms track**.

**Tinkercad circuit:** (https://www.tinkercad.com/things/kdIKciD8UJ1-roboticarmcapstone?sharecode=2wLQdPfzfIeYYq6piBK8LpTk7OQQ-Z6QWqpUG5AajUI)

---

## What it does

The arm has three joints: a **base** that rotates, a **shoulder** that lifts and lowers, and a **gripper** that opens and closes.

It has two ways of working:

- **Manual mode:** you turn a potentiometer (a knob), and the selected joint follows it.
- **Autonomous mode:** the arm sweeps left and right on its own. When its ultrasonic sensor spots an object, it beeps, picks it up, drops it in a side zone, and goes back to sweeping.

There are no buttons. You switch modes by holding something close to the sensor (see [Switching modes](#switching-modes)).

---

## Components

Only the five component types the brief allowed:

| Component | Qty | Job |
|---|---|---|
| Arduino Uno (microcontroller) | 1 | The brain. Reads inputs, decides, drives outputs |
| Servo motors | 3 | The joints: base, shoulder, gripper |
| Potentiometer | 1 | The manual control knob |
| HC-SR04 ultrasonic sensor | 1 | Measures distance to find objects, and acts as the mode switch |
| Piezo buzzer | 1 | Audio feedback for mode changes and pickups |

---

## Wiring

| Part | Connects to |
|---|---|
| Potentiometer (middle pin) | A1 |
| Ultrasonic Trig | D10 |
| Ultrasonic Echo | D9 |
| Buzzer (+) | D11 |
| Base servo signal | `[fill in pin from your code]` |
| Shoulder servo signal | `[fill in pin from your code]` |
| Gripper servo signal | `[fill in pin from your code]` |
| All grounds | GND |
| Servos, sensor, pot power | 5V |

> On real hardware, servos should be powered from a separate 5 to 6 V supply, with its ground joined to the Arduino's ground. In the Tinkercad simulation, the Uno's 5V is fine.

---

## How it works

Every robot follows the same loop: **sense, decide, act**.

- **Sense:** the potentiometer and the ultrasonic sensor.
- **Decide:** the Arduino code, controlled by one variable called `mode`.
- **Act:** the servos move and the buzzer sounds.

The program runs `loop()` over and over. Each time around, it does three things:

1. Measure the distance with the ultrasonic sensor.
2. Check whether someone is holding an object close to switch modes.
3. Run the code for the current mode.

### The four modes

| `mode` | Name | What happens |
|---|---|---|
| 0 | Manual: Base | The potentiometer controls the base |
| 1 | Manual: Shoulder | The potentiometer controls the shoulder |
| 2 | Manual: Gripper | The potentiometer controls the gripper |
| 3 | Autonomous | The arm sweeps and picks objects on its own |

One knob can only control one thing at a time, so modes 0 to 2 let a single potentiometer handle all three joints.

### Manual control

The knob gives a number from 0 to 1023. The code rescales it to an angle from 0 to 180 degrees with `map()`, then sends it to the selected servo with `write()`. Turn the knob, and the joint follows in real time.

### Autonomous control

1. The base sweeps from 0 to 180 degrees and back, two degrees at a time.
2. If the sensor reads an object between 12 and 40 cm away, the arm runs `pickAndPlace()`: beep, open the gripper, lower the shoulder, close the gripper, lift, swing to the drop zone, release, and beep again.
3. The arm then **waits until the object is cleared** (the distance goes above 40 cm) before it starts sweeping again. This stops it from picking the same object forever.

### Measuring distance

The HC-SR04 sends out a burst of sound and listens for the echo. The code times the echo and converts it to centimetres:

```
distance (cm) = echo time (microseconds) x 0.034 / 2
```

It divides by 2 because the sound travels to the object *and back*. If no echo comes back, the code returns 999 so a missing reading is never mistaken for "0 cm".

### Switching modes

There is no button. Instead, the ultrasonic sensor doubles as a gesture switch:

- **Hold an object closer than 8 cm for 1 second** and the mode advances to the next one (after Autonomous it wraps back to Manual: Base).
- The buzzer plays a different pitch for each mode, and the Serial Monitor prints the new mode (`MANUAL 0`, `MANUAL 1`, `MANUAL 2`, or `AUTO`).
- The switch re-arms only after the object moves farther than 12 cm, so one gesture never triggers several changes.

The distance zones are kept apart on purpose, so the two uses of the sensor never clash:

| Distance | Meaning |
|---|---|
| Under 8 cm (held 1 s) | Change mode |
| 12 to 40 cm (in Autonomous) | Object detected, pick it up |
| Over 40 cm | Nothing there, keep sweeping |

---

## How to run it

1. Open the Tinkercad link above (or build the circuit from the wiring table).
2. Open the **Code** panel and paste in `robotic_arm_capstone.ino` from this repo.
3. Click **Start Simulation** and open the **Serial Monitor**.
4. Click the ultrasonic sensor in Tinkercad. A distance control appears: drag it to move the "object" closer or farther.
5. The arm starts in **Autonomous** mode. Try this:
   - Keep the distance high (over 40 cm): the base sweeps.
   - Set the distance to about 25 cm: the arm beeps, picks, and drops.
   - Move the distance back above 40 cm: the sweep resumes.
   - Hold the distance under 8 cm for a second: mode changes to **Manual: Base**. Turn the potentiometer and the base follows.
   - Repeat the gesture to move through the shoulder, the gripper, and back to Autonomous.

---

## Test results

| # | Test | Result |
|---|---|---|
| 1 | Distance at 100 cm | Base sweeps 0 to 180 and back |
| 2 | Hold under 8 cm for 1 s | Mode changes, buzzer beeps, Serial prints the mode |
| 3 | Turn the pot in each manual mode | Only the selected joint moves |
| 4 | Cycle through all four modes | Returns to Autonomous after Manual: Gripper |
| 5 | Object at 25 cm in Autonomous | Beep, one full pick and drop |
| 6 | Move object away (over 40 cm) | Sweep resumes |

---

## Limitations

- **Blocking delays.** The pick sequence uses `delay()`, so while it runs the arm ignores all input. A `millis()`-based timer would fix this.
- **Simulated sensor.** In Tinkercad the distance is a slider, so the sensor is not physically pointed where the base points. It acts as a sensor watching a pickup zone in front of the arm.
- **Joint jumps.** When you switch modes, the joint can jump because the knob is at a different position than the joint it just took over.
- **Simulation only.** On real servos, the angles for open, close, and lower would need tuning.

## Ideas for improvement

- Replace `delay()` with non-blocking timing using `millis()`
- Average several ultrasonic readings for a steadier distance
- Add Bluetooth (HC-05) control from a phone
- Scale up to a full 6-DOF arm using a PCA9685 servo driver and a separate power supply

---

## What this project uses from the course

| Week | Lesson | Used here for |
|---|---|---|
| 1 | Sense, decide, act; microcontrollers | The overall design |
| 2 | Variables, `if / else`, Serial | The `mode` variable and debug output |
| 3 | Servos, PWM, read-map-write | Potentiometer driving the servos |
| 4 | Ultrasonic ranging, out-of-range checks | Object detection and the mode gesture |
| 5 | Commands and protocols | Treating a gesture as a command |

A full write-up of what I learned in the track is in `What_I_Learned.pdf`.

---

## Repository contents

```
.
├── README.md
├── robotic_arm_capstone.ino
├── images/
│   └── circuit.png
└── What_I_Learned.pdf
```

---

Built by **Ekan** for the Aurora Educators Program, Robotic Arms track, by Aurora Robotics.
