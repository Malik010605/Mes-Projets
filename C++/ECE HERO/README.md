# Projet ECE HERO – Jeu de piano électronique (Arduino + Java)

## Présentation

ECE HERO est un jeu de rythme musical inspiré de Guitar Hero, développé dans le cadre d’un projet d’électronique
à l’ECE Paris. L’objectif est de synchroniser l’exécution d’un **piano électronique contrôlé par Arduino** avec une
**interface graphique Java** affichant les notes à jouer.

Le projet combine électronique embarquée, détection des touches via un microcontrôleur, communication série, et une
interface utilisateur dynamique.

## Objectifs pédagogiques

- Détecter en temps réel l’appui sur les touches d’un piano électronique via Arduino
- Transmettre les informations à un programme Java via port série
- Développer une interface graphique Java simulant le jeu en sa globalité
- Créer une interaction fluide entre le hardware (Arduino) et le software (Java)

## Composants utilisés

- Carte Arduino Uno ou Nano
- Boutons poussoirs (clavier de notes)
- Résistances, MOSFET, breadboard, Potentiomètres, NE555
- Logiciel Java 
- Port série pour la communication (USB)

## Fonctionnalités

### Côté Arduino :
- Détection des touches enfoncées
- Attribution de fréquénce aux touches
- Transmission des données via Serial (USB) au PC

### Côté Java :
- Lecture des données reçues via port série
- Interface graphique affichant les notes descendantes
- Détection en temps réel des correspondances entre notes attendues et touches jouées
- Système de score ou retour visuel en cas d’erreur/réussite

### Pour lancer l'interface graphique : 
- Créer un dossier du nom de votre choix
- Mettre le sketch dedans
- Créer un dossier dans ce même dossier que vous appelerez "data"
- Insérer les fichiers "Son Menu.mp3", "fond.jpg" et "logo ece.png"
