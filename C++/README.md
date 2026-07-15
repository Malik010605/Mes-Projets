# Projets Arduino – C++

Ce dépôt regroupe plusieurs projets électroniques développés en **C++ embarqué avec Arduino et ESP32**, dans le cadre de ma formation à **l'ECE Paris**. Ces projets combinent programmation bas niveau, électronique, automatique et interaction utilisateur à travers des systèmes embarqués concrets.

---

# 1. Digiwake – Réveil électronique

Digiwake est un réveil digital entièrement fonctionnel. Il utilise un module RTC pour conserver l'heure, un écran LCD pour l'affichage, des boutons pour la navigation et un buzzer pour le déclenchement de l'alarme.

## Fonctionnalités principales

- Affichage de l'heure en temps réel
- Réglage manuel de l'heure et de l'alarme
- Déclenchement sonore à l'heure programmée
- Interaction via boutons physiques

## Composants

- Arduino ATMega328P
- Module RTC
- Écran LCD 16x2 (I2C)
- Buzzer
- Boutons poussoirs
- Potentiomètre

Le projet est entièrement documenté dans le dossier **Digiwake/**.

---

# 2. ECE HERO – Jeu de piano électronique

ECE HERO est un jeu musical interactif inspiré de Guitar Hero, adapté à un piano électronique réel. Le projet combine un contrôleur Arduino qui détecte les touches jouées avec une interface graphique Java affichant les notes en temps réel.

## Fonctionnalités principales

- Détection des touches jouées sur un clavier électronique
- Interface graphique Java affichant les notes en temps réel
- Synchronisation entre l'Arduino et l'application Java
- Système de score et feedback visuel

## Technologies utilisées

- Arduino (C++)
- Java
- Communication série entre Arduino et ordinateur

Le projet est présenté dans le dossier **ECEHERO/** avec le code Arduino, le code Java ainsi que la documentation.

---

# 3. Gyrobot – DrawBot (Systèmes Bouclés)

Gyrobot est un projet de robotique réalisé dans le cadre du module **Systèmes Bouclés**. L'objectif est de développer un robot autonome capable de dessiner différentes figures géométriques en utilisant des **boucles d'asservissement** et des **correcteurs PID**.

Le robot est piloté par une carte **NodeMCU ESP32** et utilise plusieurs capteurs (encodeurs, IMU et magnétomètre) afin de contrôler précisément ses déplacements et son orientation. :contentReference[oaicite:0]{index=0}

## Fonctionnalités principales

- Pilotage des moteurs en boucle fermée
- Asservissement en position des roues
- Communication sans fil entre l'ordinateur et le robot
- Dessin de lignes, cercles et figures orientées
- Réglage et validation de correcteurs PID
- Exploitation des données issues des encodeurs, de l'IMU et du magnétomètre

## Technologies utilisées

- ESP32 (NodeMCU)
- C++ embarqué
- Correcteurs PID
- Capteurs inertiels (IMU)
- Magnétomètre
- Encodeurs de roues
- Communication Wi-Fi

## Compétences développées

- Commande de moteurs
- Asservissement numérique
- Régulation PID
- Acquisition et traitement de données capteurs
- Robotique mobile
- Conception de systèmes embarqués
- Validation expérimentale

Le projet est présenté dans le dossier **Gyrobot/** avec le code source, les rapports ainsi que la documentation.

---

# Objectifs pédagogiques communs

- Développer des systèmes embarqués interactifs
- Concevoir des architectures mêlant matériel et logiciel
- Programmer des microcontrôleurs (Arduino et ESP32)
- Maîtriser les communications entre différentes couches (microcontrôleur ↔ interface graphique ↔ ordinateur)
- Mettre en œuvre des algorithmes de contrôle et d'asservissement
- Développer des compétences en électronique, automatique et robotique

---

Chaque dossier de projet contient son propre **README.md**, les fichiers sources ainsi que la documentation associée.

Pour toute question ou suggestion, n'hésitez pas à me contacter.
