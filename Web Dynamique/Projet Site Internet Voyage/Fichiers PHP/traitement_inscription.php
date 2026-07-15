<?php
session_start();
require_once "config.php";

$prenom = "";
$nom = "";
$email = "";
$role = "";
$mot_de_passe = "";
$confirmation = "";
$error = "";

if (isset($_POST["submit_inscription"])) {
  $prenom = isset($_POST["prenom"]) ? $_POST["prenom"] : "";
  $nom = isset($_POST["nom"]) ? $_POST["nom"] : "";
  $email = isset($_POST["email"]) ? $_POST["email"] : "";
  $role = isset($_POST["role"]) ? $_POST["role"] : "";
  $mot_de_passe = isset($_POST["mot_de_passe"]) ? $_POST["mot_de_passe"] : "";
  $confirmation = isset($_POST["confirmation"]) ? $_POST["confirmation"] : "";

  if (empty($prenom)) {
    $error .= "Le prénom est requis.<br>";
  }

  if (empty($nom)) {
    $error .= "Le nom est requis.<br>";
  }

  if (empty($email)) {
    $error .= "L'email est requis.<br>";
  }

  if (!empty($email) && !filter_var($email, FILTER_VALIDATE_EMAIL)) {
    $error .= "L'email n'est pas valide.<br>";
  }

  if (empty($role)) {
    $error .= "Le rôle est requis.<br>";
  }

  if ($role != "voyageur" && $role != "prestataire") {
    $error .= "Le rôle choisi n'est pas valide.<br>";
  }

  if (empty($mot_de_passe)) {
    $error .= "Le mot de passe est requis.<br>";
  }

  if ($mot_de_passe != $confirmation) {
    $error .= "Les mots de passe ne sont pas identiques.<br>";
  }

  if (!$conn) {
    $error .= "Connexion à la base de données impossible.<br>";
  }

  if ($error == "") {
    $sql = "SELECT id FROM utilisateurs WHERE email = ?";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "s", $email);
    mysqli_stmt_execute($stmt);
    $resultat = mysqli_stmt_get_result($stmt);

    if (mysqli_fetch_assoc($resultat)) {
      $error .= "Un compte existe déjà avec cet email.<br>";
    }
  }

  if ($error == "") {
    $mot_de_passe_hash = password_hash($mot_de_passe, PASSWORD_DEFAULT);
    $sql = "INSERT INTO utilisateurs (prenom, nom, email, mot_de_passe, role) VALUES (?, ?, ?, ?, ?)";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "sssss", $prenom, $nom, $email, $mot_de_passe_hash, $role);

    if (mysqli_stmt_execute($stmt)) {
      header("Location: ../connexion.php?inscription=ok");
      exit();
    } else {
      $error .= "L'inscription n'a pas pu être enregistrée.<br>";
    }
  }
} else {
  $error = "Aucun formulaire d'inscription n'a été envoyé.<br>";
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <title>VoyageVista - Inscription</title>
  <link rel="stylesheet" href="../style.css">
</head>
<body>
  <main>
    <section>
      <h1>Inscription impossible</h1>
      <p class="error"><?php echo $error; ?></p>
      <p><a class="button-link" href="../inscription.php">Retour a l'inscription</a></p>
    </section>
  </main>
</body>
</html>
