# VoyageVista – Plateforme de planification de voyages

## Présentation

VoyageVista est une plateforme web dynamique développée dans le cadre du module **Web Dynamique** à l'**ECE Paris**.

L'objectif du projet est de concevoir une application permettant aux utilisateurs de rechercher, organiser et réserver un séjour en regroupant différents services tels que les destinations, les transports, les hébergements et les activités.

Le projet repose sur une architecture **client–serveur** avec une base de données **MySQL**, offrant une expérience utilisateur fluide ainsi qu'une gestion des comptes, des réservations et des différents rôles utilisateurs. :contentReference[oaicite:0]{index=0}

---

## Objectifs du projet

- Développer une application web dynamique complète
- Concevoir une architecture client–serveur maintenable
- Mettre en œuvre une base de données relationnelle
- Gérer plusieurs rôles utilisateurs
- Simuler un processus complet de réservation de voyage
- Développer une interface ergonomique et responsive

---

## Technologies utilisées

- HTML5
- CSS3
- JavaScript
- PHP
- MySQL
- Apache (XAMPP)

---

## Fonctionnalités

La plateforme permet notamment de :

### Gestion des utilisateurs

- Inscription et connexion
- Gestion des sessions
- Espace utilisateur
- Gestion des rôles (utilisateur, prestataire, administrateur)

### Catalogue touristique

- Consultation des destinations
- Recherche de séjours
- Consultation des hébergements
- Consultation des activités
- Consultation des moyens de transport

### Réservation

- Création d'un séjour personnalisé
- Ajout d'éléments dans un panier
- Validation des réservations
- Simulation du paiement
- Historique des réservations

### Administration

- Gestion des contenus
- Gestion des utilisateurs
- Gestion des prestataires
- Administration de la plateforme

---

## Structure du projet

```
Projet/
│
├── assets/
│   └── images/
│
├── Fichiers PHP/
│   ├── configuration
│   ├── traitements
│   └── gestion de la base de données
│
├── index.html
├── destinations.php
├── transports.php
├── hebergements.php
├── activites.php
├── sejour.php
├── panier.php
├── validation.php
├── connexion.php
├── inscription.php
├── admin.php
├── prestataire.php
├── compte.php
├── database.sql
└── style.css
```

---

## Compétences développées

- Développement Web Full Stack
- PHP orienté serveur
- Conception de bases de données MySQL
- Gestion des sessions utilisateurs
- Architecture client–serveur
- Développement d'interfaces responsives
- Organisation d'un projet web
- Travail en équipe avec Git

---

## Installation

1. Cloner le dépôt GitHub.
2. Placer le projet dans le dossier `htdocs` de XAMPP.
3. Importer le fichier `database.sql` dans **phpMyAdmin**.
4. Vérifier les paramètres de connexion à la base de données dans `config.php`.
5. Démarrer **Apache** et **MySQL**.
6. Accéder au projet depuis votre navigateur.

---

## Améliorations possibles

- Intégration d'une API de réservation réelle
- Paiement sécurisé
- Notifications par e-mail
- Gestion des favoris
- Carte interactive des destinations
- Avis et commentaires utilisateurs
- Tableau de bord statistique pour les administrateurs
- Application mobile

---

Ce projet m'a permis de mettre en pratique les concepts de **développement web dynamique**, de **gestion de bases de données**, de **programmation côté serveur** et de **conception d'une application web complète** en reproduisant les principales fonctionnalités d'une plateforme moderne de planification de voyages.
