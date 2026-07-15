<?php
session_start();

$panier = isset($_SESSION["panier"]) ? $_SESSION["panier"] : array();
$total = 0;
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Panier</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Panier de voyage</p>
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
      <h2>Récapitulatif</h2>

      <?php if (count($panier) > 0) { ?>
        <p><strong>Articles :</strong> <span id="cartCount">0</span></p>

        <table>
          <tr>
            <th>Element</th>
            <th>Choix</th>
            <th>Prix</th>
            <th>Action</th>
          </tr>
          <?php foreach ($panier as $index => $item) { ?>
            <?php $total = $total + $item["prix"]; ?>
            <tr class="panier-item">
              <td><?php echo htmlspecialchars($item["type"]); ?></td>
              <td><?php echo htmlspecialchars($item["nom"]); ?></td>
              <td><?php echo htmlspecialchars($item["prix"]); ?> euros</td>
              <td>
                <form method="post" action="Fichiers PHP/traitement_panier.php">
                  <input type="hidden" name="index_panier" value="<?php echo htmlspecialchars($index); ?>">
                  <button type="submit" name="retirer_panier">Retirer</button>
                </form>
              </td>
            </tr>
          <?php } ?>
        </table>

        <p><strong>Total estimé :</strong> <?php echo $total; ?> euros</p>

        <form method="post" action="Fichiers PHP/traitement_panier.php">
          <button type="submit" name="vider_panier" data-confirm="Vider le panier ?">Vider le panier</button>
        </form>

        <form method="post" action="Fichiers PHP/traitement_reservation.php" id="reservationForm">
          <h3>Paiement simulé</h3>

          <label for="nom_carte">Nom du titulaire :</label>
          <input type="text" id="nom_carte" name="nom_carte" required>

          <label for="numero_carte">Numéro de carte :</label>
          <input type="text" id="numero_carte" name="numero_carte" minlength="12" maxlength="19" required>

          <label for="expiration">Expiration :</label>
          <input type="month" id="expiration" name="expiration" required>

          <button type="submit" name="valider_reservation">Valider la réservation</button>
        </form>
      <?php } else { ?>
        <p class="info-box">Votre panier est vide pour le moment.</p>
        <p><a class="button-link" href="sejour.php">Composer un séjour</a></p>
      <?php } ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
