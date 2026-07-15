<?php
session_start();
require_once "Fichiers PHP/config.php";

$utilisateur = isset($_SESSION["utilisateur"]) ? $_SESSION["utilisateur"] : null;
$notifications = array();
$message_compte = "";

if ($conn && $utilisateur && isset($_POST["modifier_profil"])) {
  $prenom = isset($_POST["prenom"]) ? trim($_POST["prenom"]) : "";
  $nom = isset($_POST["nom"]) ? trim($_POST["nom"]) : "";
  $email = isset($_POST["email"]) ? trim($_POST["email"]) : "";

  if ($prenom == "" || $nom == "" || !filter_var($email, FILTER_VALIDATE_EMAIL)) {
    $message_compte = "Les informations du profil sont incomplètes.";
  } else {
    $sql = "UPDATE utilisateurs SET prenom = ?, nom = ?, email = ? WHERE id = ?";
    $stmt = mysqli_prepare($conn, $sql);
    $utilisateur_id = (int) $utilisateur["id"];
    mysqli_stmt_bind_param($stmt, "sssi", $prenom, $nom, $email, $utilisateur_id);

    if (mysqli_stmt_execute($stmt)) {
      $_SESSION["utilisateur"]["prenom"] = $prenom;
      $_SESSION["utilisateur"]["nom"] = $nom;
      $_SESSION["utilisateur"]["email"] = $email;
      $utilisateur = $_SESSION["utilisateur"];
      $message_compte = "Le profil a été modifié.";
    } else {
      $message_compte = "Le profil n'a pas pu être modifié.";
    }
  }
}

if ($conn && $utilisateur && isset($_POST["marquer_notifications_lues"])) {
  $utilisateur_id = (int) $utilisateur["id"];
  $sql = "UPDATE notifications SET lu = 1 WHERE utilisateur_id = ?";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "i", $utilisateur_id);
  mysqli_stmt_execute($stmt);
  $message_compte = "Les notifications ont été marquées comme lues.";
}

if ($conn && $utilisateur && isset($utilisateur["id"])) {
  $utilisateur_id = (int) $utilisateur["id"];
  $sql = "SELECT message, lu, date_notification FROM notifications WHERE utilisateur_id = ? ORDER BY date_notification DESC";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "i", $utilisateur_id);
  mysqli_stmt_execute($stmt);
  $resultat = mysqli_stmt_get_result($stmt);

  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $notifications[] = $ligne;
    }
  }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Mon compte</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Espace utilisateur</p>
      </div>
    </div>

    <nav>
      <ul>
        <li><a href="index.html">Accueil</a></li>
        <li><a href="destinations.php">Destinations</a></li>
        <li><a href="transports.php">Transports</a></li>
        <li><a href="hebergements.php">Hébergements</a></li>
        <li><a href="activites.php">Activités</a></li>
        <li><a href="sejour.php">Mon séjour</a></li>
        <li><a href="panier.php">Panier</a></li>
        <li><a href="connexion.php">Connexion</a></li>
      </ul>
    </nav>
  </header>

  <main>
    <section>
      <h2>Mon compte</h2>

      <?php if ($message_compte != "") { ?>
        <p class="info-box"><?php echo htmlspecialchars($message_compte); ?></p>
      <?php } ?>

      <?php if ($utilisateur) { ?>
        <p>Bienvenue, <?php echo htmlspecialchars($utilisateur["prenom"]); ?>.</p>
        <p><strong>Email :</strong> <?php echo htmlspecialchars($utilisateur["email"]); ?></p>
        <p><strong>Rôle :</strong> <?php echo htmlspecialchars($utilisateur["role"]); ?></p>

        <h3>Modifier mon profil</h3>
        <form method="post" action="compte.php">
          <label for="prenom">Prénom :</label>
          <input type="text" id="prenom" name="prenom" value="<?php echo htmlspecialchars($utilisateur["prenom"]); ?>" required>

          <label for="nom">Nom :</label>
          <input type="text" id="nom" name="nom" value="<?php echo htmlspecialchars($utilisateur["nom"]); ?>" required>

          <label for="email">Email :</label>
          <input type="email" id="email" name="email" value="<?php echo htmlspecialchars($utilisateur["email"]); ?>" required>

          <button type="submit" name="modifier_profil">Enregistrer le profil</button>
        </form>

        <h3>Notifications</h3>
        <?php if (count($notifications) > 0) { ?>
          <ul>
            <?php foreach ($notifications as $notification) { ?>
              <li>
                <?php echo htmlspecialchars($notification["message"]); ?> -
                <?php echo htmlspecialchars($notification["date_notification"]); ?> -
                <?php echo $notification["lu"] ? "lue" : "non lue"; ?>
              </li>
            <?php } ?>
          </ul>

          <form method="post" action="compte.php">
            <button type="submit" name="marquer_notifications_lues">Marquer comme lues</button>
          </form>
        <?php } else { ?>
          <p>Aucune notification pour le moment.</p>
        <?php } ?>

        <p><a class="button-link" href="Fichiers PHP/traitement_deconnexion.php">Déconnexion</a></p>
      <?php } else { ?>
        <p class="info-box">Vous devez être connecté pour accéder à votre compte.</p>
        <p><a class="button-link" href="connexion.php">Se connecter</a></p>
      <?php } ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
