Installation Du Projet VoyageVista
__________________________________

1) Installer et lancer MAMP
Veuillez démarrer les services suivants :
• Apache
• MySQL

2) Placer le projet dans le dossier htdocs de MAMP.
Le dossier du projet doit être placé ici :
C:\MAMP\htdocs\Projet Piscine 2026

3) Importer la base de données.
Veuillez ouvrir phpMyAdmin :
http://localhost/phpMyAdmin/
Puis importer le fichier :
database.sql
L’importation doit créer la base de données :
voyagevista_db

4) Vérifier la configuration de connexion.
Le fichier de configuration se trouve ici :
Fichiers PHP/config.php
La configuration utilisée est :
$servername = "localhost";
$username = "root";
$password = "root";
$dbname = "voyagevista_db";
$port = 3306;
Si votre installation MAMP utilise un mot de passe vide pour MySQL, veuillez remplacer :
$password = "root";
par :
$password = "";

5) Lancer le site.
Veuillez ouvrir l’adresse suivante dans le navigateur :
http://localhost/Projet%20Piscine%202026/index.html