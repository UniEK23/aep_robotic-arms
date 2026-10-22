# Arduino Traffic Light System

## Overview

This project simulates a basic traffic light system using an Arduino and three LEDs. The system simulates a traffic signal by controlling the LEDs in a predefined sequence with specific time intervals.

The project demonstrates fundamental concepts in embedded systems programming, including digital output control, timing and sequential execution using C++.

## Features

- Three-state traffic light sequence: Red, Green, and Yellow.
- Configurable duration for each light.
- Automatic, continuous operation.
- Simple implementation using Arduino digital output pins.

## Components Required

- Arduino Uno
- 1 red LED
- 1 green LED
- 1 yellow LED
- 3 current-limiting resistors (220Ω–330Ω)
- Breadboard
- Wires

## Circuit Connections

| Component | Arduino Pin |
| --- | --- |
| Red LED | Digital Pin 13 |
| Yellow LED | Digital Pin 12 |
| Green LED | Digital Pin 11 |

Connect each LED in series with a current-limiting resistor. Connect the cathode of each LED to GND.

## Traffic Light Timing

| State | Active LED | Duration |
| --- | --- | --- |
| 1 | Red | 5 seconds |
| 2 | Green | 6 seconds |
| 3 | Yellow | 1.5 seconds |

**Total cycle duration:** 12.5 seconds, excluding minor execution overhead.

The system repeats this sequence continuously.

## How It Works

### 1. Initialization

The `setup()` function configures the three LED pins as outputs using `pinMode()`.

### 2. Red Phase

The red LED turns on while the green and yellow LEDs remain off. The program waits for 5,000 milliseconds.

### 3. Green Phase

The red LED turns off, and the green LED turns on. The program waits for 6,000 milliseconds.

### 4. Yellow Phase

The green LED turns off, and the yellow LED turns on. The program waits for 1,500 milliseconds.

### 5. Repetition

Once the yellow phase ends, the `loop()` function starts the sequence again with the red LED.

` ` `

## Limitations

- The `delay()` function blocks normal program execution while waiting.
- The sequence uses fixed durations rather than dynamically adjusting to traffic conditions.
- This is an educational prototype and is not intended for controlling real road traffic.

## Possible Improvements

- Replace `delay()` with `millis()` for non-blocking timing.
- Add a pedestrian crossing button.
- Introduce a countdown display.
- Add sensors to detect vehicles.
- Expand the system to control multiple traffic signals.

## Learning Objectives

This project provides practical experience with Arduino programming, electronic components, GPIO configuration, time-based control and sequential logic.

## Author

Ekan Anaikot
