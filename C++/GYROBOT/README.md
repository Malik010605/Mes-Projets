# Gyrobot – DrawBot

## Présentation

Gyrobot est un projet de robotique réalisé dans le cadre du module **Systèmes Bouclés** à l'ECE Paris.

L'objectif est de développer un robot mobile capable de **dessiner automatiquement différentes figures géométriques** tout en utilisant des **boucles d'asservissement** afin de garantir la précision de ses déplacements.

Le robot est basé sur une plateforme **Gyrobot** pilotée par un **ESP32 (NodeMCU)**. Il est commandé à distance depuis une interface web via le Wi-Fi et utilise plusieurs capteurs (encodeurs, centrale inertielle et magnétomètre) pour corriger sa trajectoire en temps réel.

---

## Objectifs du projet

- Concevoir un robot mobile autonome capable de dessiner
- Développer des asservissements en boucle fermée
- Implémenter des correcteurs PID
- Exploiter les données de plusieurs capteurs
- Mettre en pratique les notions d'automatique et de robotique
- Développer une interface de commande sans fil

---

## Matériel utilisé

- NodeMCU ESP32
- Plateforme Gyrobot
- Deux motoréducteurs avec encodeurs
- Centrale inertielle LSM6DS3 (IMU)
- Magnétomètre LIS3MDL
- Drivers moteurs DRV8837
- Communication Wi-Fi
- Stylo monté sur le robot pour le dessin

---

## Technologies utilisées

- C++
- PlatformIO
- ESP32
- Arduino Framework
- Wi-Fi
- LittleFS
- PID_v1
- HTML / CSS / JavaScript (interface web embarquée)

---

## Fonctionnalités

Le projet permet notamment de :

- Commander le robot via une interface web accessible en Wi-Fi
- Piloter indépendamment les deux moteurs
- Lire les encodeurs des roues
- Exploiter les données de la centrale inertielle (IMU)
- Utiliser le magnétomètre pour l'orientation
- Calculer l'odométrie du robot
- Corriger automatiquement la trajectoire grâce à des correcteurs PID
- Enregistrer et rejouer des trajectoires

---

## Séquences réalisées

### Séquence 1 – Escalier

Le robot dessine automatiquement une succession de segments droits avec des rotations à 90°.

Compétences mises en œuvre :

- Asservissement en position
- Contrôle des distances
- Contrôle des angles
- Exploitation des encodeurs

---

### Séquence 2 – Cercle paramétrable

Le robot dessine un cercle dont le rayon peut être choisi directement depuis l'interface utilisateur.

Fonctionnalités :

- Rayon configurable
- Synchronisation des deux roues
- Compensation des erreurs de fermeture
- Calibration du rayon

---

### Séquence 3 – Orientation Nord

Le robot s'oriente automatiquement vers le Nord magnétique avant de dessiner une figure.

Cette fonctionnalité repose sur :

- Le magnétomètre
- La centrale inertielle
- Une correction automatique de l'orientation

---

## Compétences développées

- Robotique mobile
- Automatique
- Asservissement numérique
- Correcteurs PID
- Odométrie
- Communication Wi-Fi embarquée
- Acquisition de données capteurs
- Développement embarqué sur ESP32
- Interface Web embarquée
- Validation expérimentale

---

## Améliorations possibles

- Dessin de formes complexes (SVG)
- Navigation autonome par points de passage
- Cartographie de l'environnement
- Calibration automatique des capteurs
- Contrôle de vitesse plus avancé
- Visualisation en temps réel de la position du robot
- Optimisation des PID par auto-réglage

---

Ce projet m'a permis d'approfondir les notions de **robotique**, **automatique**, **programmation embarquée** et **commande en boucle fermée**, tout en développant une architecture complète mêlant électronique, logiciel embarqué et interface utilisateur.
