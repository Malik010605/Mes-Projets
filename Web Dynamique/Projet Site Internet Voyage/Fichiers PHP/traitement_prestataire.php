<?php
session_start();
require_once "config.php";

$nom_prestataire = "";
$email = "";
$type_prestation = "";
$destination = "";
$description = "";
$prix = 0;
$error = "";

if (isset($_POST["submit_prestation"])) {
  $nom_prestataire = isset($_POST["nom_prestataire"]) ? $_POST["nom_prestataire"] : "";
  $email = isset($_POST["email"]) ? $_POST["email"] : "";
  $type_prestation = isset($_POST["type_prestation"]) ? $_POST["type_prestation"] : "";
  $destination = isset($_POST["destination"]) ? $_POST["destination"] : "";
  $description = isset($_POST["description"]) ? $_POST["description"] : "";
  $prix = isset($_POST["prix"]) ? (float) $_POST["prix"] : 0;

  if (empty($nom_prestataire)) {
    $error .= "Le nom du prestataire est requis.<br>";
  }

  if (empty($email)) {
    $error .= "L'email est requis.<br>";
  }

  if (!empty($email) && !filter_var($email, FILTER_VALIDATE_EMAIL)) {
    $error .= "L'email n'est pas valide.<br>";
  }

  if (empty($type_prestation)) {
    $error .= "Le type de prestation est requis.<br>";
  }

  if (empty($destination)) {
    $error .= "La destination est requise.<br>";
  }

  if (empty($description)) {
    $error .= "La description est requise.<br>";
  }

  if ($prix < 0) {
    $error .= "Le prix ne peut pas etre negatif.<br>";
  }

  if (!$conn) {
    $error .= "Connexion à la base de données impossible.<br>";
  }

  if ($error == "") {
    $statut = "a_verifier";
    $sql = "INSERT INTO demandes_prestataires (nom_prestataire, email, type_prestation, destination, description, prix, statut) VALUES (?, ?, ?, ?, ?, ?, ?)";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "sssssds", $nom_prestataire, $email, $type_prestation, $destination, $description, $prix, $statut);

    if (!mysqli_stmt_execute($stmt)) {
      $error .= "La proposition n'a pas pu être enregistrée.<br>";
    }
  }
} else {
  $error = "Aucune proposition n'a été envoyée.<br>";
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <title>VoyageVista - Proposition prestataire</title>
  <link rel="stylesheet" href="../style.css">
</head>
<body>
  <main>
    <section>
      <?php if ($error != "") { ?>
        <h1>Proposition refusee</h1>
        <p class="error"><?php echo $error; ?></p>
        <p><a class="button-link" href="../prestataire.php">Retour au formulaire</a></p>
      <?php } else { ?>
        <h1>Proposition envoyée</h1>
        <p class="success">La prestation a été enregistrée temporairement et pourra être vérifiée par l'administrateur.</p>
        <p><strong>Prestataire :</strong> <?php echo htmlspecialchars($nom_prestataire); ?></p>
        <p><strong>Type :</strong> <?php echo htmlspecialchars($type_prestation); ?></p>
        <p><strong>Destination :</strong> <?php echo htmlspecialchars($destination); ?></p>
        <p><a class="button-link" href="../prestataire.php">Ajouter une autre prestation</a></p>
      <?php } ?>
    </section>
  </main>
</body>
</html>
