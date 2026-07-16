# Quoridor Project

## Overview

This project is a **C implementation** of the board game **Quoridor**. It was developed as part of my preparatory engineering studies at **ECE Paris**. The objective was to build a complete terminal-based game while writing clean, modular code and faithfully implementing the official game rules.

The game is fully playable through a text-based interface in the terminal.

## Game Rules

Each player must reach the opposite side of the board by moving their pawn one square at a time or by placing walls to slow down their opponent.

The game is played on a **9×9 grid**, and each player has a limited number of walls. Pawn movements and wall placements follow the official rules, including path validation (no completely blocking an opponent) and jumping over adjacent pawns when allowed.

## Features

- Dynamic board rendering in the terminal
- Player movement following the official Quoridor rules
- Wall placement with validity checks (preventing complete blockage)
- Turn-based gameplay for two human players
- Coordinate-based system for easy interaction with the board
- Automatic win detection

## Code Organization

The project is organized into several source files:

- `main.c` – Program entry point
- `quoridor.c / quoridor.h` – Board display and game state management
- `joueur.c / joueur.h` – Player movement logic
- `plateau.c / plateau.h` – Wall validation and placement

## Compilation

Compile the project using `gcc`. For example:

```bash
gcc main.c plateau.c joueur.c quoridor.c -o quoridor
```
