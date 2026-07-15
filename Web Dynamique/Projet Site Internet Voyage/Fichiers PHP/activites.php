<?php
require_once "Fichiers PHP/config.php";

$activites = array();

if ($conn) {
  $sql = "SELECT a.nom, a.description, a.duree, a.prix, a.places, a.date_activite, d.nom AS destination
          FROM activites a
          INNER JOIN destinations d ON a.destination_id = d.id
          ORDER BY d.nom, a.nom";
  $resultat = mysqli_query($conn, $sql);

  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $activites[] = $ligne;
    }
  }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Activités</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Activités et expériences</p>
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
      <h2>Activités proposees</h2>
      <img class="page-image" src="assets/images/activite-voyage.png" alt="Activités touristiques pour compléter un séjour">
      <div class="card-list">
        <?php foreach ($activites as $activite) { ?>
          <article>
            <span class="tag"><?php echo htmlspecialchars($activite["destination"]); ?></span>
            <h3><?php echo htmlspecialchars($activite["nom"]); ?></h3>
            <p><?php echo htmlspecialchars($activite["description"]); ?></p>
            <p><strong>Durée :</strong> <?php echo htmlspecialchars($activite["duree"]); ?></p>
            <p><strong>Date :</strong> <?php echo htmlspecialchars($activite["date_activite"]); ?></p>
            <p><strong>Places :</strong> <?php echo htmlspecialchars($activite["places"]); ?></p>
            <p><strong>Prix :</strong> <?php echo htmlspecialchars($activite["prix"]); ?> euros</p>
            <p><a class="button-link" href="sejour.php">Ajouter au séjour</a></p>
          </article>
        <?php } ?>
      </div>

      <?php if (count($activites) == 0) { ?>
        <p class="info-box">Aucune activité n'est disponible pour le moment.</p>
      <?php } ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
