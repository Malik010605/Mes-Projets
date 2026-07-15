<?php
require_once "Fichiers PHP/config.php";

$hebergements = array();

if ($conn) {
  $sql = "SELECT h.nom, h.lieu, h.description, h.prix_nuit, h.capacite, h.disponible, d.nom AS destination
          FROM hebergements h
          INNER JOIN destinations d ON h.destination_id = d.id
          ORDER BY d.nom, h.nom";
  $resultat = mysqli_query($conn, $sql);

  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $hebergements[] = $ligne;
    }
  }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Hébergements</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Hébergements et disponibilites</p>
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
      <h2>Hébergements disponibles</h2>
      <img class="page-image" src="assets/images/hebergement-voyage.png" alt="Hébergement confortable pour un séjour">
      <p>Les hébergements affichés viennent de la base de données.</p>

      <div class="card-list">
        <?php foreach ($hebergements as $hebergement) { ?>
          <article>
            <h3><?php echo htmlspecialchars($hebergement["nom"]); ?></h3>
            <p><strong>Destination :</strong> <?php echo htmlspecialchars($hebergement["destination"]); ?></p>
            <p><strong>Lieu :</strong> <?php echo htmlspecialchars($hebergement["lieu"]); ?></p>
            <p><?php echo htmlspecialchars($hebergement["description"]); ?></p>
            <p><strong>Capacité :</strong> <?php echo htmlspecialchars($hebergement["capacite"]); ?> personne(s)</p>
            <p><strong>Prix :</strong> <?php echo htmlspecialchars($hebergement["prix_nuit"]); ?> euros / nuit</p>
            <p><strong>Disponible :</strong> <?php if ($hebergement["disponible"]) { echo "Oui"; } else { echo "Non"; } ?></p>
            <p><a class="button-link" href="sejour.php">Sélectionner</a></p>
          </article>
        <?php } ?>
      </div>

      <?php if (count($hebergements) == 0) { ?>
        <p class="info-box">Aucun hébergement n'est disponible pour le moment.</p>
      <?php } ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
