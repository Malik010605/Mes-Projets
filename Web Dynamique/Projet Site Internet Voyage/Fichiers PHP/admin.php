<?php
session_start();
require_once "Fichiers PHP/config.php";

$utilisateurs = array();
$reservations = array();
$prestations = array();
$destinations = array();
$message_admin = isset($_SESSION["message_admin"]) ? $_SESSION["message_admin"] : "";
unset($_SESSION["message_admin"]);

function afficherStatut($statut) {
  if ($statut == "validee") {
    return "validée";
  }

  if ($statut == "a_verifier") {
    return "à vérifier";
  }

  if ($statut == "annulee") {
    return "annulée";
  }

  if ($statut == "supprimee") {
    return "supprimée";
  }

  return str_replace("_", " ", $statut);
}

if ($conn) {
  $resultat = mysqli_query($conn, "SELECT id, nom, prenom, email, role FROM utilisateurs ORDER BY nom");
  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $utilisateurs[] = $ligne;
    }
  }

  $resultat = mysqli_query($conn, "SELECT id, nom, categorie, description, prix_base, image, statut FROM destinations ORDER BY nom");
  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $destinations[] = $ligne;
    }
  }

  $sql = "SELECT r.id, r.statut, r.total, u.nom AS client, d.nom AS destination
          FROM reservations r
          LEFT JOIN utilisateurs u ON r.utilisateur_id = u.id
          LEFT JOIN reservation_details rd ON rd.reservation_id = r.id
          LEFT JOIN destinations d ON rd.destination_id = d.id
          ORDER BY r.date_reservation DESC";
  $resultat = mysqli_query($conn, $sql);
  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $reservations[] = $ligne;
    }
  }

  $sql = "SELECT id, type_prestation, nom_prestataire, destination, statut FROM demandes_prestataires ORDER BY date_demande DESC";
  $resultat = mysqli_query($conn, $sql);
  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $prestations[] = $ligne;
    }
  }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Administration</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Administration simple</p>
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
    <?php if (!empty($message_admin)) { ?>
      <section class="info-box">
        <p><?php echo htmlspecialchars($message_admin); ?></p>
      </section>
    <?php } ?>

    <section>
      <h2>Ajouter une destination</h2>
      <p>Cette partie permet d'ajouter rapidement une nouvelle offre visible dans le catalogue.</p>

      <form method="post" action="Fichiers PHP/traitement_admin.php">
        <label for="nom_destination">Nom :</label>
        <input type="text" id="nom_destination" name="nom" required>

        <label for="categorie_destination">Catégorie :</label>
        <input type="text" id="categorie_destination" name="categorie" placeholder="Culture, mer, gastronomie..." required>

        <label for="description_destination">Description :</label>
        <textarea id="description_destination" name="description" required></textarea>

        <label for="prix_destination">Prix indicatif :</label>
        <input type="number" id="prix_destination" name="prix_base" min="0" step="0.01" required>

        <label for="image_destination">Nom du fichier image :</label>
        <input type="text" id="image_destination" name="image" value="destination-paris.png" required>

        <button type="submit" name="ajouter_destination">Ajouter la destination</button>
      </form>
    </section>

    <section>
      <h2>Gestion des destinations</h2>
      <div class="card-list">
        <?php foreach ($destinations as $destination) { ?>
          <article>
            <form method="post" action="Fichiers PHP/traitement_admin.php">
              <input type="hidden" name="destination_id" value="<?php echo htmlspecialchars($destination["id"]); ?>">

              <label>Nom :</label>
              <input type="text" name="nom" value="<?php echo htmlspecialchars($destination["nom"]); ?>" required>

              <label>Catégorie :</label>
              <input type="text" name="categorie" value="<?php echo htmlspecialchars($destination["categorie"]); ?>" required>

              <label>Description :</label>
              <textarea name="description" required><?php echo htmlspecialchars($destination["description"]); ?></textarea>

              <label>Prix :</label>
              <input type="number" name="prix_base" min="0" step="0.01" value="<?php echo htmlspecialchars($destination["prix_base"]); ?>" required>

              <label>Image :</label>
              <input type="text" name="image" value="<?php echo htmlspecialchars($destination["image"]); ?>" required>

              <label>Statut :</label>
              <select name="statut">
                <option value="validee" <?php if ($destination["statut"] == "validee") { echo "selected"; } ?>>Validée</option>
                <option value="a_verifier" <?php if ($destination["statut"] == "a_verifier") { echo "selected"; } ?>>À vérifier</option>
                <option value="supprimee" <?php if ($destination["statut"] == "supprimee") { echo "selected"; } ?>>Supprimée</option>
              </select>

              <p><strong>État actuel :</strong> <?php echo htmlspecialchars(afficherStatut($destination["statut"])); ?></p>

              <button type="submit" name="modifier_destination">Modifier</button>
              <button type="submit" name="supprimer_destination" data-confirm="Supprimer cette destination du catalogue ?">Supprimer</button>
            </form>
          </article>
        <?php } ?>
      </div>
    </section>

    <section>
      <h2>Utilisateurs</h2>
      <table>
        <tr>
          <th>Nom</th>
          <th>Email</th>
          <th>Rôle</th>
          <th>Action</th>
        </tr>
        <?php foreach ($utilisateurs as $utilisateur) { ?>
          <tr>
            <td><?php echo htmlspecialchars($utilisateur["prenom"] . " " . $utilisateur["nom"]); ?></td>
            <td><?php echo htmlspecialchars($utilisateur["email"]); ?></td>
            <td><?php echo htmlspecialchars($utilisateur["role"]); ?></td>
            <td>
              <form method="post" action="Fichiers PHP/traitement_admin.php">
                <input type="hidden" name="utilisateur_id" value="<?php echo htmlspecialchars($utilisateur["id"]); ?>">
                <select name="role">
                  <option value="voyageur" <?php if ($utilisateur["role"] == "voyageur") { echo "selected"; } ?>>Voyageur</option>
                  <option value="prestataire" <?php if ($utilisateur["role"] == "prestataire") { echo "selected"; } ?>>Prestataire</option>
                  <option value="admin" <?php if ($utilisateur["role"] == "admin") { echo "selected"; } ?>>Admin</option>
                </select>
                <button type="submit" name="modifier_role">Changer le rôle</button>
              </form>
            </td>
          </tr>
        <?php } ?>
      </table>
    </section>

    <section>
      <h2>Réservations</h2>
      <table>
        <tr>
          <th>Client</th>
          <th>Destination</th>
          <th>Total</th>
          <th>Statut</th>
          <th>Action</th>
        </tr>
        <?php foreach ($reservations as $reservation) { ?>
          <tr>
            <td><?php echo htmlspecialchars($reservation["client"] ? $reservation["client"] : "Invité"); ?></td>
            <td><?php echo htmlspecialchars($reservation["destination"]); ?></td>
            <td><?php echo htmlspecialchars($reservation["total"]); ?> euros</td>
            <td><?php echo htmlspecialchars(afficherStatut($reservation["statut"])); ?></td>
            <td>
              <?php if ($reservation["statut"] != "annulee") { ?>
                <form method="post" action="Fichiers PHP/traitement_admin.php">
                  <input type="hidden" name="reservation_id" value="<?php echo htmlspecialchars($reservation["id"]); ?>">
                  <button type="submit" name="annuler_reservation" data-confirm="Annuler cette réservation ?">Annuler</button>
                </form>
              <?php } else { ?>
                <span>Déjà annulée</span>
              <?php } ?>
            </td>
          </tr>
        <?php } ?>
      </table>
    </section>

    <section>
      <h2>Prestations proposées</h2>
      <table>
        <tr>
          <th>Type</th>
          <th>Nom</th>
          <th>Statut</th>
          <th>Action</th>
        </tr>
        <?php foreach ($prestations as $prestation) { ?>
          <tr>
            <td><?php echo htmlspecialchars($prestation["type_prestation"]); ?></td>
            <td><?php echo htmlspecialchars($prestation["nom_prestataire"] . " - " . $prestation["destination"]); ?></td>
            <td><?php echo htmlspecialchars(afficherStatut($prestation["statut"])); ?></td>
            <td>
              <?php if ($prestation["statut"] != "validee") { ?>
                <form method="post" action="Fichiers PHP/traitement_admin.php">
                  <input type="hidden" name="prestation_id" value="<?php echo htmlspecialchars($prestation["id"]); ?>">
                  <button type="submit" name="verifier">Vérifier</button>
                </form>
              <?php } else { ?>
                <span>Validée</span>
              <?php } ?>
            </td>
          </tr>
        <?php } ?>
      </table>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
