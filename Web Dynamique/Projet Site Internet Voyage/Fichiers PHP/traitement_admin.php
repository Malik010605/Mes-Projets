<?php
session_start();
require_once "config.php";

function retourAdmin($message) {
  $_SESSION["message_admin"] = $message;
  header("Location: ../admin.php");
  exit();
}

function imageDestination($nom) {
  $nom = strtolower(trim($nom));

  if ($nom == "paris") {
    return "destination-paris.png";
  }

  if ($nom == "nice") {
    return "destination-nice.png";
  }

  if ($nom == "lyon") {
    return "destination-lyon.png";
  }

  return "destination-paris.png";
}

function trouverDestinationId($conn, $nom) {
  $sql = "SELECT id FROM destinations WHERE nom = ? AND statut = 'validee' LIMIT 1";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "s", $nom);
  mysqli_stmt_execute($stmt);
  $resultat = mysqli_stmt_get_result($stmt);
  $destination = mysqli_fetch_assoc($resultat);

  if ($destination) {
    return (int) $destination["id"];
  }

  return 0;
}

if (!$conn) {
  retourAdmin("Connexion à la base de données impossible.");
}

if (isset($_POST["ajouter_destination"])) {
  $nom = isset($_POST["nom"]) ? trim($_POST["nom"]) : "";
  $categorie = isset($_POST["categorie"]) ? trim($_POST["categorie"]) : "";
  $description = isset($_POST["description"]) ? trim($_POST["description"]) : "";
  $prix_base = isset($_POST["prix_base"]) ? (float) $_POST["prix_base"] : 0;
  $image = isset($_POST["image"]) ? basename(trim($_POST["image"])) : "destination-paris.png";
  $statut = "validee";

  if ($nom == "" || $categorie == "" || $description == "" || $prix_base < 0 || $image == "") {
    retourAdmin("Tous les champs de la destination doivent être remplis correctement.");
  }

  $sql = "INSERT INTO destinations (nom, categorie, description, prix_base, image, statut) VALUES (?, ?, ?, ?, ?, ?)";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "sssdss", $nom, $categorie, $description, $prix_base, $image, $statut);

  if (mysqli_stmt_execute($stmt)) {
    retourAdmin("La destination a été ajoutée au catalogue.");
  }

  retourAdmin("La destination n'a pas pu être ajoutée.");
}

if (isset($_POST["modifier_destination"])) {
  $destination_id = isset($_POST["destination_id"]) ? (int) $_POST["destination_id"] : 0;
  $nom = isset($_POST["nom"]) ? trim($_POST["nom"]) : "";
  $categorie = isset($_POST["categorie"]) ? trim($_POST["categorie"]) : "";
  $description = isset($_POST["description"]) ? trim($_POST["description"]) : "";
  $prix_base = isset($_POST["prix_base"]) ? (float) $_POST["prix_base"] : 0;
  $image = isset($_POST["image"]) ? basename(trim($_POST["image"])) : "";
  $statut = isset($_POST["statut"]) ? $_POST["statut"] : "validee";

  if ($destination_id <= 0 || $nom == "" || $categorie == "" || $description == "" || $prix_base < 0 || $image == "") {
    retourAdmin("La destination à modifier est incomplète.");
  }

  if ($statut != "validee" && $statut != "a_verifier" && $statut != "supprimee") {
    $statut = "validee";
  }

  $sql = "UPDATE destinations SET nom = ?, categorie = ?, description = ?, prix_base = ?, image = ?, statut = ? WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "sssdssi", $nom, $categorie, $description, $prix_base, $image, $statut, $destination_id);

  if (mysqli_stmt_execute($stmt)) {
    retourAdmin("La destination a été modifiée.");
  }

  retourAdmin("La destination n'a pas pu être modifiée.");
}

if (isset($_POST["supprimer_destination"])) {
  $destination_id = isset($_POST["destination_id"]) ? (int) $_POST["destination_id"] : 0;

  if ($destination_id <= 0) {
    retourAdmin("Aucune destination n'a été sélectionnée.");
  }

  $statut = "supprimee";
  $sql = "UPDATE destinations SET statut = ? WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "si", $statut, $destination_id);

  if (mysqli_stmt_execute($stmt)) {
    retourAdmin("La destination a été retirée du catalogue.");
  }

  retourAdmin("La destination n'a pas pu être supprimée.");
}

if (isset($_POST["modifier_role"])) {
  $utilisateur_id = isset($_POST["utilisateur_id"]) ? (int) $_POST["utilisateur_id"] : 0;
  $role = isset($_POST["role"]) ? $_POST["role"] : "voyageur";

  if ($role != "voyageur" && $role != "prestataire" && $role != "admin") {
    $role = "voyageur";
  }

  if ($utilisateur_id <= 0) {
    retourAdmin("Aucun utilisateur n'a été sélectionné.");
  }

  $sql = "UPDATE utilisateurs SET role = ? WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "si", $role, $utilisateur_id);

  if (mysqli_stmt_execute($stmt)) {
    retourAdmin("Le rôle de l'utilisateur a été modifié.");
  }

  retourAdmin("Le rôle n'a pas pu être modifié.");
}

if (isset($_POST["annuler_reservation"])) {
  $reservation_id = isset($_POST["reservation_id"]) ? (int) $_POST["reservation_id"] : 0;

  if ($reservation_id <= 0) {
    retourAdmin("Aucune réservation n'a été sélectionnée.");
  }

  $statut = "annulee";
  $sql = "UPDATE reservations SET statut = ? WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "si", $statut, $reservation_id);

  if (mysqli_stmt_execute($stmt)) {
    retourAdmin("La réservation a été annulée.");
  }

  retourAdmin("La réservation n'a pas pu être annulée.");
}

if (isset($_POST["verifier"])) {
  $prestation_id = isset($_POST["prestation_id"]) ? (int) $_POST["prestation_id"] : 0;

  if ($prestation_id <= 0) {
    retourAdmin("Aucune prestation n'a été sélectionnée.");
  }

  $sql = "SELECT * FROM demandes_prestataires WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "i", $prestation_id);
  mysqli_stmt_execute($stmt);
  $resultat = mysqli_stmt_get_result($stmt);
  $prestation = mysqli_fetch_assoc($resultat);

  if (!$prestation) {
    retourAdmin("La prestation demandée est introuvable.");
  }

  $statut = "validee";
  $sql = "UPDATE demandes_prestataires SET statut = ? WHERE id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "si", $statut, $prestation_id);
  mysqli_stmt_execute($stmt);

  $type = $prestation["type_prestation"];
  $nom = $prestation["nom_prestataire"];
  $destination = $prestation["destination"];
  $description = $prestation["description"];
  $prix = (float) $prestation["prix"];

  if ($type == "destination") {
    $categorie = "Prestataire";
    $image = imageDestination($destination);
    $sql = "INSERT INTO destinations (nom, categorie, description, prix_base, image, statut) VALUES (?, ?, ?, ?, ?, ?)";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "sssdss", $destination, $categorie, $description, $prix, $image, $statut);
    mysqli_stmt_execute($stmt);
  } elseif ($type == "hebergement") {
    $destination_id = trouverDestinationId($conn, $destination);
    if ($destination_id > 0) {
      $lieu = $destination;
      $capacite = 2;
      $disponible = 1;
      $image = "hebergement-voyage.png";
      $sql = "INSERT INTO hebergements (destination_id, nom, lieu, description, prix_nuit, capacite, disponible, image) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
      $stmt = mysqli_prepare($conn, $sql);
      mysqli_stmt_bind_param($stmt, "isssdiis", $destination_id, $nom, $lieu, $description, $prix, $capacite, $disponible, $image);
      mysqli_stmt_execute($stmt);
    }
  } elseif ($type == "activite") {
    $destination_id = trouverDestinationId($conn, $destination);
    if ($destination_id > 0) {
      $duree = "2h";
      $places = 10;
      $image = "activite-voyage.png";
      $sql = "INSERT INTO activites (destination_id, nom, description, duree, prix, places, image) VALUES (?, ?, ?, ?, ?, ?, ?)";
      $stmt = mysqli_prepare($conn, $sql);
      mysqli_stmt_bind_param($stmt, "isssdis", $destination_id, $nom, $description, $duree, $prix, $places, $image);
      mysqli_stmt_execute($stmt);
    }
  }

  retourAdmin("La prestation a été vérifiée et ajoutée au catalogue si possible.");
}

retourAdmin("Aucune action administrateur n'a été envoyée.");
?>
