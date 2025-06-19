# Projets Arduino – C++

Ce dossier regroupe deux projets électroniques développés en **C++ embarqué avec Arduino**, dans le cadre
de ma formation à l’ECE Paris. Ces projets combinent programmation bas-niveau, conception électronique, et interaction utilisateur.

## 1. Digiwake – Réveil électronique

**Digiwake** est un réveil digital entièrement fonctionnel. Il utilise un module RTC pour garder l’heure,
un écran LCD pour l’affichage, des boutons pour la navigation, et un buzzer pour l’alarme.

### Fonctionnalités principales :
- Affichage de l’heure en temps réel
- Réglage manuel de l’heure et de l’alarme
- Déclenchement sonore à l’heure programmée
- Interaction via boutons physiques

### Composants :
- Arduino ATMega328P
- Module RTC 
- Écran LCD 16x2 (I2C)
- Buzzer
- Boutons poussoirs
- Potentiomètre

Le projet est entièrement documenté dans le dossier `Digiwake/`.

## 2. ECE HERO – Jeu de piano électronique

**ECE HERO** est un jeu musical interactif inspiré de Guitar Hero, adapté à un **piano électronique réel**.
Le projet combine un **contrôleur Arduino** qui détecte les touches jouées avec une **interface graphique en Java** qui affiche les notes à l’écran.

### Fonctionnalités principales :
- Détection des touches jouées sur un clavier électronique
- Interface graphique Java affichant les notes en temps réel
- Synchronisation entre l’Arduino et l’interface de jeu
- Système de score et feedback visuel

### Technologies utilisées :
- Arduino (C++)
- Java pour l’interface utilisateur
- Communication série entre Arduino et Ordinateur

Le projet est présenté dans le dossier `ECEHERO/`, avec code Arduino, code Java et documentation.

## Objectifs pédagogiques communs

- Développer des systèmes embarqués interactifs
- Structurer des projets multi-composants (software + hardware)
- Apprendre à gérer la communication entre différentes couches (microcontrôleur ↔ interface graphique)
- Mettre en pratique l’intégration électronique + logicielle

---

Chaque dossier de projet contient son propre `README.md`, les fichiers sources, et des explications détaillées.

Pour toute question ou suggestion, n'hésitez pas à me contacter.
