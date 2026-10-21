# Force-Limit Gripper (Arduino + Force Sensor + Servo)

A robotic gripper that closes a little at a time and stops as soon as a force sensor decides the grip is firm enough. It holds soft objects gently (allegedly), holds firm objects securely (allegedly) and reports when there is nothing in the gripper.

Built in [Tinkercad Circuits](https://www.tinkercad.com/circuits) for Assignment 3 of the Aurora Robotics Educators Program (Robotic Arms track).

---

## The problem

So far I've been simulating robotic arms that move at fixed angles, but I realised this might be a potiential problem when it comes to the gripper. Two problems come up:

- **Soft or fragile objects get crushed**, such as an egg, fruit or a paper cup, because the gripper keeps closing to its set angle regardless of the object.
- **Firm objects can be held too loosely** if the angle was set for something bigger.

A fixed angle has no way of knowing what it is holding. The gripper needs a sense of "touch".

## The solution

Add a **force sensor** at the fingertip and close the gripper in small steps, checking the force after every step. When the force reaches a set limit, stop. The grip then adapts to the object instead of following a fixed angle.

This is **closed-loop control**: **sense** (how hard am I squeezing?) → **decide** (is that enough?) → **act** (squeeze a bit more, or stop), repeated many times a second.

## Components

| Component | Qty | Role |
| --- | --- | --- |
| Arduino Uno R3 | 1 | The microcontroller that runs the logic |
| Force sensor (FSR) | 1 | Senses grip force |
| Micro servo | 1 | Opens and closes the gripper |
| Pushbutton | 1 | Tells the gripper to grip or release |
| 10 kΩ resistor | 1 | Forms the voltage divider with the force sensor |
| Breadboard | 1 | Connections |
| Wires | several | Wiring |

## Wiring

| From | To |
| --- | --- |
| Arduino **5V** | Breadboard **+ rail** |
| Arduino **GND** | Breadboard **− rail** |
| Force sensor, pin 1 | **+ rail** (5V) |
| Force sensor, pin 2 | Breadboard column **X** (the "junction") |
| 10 kΩ resistor (one end) | Column **X** (the junction) |
| 10 kΩ resistor (second end) | GND on the breadboard |
| Arduino **A0** | Column **X** (the junction) |
| Pushbutton, one side | Arduino **D2** |
| Pushbutton, other side | **− rail** (GND) |
| Servo signal | Arduino **D9** |
| Servo power (red) | **+ rail** |
| Servo ground | **− rail** |

### The force sensor voltage divider

A force sensor is a variable resistor: the harder you press, the lower its resistance. The Arduino doesn't read resistance, only voltage, so the sensor is paired with a fixed 10 kΩ resistor to make a voltage divider:

More force means lower sensor resistance, which means a higher voltage at A0 and a higher `analogRead` value (0 to 1023). With no force the reading is close to 0.

### The button

The button uses the Arduino's built-in pull-up resistor (`INPUT_PULLUP`), so no external resistor is needed. D2 reads HIGH when the button is released and LOW when it is pressed.

## How it works

The gripper is basically a **state machine** with three states:

| State | What the gripper is doing |
| --- | --- |
| **OPEN** | Waiting with the gripper open. Pressing the button starts closing. |
| **CLOSING** | Closes `STEP_DEG` degrees, then reads the force, and repeats. |
| **HOLDING** | Stops moving and keeps the grip. Pressing the button releases it. |

### Key design decisions

- **Step and check.** Closing 1° at a time with a force check after each step is what makes a safe stop possible.
- **Averaged readings.** Each force check averages 5 `analogRead` samples, so one noisy reading cannot trigger a decision.
- **Edge detection on the button.** The loop runs many times during one press. The sketch reacts only to the moment the button goes down, so one press cannot close and release the gripper at once.

### Safety behaviour

| Situation | What the gripper does |
| --- | --- |
| Fully closed and the force is still low | Prints `Nothing gripped. Reopening.` and opens |
| Force goes above `MAX_FORCE` | Prints `Too much force! Releasing.` and opens |
| Button pressed again while closing | Cancels and reopens |

## Settings and calibration

| Constant | Default | Meaning |
| --- | --- | --- |
| `OPEN_ANGLE` | 0 | Servo angle with the gripper fully open |
| `CLOSED_ANGLE` | 90 | Servo angle with the gripper fully closed |
| `STEP_DEG` | 1 | Degrees closed between force checks (smaller is gentler and slower) |
| `GRIP_THRESHOLD` | 300 | Force reading that counts as a firm grip |
| `MAX_FORCE` | 800 | Force reading that is too much, so release |
| `NUM_SAMPLES` | 5 | Readings averaged per force check |
| `LOOP_DELAY_MS` | 30 | Pause per loop; sets the closing speed |

The defaults are just starting values. It will need to be calliberated for your own setup:

1. Run the simulation and watch the `force=` value in the Serial Monitor while you change the sensor's force.
2. Note the reading for a light touch and for a firm grip.
3. Set `GRIP_THRESHOLD` to the firm-grip reading and `MAX_FORCE` comfortably above it.
4. Set `OPEN_ANGLE` and `CLOSED_ANGLE` to match how the servo moves.

**Simulator note:** in Tinkercad the servo cannot physically press on the force sensor, so the force sensor's own control stands in for the object, hence, the reason for the earlier "allegedly".

## Example Serial Monitor output

From a test run of the sketch logic with a simulated soft object touching at 40°:

```
Gripper ready. Press the button to grip.
Closing...
CLOSING  angle=6  force=0
CLOSING  angle=14  force=0
CLOSING  angle=22  force=0
CLOSING  angle=30  force=0
CLOSING  angle=38  force=0
Firm grip reached. Holding.
HOLDING  angle=45  force=300
HOLDING  angle=45  force=300
Released.
OPEN     angle=0  force=0
```

## Limitations

- Tested in simulation only. Not yet tested on a physical gripper.
- The simulator does not model real sensor noise, so the averaging has little visible effect here but matters on real hardware.
- The servo is powered from the Arduino's 5V in this circuit. A small micro servo is fine for this case, but a larger servo will need its own power supply with its ground joined to the Arduino's.
- Once holding, the grip angle stays fixed. It does not tighten if the object slips.

## Author

**Ekan Etop Anaikot**: web developer, 3D artist and game developer, learning robotics and robotic arms through the Aurora Robotics Educators Program.
