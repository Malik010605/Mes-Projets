# Projet Digiwake – Réveil électronique avec Arduino

## Présentation

Digiwake est un réveil électronique entièrement fonctionnel développé avec Arduino. 
Ce projet a été réalisé dans le cadre de ma formation à l’ECE Paris, en C++ embarqué. Il combine programmation,
électronique et interaction utilisateur pour concevoir un appareil simple et intuitif.

Le réveil permet à l’utilisateur de régler l’heure actuelle et l’heure d’alarme, avec affichage sur écran LCD,
boutons de navigation, et déclenchement sonore à l’heure programmée.

## Objectifs du projet

- Concevoir un réveil avec affichage digital
- Gérer l’entrée utilisateur via des boutons physiques
- Programmer une alarme déclenchée à heure fixe
- Maîtriser l’utilisation de composants Arduino : RTC, écran LCD, buzzer, etc.

## Composants utilisés

- Carte Arduino ATMega328P
- Module RTC (Real-Time Clock)
- Écran LCD 16x2 avec interface I2C
- Buzzer actif
- Boutons poussoirs et Potentiomètre (pour réglage heure, minutes, validation)
- Résistances, câblage, breadboard

## Fonctionnalités

- Affichage de l’heure en temps réel sur écran LCD
- Réglage de l’heure et de l’alarme par l’utilisateur
- Détection du passage à l’heure d’alarme
- Déclenchement d’un signal sonore (buzzer)
- Possibilité d’interrompre ou de reporter l’alarme

## Améliorations possibles

- Ajout d’un écran OLED
- Sauvegarde de l’alarme en EEPROM
- Ajout d’un menu avec plusieurs alarmes
- Interface tactile (future version)
