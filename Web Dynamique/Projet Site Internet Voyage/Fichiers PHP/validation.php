<?php
session_start();

$reservation = isset($_SESSION["reservation"]) ? $_SESSION["reservation"] : null;
$utilisateurConnecte = isset($_SESSION["utilisateur"]);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Validation</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Validation de réservation</p>
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
      <h2>Confirmation</h2>

      <?php if ($reservation) { ?>
        <p class="success">Votre réservation a bien été prise en compte.</p>
        <p>Un récapitulatif sera conservé dans votre compte utilisateur.</p>
      <?php } else { ?>
        <p class="info-box">Aucune réservation complète n'est disponible pour le moment.</p>
        <p>Cette page servira a afficher la confirmation apres le traitement PHP.</p>
      <?php } ?>

      <p>
        <a class="button-link" href="index.html">Retour a l'accueil</a>
        <?php if ($utilisateurConnecte) { ?>
          <a class="second-link" href="compte.php">Voir mon compte</a>
        <?php } else { ?>
          <a class="second-link" href="connexion.php">Se connecter</a>
        <?php } ?>
      </p>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
