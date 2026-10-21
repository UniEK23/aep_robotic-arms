# Ultrasonic Distance Sensor (HC-SR04 + Arduino Uno)

An Arduino sketch that measures distance with an HC-SR04 ultrasonic sensor and prints live readings to the Serial Monitor. It averages several measurements for a steadier value and flags readings that make no sense instead of printing them as real distances.

Built for Week 4 (Sensors and Communication) of the Aurora Robotics Educators Program, Robotic Arms track.

---

## What it does

- Fires a 10 µs trigger pulse and times the echo with `pulseIn()`
- Converts the echo time to centimetres
- Takes 5 readings per output and averages the valid ones
- Prints `Out of range` when there is no usable echo, instead of a wrong number
- Updates once per second at 9600 baud

Example Serial Monitor output:

```
Distance: 130.13 cm
Distance: 130.10 cm
Out of range
Distance: 130.60 cm
```

## Components

Built in a circuit simulator ([Tinkercad Circuits](https://www.tinkercad.com/circuits)). The same wiring and sketch are meant to work on a physical board, but I have not tested it on real hardware yet.

| Component | Qty |
| --- | --- |
| Arduino Uno | 1 |
| HC-SR04 ultrasonic sensor | 1 |
| Breadboard | 1 |
| Jumper wires | 4 |

## Wiring

| HC-SR04 pin | Arduino pin | Purpose |
| --- | --- | --- |
| VCC | 5V | Power the sensor |
| GND | GND | Shared ground |
| Trig | D9 | 10 µs start pulse out |
| Echo | D10 | Echo pulse in |

The sensor and the Arduino must share GND. Without a common ground you will read 0 cm and nothing else.

## How it works

```
Trig LOW 2 µs -> Trig HIGH 10 µs -> Trig LOW
        |
  sensor sends 8 bursts of 40 kHz sound by itself
        |
  Echo goes HIGH for as long as the sound is travelling
        |
  pulseIn(ECHO, HIGH) times that pulse in microseconds
        |
  distance (cm) = duration (µs) x 0.0343 / 2
```

Sound travels at about 343 m/s, which is 0.0343 cm per microsecond. The pulse goes to the object and back, so the time is divided by 2 to get the one-way distance. For example, a 1166 µs echo gives 1166 x 0.0343 / 2, which is about 20 cm.

## Problems I solved

**1. Bad readings looked like real ones.**
When no echo comes back, `pulseIn()` returns 0, and the first version of the sketch printed that as `Distance: 0.00 cm`. The HC-SR04 is only reliable from 2 to 400 cm, so the sketch now rejects anything outside that range, as well as timeouts, and prints `Out of range`.

**2. A missing echo froze the loop.**
`pulseIn()` waits up to 1 second by default. Every missed echo stalled the whole sketch for a full second before the normal delay even started. I pass a 30 ms timeout (about 5 m, past the sensor's range), so a missed echo costs 30 ms instead.

**3. Single readings were noisy.**
One bad echo could jump the output around. The sketch now takes 5 readings, ignores the invalid ones, and averages the rest, so a single bad reading no longer spoils the result. It waits 60 ms between pings so the previous echo has died out before the next one starts.

| | First version | Final version |
| --- | --- | --- |
| Handles no echo | Prints `0.00 cm` | Prints `Out of range` |
| Handles under 2 cm or over 400 cm | No | Yes |
| Time lost on a missed echo | 1 s | 30 ms |
| Readings per output | 1 | 5, averaged |

## The code

[`ultrasonic/ultrasonic.ino`](ultrasonic/ultrasonic.ino)

Key settings at the top of the sketch:

| Constant | Value | Meaning |
| --- | --- | --- |
| `SOUND_CM_PER_US` | 0.0343 | Speed of sound in cm per µs |
| `MIN_CM` / `MAX_CM` | 2 / 400 | Reliable range of the HC-SR04 |
| `ECHO_TIMEOUT_US` | 30000 | How long to wait for an echo |
| `NUM_SAMPLES` | 5 | Readings averaged per output |

## How to run it

**In the simulator**

1. Build the wiring above and paste the sketch into the Code panel (text mode).
2. Click **Start Simulation** and open the Serial Monitor.
3. Move the sensor's object closer and farther and watch the readings follow it.

**On a real Arduino Uno**

1. Wire the sensor as shown above.
2. Open `ultrasonic/ultrasonic.ino` in the Arduino IDE, select **Arduino Uno** and your port, then upload.
3. Open the Serial Monitor at **9600 baud** and compare the readings with a ruler.

## Testing the out-of-range handling

The simulator keeps the object inside the sensor's range, so the "Out of range" branches never trigger on their own. To exercise them, temporarily tighten the limits at the top of the sketch, test, then put the original values back.

| Branch to test | Temporary change | What to do | Expected output |
| --- | --- | --- | --- |
| Too far | `MAX_CM = 100` | Move the object to about 150 cm | `Out of range` |
| Too close | `MIN_CM = 30` | Move the object to about 10 cm | `Out of range` |
| No echo (timeout) | `ECHO_TIMEOUT_US = 3000` | Move the object past about 50 cm | `Out of range` |
| Normal | original values | Object at about 20 cm | `Distance: 20.00 cm` (roughly) |

## Surfaces that give bad echoes

I could not test this part without a physical sensor, so these notes come from how ultrasonic sensing works. They are not measurements from my own setup.

| Surface or situation | Why it gives a bad echo | Likely result |
| --- | --- | --- |
| Soft materials (fabric, foam, clothing) | They absorb the sound instead of reflecting it | Weak or no echo, so `Out of range` |
| Flat surface tilted away from the sensor | The sound bounces off at an angle and never returns to the receiver | No echo, or a reading that jumps around |
| Very small or thin objects (a pen, a thin pole) | Too little surface to reflect a usable echo | Missed readings |
| Curved or irregular surfaces | The echo scatters in many directions | Noisy readings |
| Objects off to the side of the 15° cone | The sensor only listens in a narrow cone | Reads the wrong object, or nothing |
| Several surfaces close together | The sound reflects more than once before returning | Distance that is too long |

The best target is a flat, hard surface facing the sensor directly. To check the notes above, I would repeat the same distance with each surface on real hardware.

## Limitations

- Not yet tested on a physical HC-SR04. The simulator does not model sound absorption, angled surfaces or noise, so it cannot show the bad echoes described above.
- The speed of sound changes slightly with temperature, so readings drift a little in very hot or cold rooms. The sketch assumes about 20 °C.
- The sensor sees a 15° cone, so it reports the nearest object in that cone, not necessarily the one you are pointing at.
- Averaging makes each output about 0.3 s slower, which is fine for a display but too slow for fast obstacle avoidance.

## Author

**Ekan Etop Anaikot**: web developer, 3D artist and game developer, learning robotics and robotic arms through the Aurora Robotics Educators Program.
