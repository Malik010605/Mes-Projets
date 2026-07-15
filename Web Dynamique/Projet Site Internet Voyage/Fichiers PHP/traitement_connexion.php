<?php
session_start();
require_once "config.php";

$email = "";
$mot_de_passe = "";
$error = "";

if (isset($_POST["submit_connexion"])) {
  $email = isset($_POST["email"]) ? $_POST["email"] : "";
  $mot_de_passe = isset($_POST["mot_de_passe"]) ? $_POST["mot_de_passe"] : "";

  if (empty($email)) {
    $error .= "L'email est requis.<br>";
  }

  if (!empty($email) && !filter_var($email, FILTER_VALIDATE_EMAIL)) {
    $error .= "L'email n'est pas valide.<br>";
  }

  if (empty($mot_de_passe)) {
    $error .= "Le mot de passe est requis.<br>";
  }

  if (!$conn) {
    $error .= "Connexion à la base de données impossible.<br>";
  }

  if ($error == "") {
    $sql = "SELECT id, prenom, nom, email, mot_de_passe, role FROM utilisateurs WHERE email = ?";
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "s", $email);
    mysqli_stmt_execute($stmt);
    $resultat = mysqli_stmt_get_result($stmt);
    $utilisateur_trouve = mysqli_fetch_assoc($resultat);

    if ($utilisateur_trouve && password_verify($mot_de_passe, $utilisateur_trouve["mot_de_passe"])) {
      $_SESSION["utilisateur"] = array(
        "id" => $utilisateur_trouve["id"],
        "prenom" => $utilisateur_trouve["prenom"],
        "nom" => $utilisateur_trouve["nom"],
        "email" => $utilisateur_trouve["email"],
        "role" => $utilisateur_trouve["role"]
      );

      header("Location: ../compte.php");
      exit();
    } else {
      $error = "Email ou mot de passe incorrect.<br>";
    }
  }
} else {
  $error = "Aucun formulaire de connexion n'a été envoyé.<br>";
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <title>VoyageVista - Connexion</title>
  <link rel="stylesheet" href="../style.css">
</head>
<body>
  <main>
    <section>
      <h1>Connexion impossible</h1>
      <p class="error"><?php echo $error; ?></p>
      <p><a class="button-link" href="../connexion.php">Retour a la connexion</a></p>
    </section>
  </main>
</body>
</html>
