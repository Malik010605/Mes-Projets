# Projet Quorridor

## Présentation

Ce projet est une implémentation en C du jeu de plateau **Quoridor**. Il a été réalisé dans le cadre de ma formation
en classe préparatoire à l’ECE Paris. L’objectif était de développer un jeu complet en langage C, en structurant proprement
le code et en respectant les règles officielles du jeu.

Le jeu est entièrement jouable en terminal, avec une interface textuelle.

## Règles du jeu

Chaque joueur doit atteindre le côté opposé du plateau en déplaçant son pion d’une case à la fois ou en plaçant des barrières
pour ralentir l’adversaire.  
Le plateau est une grille de 9x9 cases. Chaque joueur dispose d’un nombre limité de barrières. Les déplacements et placements
sont régis par des règles strictes (pas de blocage total, saut d’un pion adjacent, etc.).

## Fonctionnalités

- Affichage dynamique du plateau en terminal
- Déplacement des joueurs selon les règles officielles
- Placement des barrières avec vérification de validité (pas de blocage)
- Tour par tour entre deux joueurs humains
- Système de coordonnées pour interagir facilement avec le plateau
- Détection automatique de la victoire

## Organisation du code

Le projet est structuré en plusieurs fichiers :

- `main.c` : point d'entrée du programme
- `quoridor.c / quoridor.h` : gestion de l’affichage et de l’état du plateau
- `joueur.c / joueur.h` : fonctions liées aux déplacements des pions
- `plateau.c / plateau.h` : vérification et placement des barrières

## Compilation

Utilisez `gcc` pour compiler le projet. Exemple :

```bash
gcc main.c plateau.c joueur.c quoridor.c -o quoridor
