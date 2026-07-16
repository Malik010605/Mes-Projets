# Arduino Projects – C++

This repository contains several embedded systems projects developed in **C++ using Arduino and ESP32**, as part of my engineering studies at **ECE Paris**. These projects combine low-level programming, electronics, control systems, and user interaction through real-world embedded applications.

---

# 1. Digiwake – Electronic Alarm Clock

Digiwake is a fully functional digital alarm clock. It uses an RTC module to keep track of time, an LCD screen for display, push buttons for navigation, and a buzzer to trigger the alarm.

## Main Features

- Real-time clock display
- Manual time and alarm configuration
- Alarm triggered at the programmed time
- User interaction through physical buttons

## Components

- Arduino ATMega328P
- RTC Module
- 16x2 LCD Display (I2C)
- Buzzer
- Push Buttons
- Potentiometer

The project is fully documented in the **Digiwake/** folder.

---

# 2. ECE HERO – Electronic Piano Game

ECE HERO is an interactive music game inspired by *Guitar Hero*, adapted to a real electronic piano. The project combines an Arduino controller that detects played notes with a Java graphical interface displaying notes in real time.

## Main Features

- Detection of keys played on an electronic keyboard
- Java graphical interface displaying notes in real time
- Synchronization between the Arduino and the Java application
- Scoring system and visual feedback

## Technologies Used

- Arduino (C++)
- Java
- Serial communication between the Arduino and the computer

The project is available in the **ECEHERO/** folder, including the Arduino code, Java application, and documentation.

---

# 3. Gyrobot – DrawBot (Closed-Loop Control Systems)

Gyrobot is a robotics project developed as part of the **Closed-Loop Control Systems** course. The goal was to design an autonomous robot capable of drawing various geometric shapes using feedback control loops and PID controllers.

The robot is powered by a **NodeMCU ESP32** and relies on several sensors (wheel encoders, IMU, and magnetometer) to accurately control its movement and orientation.

## Main Features

- Closed-loop motor control
- Wheel position control
- Wireless communication between the computer and the robot
- Drawing lines, circles, and oriented shapes
- PID controller tuning and validation
- Processing data from wheel encoders, IMU, and magnetometer

## Technologies Used

- ESP32 (NodeMCU)
- Embedded C++
- PID Controllers
- Inertial Measurement Unit (IMU)
- Magnetometer
- Wheel Encoders
- Wi-Fi Communication

## Skills Developed

- Motor control
- Digital feedback control
- PID tuning
- Sensor data acquisition and processing
- Mobile robotics
- Embedded systems design
- Experimental validation

The project is available in the **Gyrobot/** folder, including the source code, reports, and documentation.

---

# Common Learning Objectives

- Develop interactive embedded systems
- Design hardware/software architectures
- Program microcontrollers (Arduino and ESP32)
- Manage communication between different layers (microcontroller ↔ graphical interface ↔ computer)
- Implement control algorithms and feedback systems
- Develop skills in electronics, automation, and robotics

---

Each project folder contains its own **README.md**, source code, and associated documentation.

Feel free to contact me if you have any questions or suggestions.
