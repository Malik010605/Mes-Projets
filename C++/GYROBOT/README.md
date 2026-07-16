# Gyrobot – DrawBot

## Overview

Gyrobot is a robotics project developed as part of the **Closed-Loop Control Systems** course at **ECE Paris**.

The objective is to design a mobile robot capable of **automatically drawing various geometric shapes** while using **closed-loop feedback control** to ensure accurate movements.

The robot is built on the **Gyrobot** platform powered by an **ESP32 (NodeMCU)**. It is remotely controlled through a web interface over Wi-Fi and relies on several sensors (wheel encoders, IMU, and magnetometer) to continuously correct its trajectory.

---

## Project Objectives

- Design an autonomous mobile drawing robot
- Develop closed-loop control systems
- Implement PID controllers
- Process data from multiple sensors
- Apply concepts of control engineering and robotics
- Develop a wireless control interface

---

## Hardware Used

- NodeMCU ESP32
- Gyrobot platform
- Two geared DC motors with wheel encoders
- LSM6DS3 Inertial Measurement Unit (IMU)
- LIS3MDL Magnetometer
- DRV8837 Motor Drivers
- Wi-Fi communication
- Pen mounted on the robot for drawing

---

## Technologies Used

- C++
- PlatformIO
- ESP32
- Arduino Framework
- Wi-Fi
- LittleFS
- PID_v1
- HTML / CSS / JavaScript (embedded web interface)

---

## Features

The project includes the following features:

- Control the robot through a Wi-Fi web interface
- Independently control both motors
- Read wheel encoder data
- Process IMU sensor data
- Use the magnetometer for orientation
- Compute the robot's odometry
- Automatically correct the trajectory using PID controllers
- Record and replay drawing trajectories

---

## Implemented Sequences

### Sequence 1 – Stair Pattern

The robot automatically draws a series of straight lines connected by 90° turns.

**Skills involved:**

- Position control
- Distance control
- Angle control
- Wheel encoder processing

---

### Sequence 2 – Parametric Circle

The robot draws a circle whose radius can be selected directly from the user interface.

**Features:**

- Adjustable radius
- Dual-wheel synchronization
- Circle closure error compensation
- Radius calibration

---

### Sequence 3 – North Orientation

The robot automatically aligns itself with magnetic North before drawing a figure.

This functionality relies on:

- The magnetometer
- The inertial measurement unit (IMU)
- Automatic heading correction

---

## Skills Developed

- Mobile robotics
- Control engineering
- Digital feedback control
- PID controller tuning
- Odometry
- Embedded Wi-Fi communication
- Sensor data acquisition and processing
- Embedded development on ESP32
- Embedded web interface development
- Experimental validation

---

## Possible Improvements

- Support for complex SVG drawings
- Autonomous waypoint navigation
- Environment mapping
- Automatic sensor calibration
- Advanced speed control
- Real-time robot position visualization
- Automatic PID tuning

---

This project allowed me to deepen my knowledge of **robotics**, **control engineering**, **embedded programming**, and **closed-loop control systems**, while designing a complete architecture combining electronics, embedded software, and a web-based user interface.
