CREATE DATABASE IF NOT EXISTS voyagevista_db
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

USE voyagevista_db;

DROP TABLE IF EXISTS reservation_details;
DROP TABLE IF EXISTS notifications;
DROP TABLE IF EXISTS reservations;
DROP TABLE IF EXISTS demandes_prestataires;
DROP TABLE IF EXISTS activites;
DROP TABLE IF EXISTS hebergements;
DROP TABLE IF EXISTS transports;
DROP TABLE IF EXISTS destinations;
DROP TABLE IF EXISTS utilisateurs;

CREATE TABLE utilisateurs (
  id INT AUTO_INCREMENT PRIMARY KEY,
  prenom VARCHAR(100) NOT NULL,
  nom VARCHAR(100) NOT NULL,
  email VARCHAR(150) NOT NULL UNIQUE,
  mot_de_passe VARCHAR(255) NOT NULL,
  role VARCHAR(30) NOT NULL DEFAULT 'voyageur',
  date_creation DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE destinations (
  id INT AUTO_INCREMENT PRIMARY KEY,
  nom VARCHAR(100) NOT NULL,
  categorie VARCHAR(100) NOT NULL,
  description TEXT NOT NULL,
  prix_base DECIMAL(10,2) NOT NULL,
  image VARCHAR(150) NOT NULL,
  statut VARCHAR(30) NOT NULL DEFAULT 'validee'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE transports (
  id INT AUTO_INCREMENT PRIMARY KEY,
  destination_id INT NOT NULL,
  type_transport VARCHAR(50) NOT NULL,
  ville_depart VARCHAR(100) NOT NULL,
  ville_arrivee VARCHAR(100) NOT NULL,
  duree VARCHAR(50) NOT NULL,
  prix DECIMAL(10,2) NOT NULL,
  places INT NOT NULL,
  date_depart DATE DEFAULT NULL,
  image VARCHAR(150) NOT NULL DEFAULT 'transport-voyage.png',
  CONSTRAINT fk_transports_destination
    FOREIGN KEY (destination_id) REFERENCES destinations(id)
    ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE hebergements (
  id INT AUTO_INCREMENT PRIMARY KEY,
  destination_id INT NOT NULL,
  nom VARCHAR(100) NOT NULL,
  lieu VARCHAR(100) NOT NULL,
  description TEXT NOT NULL,
  prix_nuit DECIMAL(10,2) NOT NULL,
  capacite INT NOT NULL,
  disponible TINYINT(1) NOT NULL DEFAULT 1,
  image VARCHAR(150) NOT NULL DEFAULT 'hebergement-voyage.png',
  CONSTRAINT fk_hebergements_destination
    FOREIGN KEY (destination_id) REFERENCES destinations(id)
    ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE activites (
  id INT AUTO_INCREMENT PRIMARY KEY,
  destination_id INT NOT NULL,
  nom VARCHAR(100) NOT NULL,
  description TEXT NOT NULL,
  duree VARCHAR(50) NOT NULL,
  prix DECIMAL(10,2) NOT NULL,
  places INT NOT NULL,
  date_activite DATE DEFAULT NULL,
  image VARCHAR(150) NOT NULL DEFAULT 'activite-voyage.png',
  CONSTRAINT fk_activites_destination
    FOREIGN KEY (destination_id) REFERENCES destinations(id)
    ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE demandes_prestataires (
  id INT AUTO_INCREMENT PRIMARY KEY,
  nom_prestataire VARCHAR(150) NOT NULL,
  email VARCHAR(150) NOT NULL,
  type_prestation VARCHAR(100) NOT NULL,
  destination VARCHAR(100) NOT NULL,
  description TEXT NOT NULL,
  prix DECIMAL(10,2) NOT NULL,
  statut VARCHAR(30) NOT NULL DEFAULT 'a_verifier',
  date_demande DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE reservations (
  id INT AUTO_INCREMENT PRIMARY KEY,
  utilisateur_id INT DEFAULT NULL,
  date_reservation DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  date_depart DATE NOT NULL,
  nombre_personnes INT NOT NULL,
  total DECIMAL(10,2) NOT NULL,
  statut VARCHAR(30) NOT NULL DEFAULT 'validee',
  CONSTRAINT fk_reservations_utilisateur
    FOREIGN KEY (utilisateur_id) REFERENCES utilisateurs(id)
    ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE reservation_details (
  id INT AUTO_INCREMENT PRIMARY KEY,
  reservation_id INT NOT NULL,
  destination_id INT NOT NULL,
  transport_id INT DEFAULT NULL,
  hebergement_id INT DEFAULT NULL,
  activite_id INT DEFAULT NULL,
  CONSTRAINT fk_details_reservation
    FOREIGN KEY (reservation_id) REFERENCES reservations(id)
    ON DELETE CASCADE,
  CONSTRAINT fk_details_destination
    FOREIGN KEY (destination_id) REFERENCES destinations(id)
    ON DELETE CASCADE,
  CONSTRAINT fk_details_transport
    FOREIGN KEY (transport_id) REFERENCES transports(id)
    ON DELETE SET NULL,
  CONSTRAINT fk_details_hebergement
    FOREIGN KEY (hebergement_id) REFERENCES hebergements(id)
    ON DELETE SET NULL,
  CONSTRAINT fk_details_activite
    FOREIGN KEY (activite_id) REFERENCES activites(id)
    ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE notifications (
  id INT AUTO_INCREMENT PRIMARY KEY,
  utilisateur_id INT NOT NULL,
  message TEXT NOT NULL,
  lu TINYINT(1) NOT NULL DEFAULT 0,
  date_notification DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT fk_notifications_utilisateur
    FOREIGN KEY (utilisateur_id) REFERENCES utilisateurs(id)
    ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

INSERT INTO utilisateurs (prenom, nom, email, mot_de_passe, role) VALUES
('Alice', 'Martin', 'alice@example.com', '$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2uheWG/igi.', 'voyageur'),
('Paul', 'Dubois', 'paul@example.com', '$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2uheWG/igi.', 'prestataire'),
('Admin', 'VoyageVista', 'admin@voyagevista.test', '$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2uheWG/igi.', 'admin');

INSERT INTO destinations (nom, categorie, description, prix_base, image, statut) VALUES
('Paris', 'Culture', 'Séjour culturel autour des musées, monuments et quartiers historiques.', 320.00, 'destination-paris.png', 'validee'),
('Nice', 'Mer', 'Séjour détente entre bord de mer, vieille ville et activités en extérieur.', 410.00, 'destination-nice.png', 'validee'),
('Lyon', 'Gastronomie', 'Week-end découverte autour de la ville, des visites et des restaurants.', 280.00, 'destination-lyon.png', 'validee');

INSERT INTO transports (destination_id, type_transport, ville_depart, ville_arrivee, duree, prix, places, date_depart, image) VALUES
(1, 'Train', 'Toulouse', 'Paris', '2h10', 65.00, 18, '2026-06-10', 'transport-voyage.png'),
(2, 'Avion', 'Paris', 'Nice', '1h25', 120.00, 12, '2026-06-12', 'transport-voyage.png'),
(3, 'Bus', 'Paris', 'Lyon', '4h00', 35.00, 25, '2026-06-15', 'transport-voyage.png');

INSERT INTO hebergements (destination_id, nom, lieu, description, prix_nuit, capacite, disponible, image) VALUES
(1, 'Hôtel Central', 'Paris', 'Chambre proche des transports et des musées.', 95.00, 2, 1, 'hebergement-voyage.png'),
(2, 'Résidence Azur', 'Nice', 'Appartement simple pour un séjour au bord de la mer.', 120.00, 4, 1, 'hebergement-voyage.png'),
(3, 'Auberge Lumière', 'Lyon', 'Hébergement économique pour un court séjour.', 70.00, 2, 1, 'hebergement-voyage.png');

INSERT INTO activites (destination_id, nom, description, duree, prix, places, date_activite, image) VALUES
(1, 'Visite guidée', 'Parcours accompagné dans les quartiers historiques.', '2h', 25.00, 20, '2026-06-11', 'activite-voyage.png'),
(2, 'Balade en bord de mer', 'Activité détente avec guide local.', '1h30', 18.00, 15, '2026-06-13', 'activite-voyage.png'),
(3, 'Atelier cuisine', 'Découverte simple de spécialités locales.', '3h', 45.00, 10, '2026-06-16', 'activite-voyage.png');

INSERT INTO demandes_prestataires (nom_prestataire, email, type_prestation, destination, description, prix, statut) VALUES
('Résidence Azur', 'contact@residence-azur.test', 'Hébergement', 'Nice', 'Proposition d''appartement proche de la mer.', 120.00, 'a_verifier'),
('Atelier Lyonnais', 'contact@atelier-lyonnais.test', 'Activité', 'Lyon', 'Atelier cuisine locale pour petits groupes.', 45.00, 'validee');

INSERT INTO reservations (utilisateur_id, date_depart, nombre_personnes, total, statut) VALUES
(1, '2026-06-10', 2, 505.00, 'validee');

INSERT INTO reservation_details (reservation_id, destination_id, transport_id, hebergement_id, activite_id) VALUES
(1, 1, 1, 1, 1);

INSERT INTO notifications (utilisateur_id, message, lu) VALUES
(1, 'Votre réservation pour Paris a été validée.', 0),
(2, 'Votre proposition de prestation est en attente de verification.', 0);
