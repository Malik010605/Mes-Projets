# ECE HERO Project – Electronic Piano Rhythm Game (Arduino + Java)

## Overview

ECE HERO is a rhythm game inspired by *Guitar Hero*, developed as part of an electronics project at **ECE Paris**.

The objective is to synchronize an Arduino-controlled electronic piano with a Java graphical interface displaying the notes to be played in real time.

The project combines embedded electronics, key detection through a microcontroller, serial communication, and a dynamic graphical user interface.

## Project Objectives

- Detect key presses on an electronic piano in real time using Arduino
- Transmit the detected notes to a Java application via serial communication
- Develop a Java graphical interface simulating a complete rhythm game
- Create seamless interaction between the hardware (Arduino) and the software (Java)

## Components Used

- Arduino Uno or Nano
- Push buttons (piano keys)
- Resistors
- MOSFETs
- Breadboard
- Potentiometers
- NE555 Timer
- Java application
- USB serial communication

## Features

### Arduino Side

- Real-time key press detection
- Frequency assignment to each key
- Transmission of note data to the computer via the Serial (USB) interface

### Java Side

- Reading incoming serial data
- Graphical interface displaying falling notes
- Real-time matching between expected notes and played notes
- Scoring system and visual feedback for successful or incorrect inputs

## Running the Graphical Interface

To launch the Java application:

1. Create a folder with any name.
2. Place the project sketch inside this folder.
3. Create a subfolder named **`data`**.
4. Copy the following files into the **`data`** folder:
   - `Son Menu.mp3`
   - `fond.jpg`
   - `logo ece.png`
