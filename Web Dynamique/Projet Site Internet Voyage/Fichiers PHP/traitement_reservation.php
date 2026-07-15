<?php
session_start();
require_once "config.php";

$error = "";
$total = 0;

if (isset($_POST["valider_reservation"])) {
  $nom_carte = isset($_POST["nom_carte"]) ? trim($_POST["nom_carte"]) : "";
  $numero_carte = isset($_POST["numero_carte"]) ? preg_replace("/[^0-9]/", "", $_POST["numero_carte"]) : "";
  $expiration = isset($_POST["expiration"]) ? trim($_POST["expiration"]) : "";

  if (!isset($_SESSION["panier"]) || count($_SESSION["panier"]) == 0) {
    $error = "Le panier est vide.<br>";
  } elseif (!$conn) {
    $error = "Connexion à la base de données impossible.<br>";
  } elseif ($nom_carte == "" || strlen($numero_carte) < 12 || $expiration == "") {
    $error = "Les informations de paiement simulé sont incomplètes.<br>";
  } else {
    foreach ($_SESSION["panier"] as $item) {
      $total = $total + $item["prix"];
    }

    $sejour = isset($_SESSION["sejour"]) ? $_SESSION["sejour"] : array();
    $utilisateur_id = isset($_SESSION["utilisateur"]["id"]) ? (int) $_SESSION["utilisateur"]["id"] : null;
    $destination_id = isset($sejour["destination_id"]) ? (int) $sejour["destination_id"] : 0;
    $transport_id = isset($sejour["transport_id"]) ? (int) $sejour["transport_id"] : null;
    $hebergement_id = isset($sejour["hebergement_id"]) ? (int) $sejour["hebergement_id"] : null;
    $activite_id = isset($sejour["activite_id"]) ? $sejour["activite_id"] : null;
    $personnes = isset($sejour["personnes"]) ? (int) $sejour["personnes"] : 1;
    $date_depart = isset($sejour["date_depart"]) ? $sejour["date_depart"] : "";
    $statut = "validee";

    if ($destination_id <= 0 || empty($date_depart)) {
      $error = "Les informations du séjour sont incomplètes.<br>";
    }
  }

  if ($error == "") {
    $sql = "UPDATE transports SET places = places - ? WHERE id = ? AND places >= ?";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "iii", $personnes, $transport_id, $personnes);
    mysqli_stmt_execute($stmt);

    if (mysqli_stmt_affected_rows($stmt) == 0) {
      $error = "Le transport n'a plus assez de places disponibles.<br>";
    }
  }

  if ($error == "") {
    $disponible = 0;
    $sql = "UPDATE hebergements SET disponible = ? WHERE id = ? AND disponible = 1";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "ii", $disponible, $hebergement_id);
    mysqli_stmt_execute($stmt);

    if (mysqli_stmt_affected_rows($stmt) == 0) {
      $error = "L'hébergement n'est plus disponible.<br>";
    }
  }

  if ($error == "" && $activite_id) {
    $sql = "UPDATE activites SET places = places - ? WHERE id = ? AND places >= ?";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "iii", $personnes, $activite_id, $personnes);
    mysqli_stmt_execute($stmt);

    if (mysqli_stmt_affected_rows($stmt) == 0) {
      $error = "L'activité n'a plus assez de places disponibles.<br>";
    }
  }

  if ($error == "") {
    if ($utilisateur_id) {
      $sql = "INSERT INTO reservations (utilisateur_id, date_depart, nombre_personnes, total, statut) VALUES (?, ?, ?, ?, ?)";
      $stmt = mysqli_prepare($conn, $sql);
      mysqli_stmt_bind_param($stmt, "isids", $utilisateur_id, $date_depart, $personnes, $total, $statut);
    } else {
      $sql = "INSERT INTO reservations (date_depart, nombre_personnes, total, statut) VALUES (?, ?, ?, ?)";
      $stmt = mysqli_prepare($conn, $sql);
      mysqli_stmt_bind_param($stmt, "sids", $date_depart, $personnes, $total, $statut);
    }

    if (!mysqli_stmt_execute($stmt)) {
      $error = "La réservation n'a pas pu être enregistrée.<br>";
    } else {
      $reservation_id = mysqli_insert_id($conn);

      if ($activite_id) {
        $sql = "INSERT INTO reservation_details (reservation_id, destination_id, transport_id, hebergement_id, activite_id) VALUES (?, ?, ?, ?, ?)";
        $stmt = mysqli_prepare($conn, $sql);
        mysqli_stmt_bind_param($stmt, "iiiii", $reservation_id, $destination_id, $transport_id, $hebergement_id, $activite_id);
      } else {
        $sql = "INSERT INTO reservation_details (reservation_id, destination_id, transport_id, hebergement_id) VALUES (?, ?, ?, ?)";
        $stmt = mysqli_prepare($conn, $sql);
        mysqli_stmt_bind_param($stmt, "iiii", $reservation_id, $destination_id, $transport_id, $hebergement_id);
      }

      if (!mysqli_stmt_execute($stmt)) {
        $error = "Le détail de la réservation n'a pas pu être enregistré.<br>";
      }
    }
  }

  if ($error == "") {
    if ($utilisateur_id) {
      $message = "Votre réservation a été validée.";
      $sql = "INSERT INTO notifications (utilisateur_id, message) VALUES (?, ?)";
      $stmt = mysqli_prepare($conn, $sql);
      mysqli_stmt_bind_param($stmt, "is", $utilisateur_id, $message);
      mysqli_stmt_execute($stmt);
    }

    $_SESSION["reservation"] = array(
      "date_reservation" => date("Y-m-d"),
      "panier" => $_SESSION["panier"],
      "sejour" => $sejour,
      "total" => $total,
      "statut" => "Validee"
    );

    if (isset($_SESSION["utilisateur"])) {
      $_SESSION["notifications"][] = "Votre réservation a été validée.";
    }

    unset($_SESSION["panier"]);

    header("Location: ../validation.php");
    exit();
  }
} else {
  $error = "Aucune réservation n'a été envoyée.<br>";
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <title>VoyageVista - Reservation</title>
  <link rel="stylesheet" href="../style.css">
</head>
<body>
  <main>
    <section>
      <h1>Validation impossible</h1>
      <p class="error"><?php echo $error; ?></p>
      <p><a class="button-link" href="../panier.php">Retour au panier</a></p>
    </section>
  </main>
</body>
</html>
