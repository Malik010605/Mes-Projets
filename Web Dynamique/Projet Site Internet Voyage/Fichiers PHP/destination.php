<?php
header("Content-Type: text/html; charset=UTF-8");
require_once "Fichiers PHP/config.php";

function e($texte) {
  return htmlspecialchars($texte, ENT_QUOTES, "UTF-8");
}

function cheminImageDestination($image) {
  $nomImage = basename($image);

  if ($nomImage == "") {
    $nomImage = "destination-paris.png";
  }

  return "assets/images/" . $nomImage;
}

$id = isset($_GET["id"]) ? (int) $_GET["id"] : 0;
$destination = null;

if ($conn && $id > 0) {
  $sql = "SELECT id, nom, categorie, description, prix_base, image
          FROM destinations
          WHERE id = ? AND statut = 'validee'";
  $stmt = mysqli_prepare($conn, $sql);
  mysqli_stmt_bind_param($stmt, "i", $id);
  mysqli_stmt_execute($stmt);
  $resultat = mysqli_stmt_get_result($stmt);
  $destination = mysqli_fetch_assoc($resultat);
}

$contenuDestination = "";

if ($destination) {
  $nom = $destination["nom"];
  $categorie = $destination["categorie"];
  $description = $destination["description"];
  $prix = $destination["prix_base"];
  $image = cheminImageDestination($destination["image"]);

  $contenuDestination .= '<span class="tag">' . e($categorie) . '</span>';
  $contenuDestination .= '<h2>' . e($nom) . '</h2>';
  $contenuDestination .= '<img class="detail-image" src="' . e($image) . '" alt="Vue de ' . e($nom) . '">';
  $contenuDestination .= '<p>' . e($description) . '</p>';
  $contenuDestination .= '<table>';
  $contenuDestination .= '<tr><th>Information</th><th>Valeur</th></tr>';
  $contenuDestination .= '<tr><td>Catégorie</td><td>' . e($categorie) . '</td></tr>';
  $contenuDestination .= '<tr><td>Prix indicatif</td><td>' . e($prix) . ' euros</td></tr>';
  $contenuDestination .= '<tr><td>Remarque</td><td>Les transports, hébergements et activités se choisissent dans la page Mon séjour.</td></tr>';
  $contenuDestination .= '</table>';
  $contenuDestination .= '<p>';
  $contenuDestination .= '<a class="button-link" href="sejour.php?destination=' . e($id) . '">Composer un séjour</a> ';
  $contenuDestination .= '<a class="second-link" href="destinations.php">Retour aux destinations</a>';
  $contenuDestination .= '</p>';
} else {
  $contenuDestination .= '<h2>Destination introuvable</h2>';
  $contenuDestination .= '<p class="error">Aucune destination valide n\'a été trouvée.</p>';
  $contenuDestination .= '<p><a class="button-link" href="destinations.php">Retour aux destinations</a></p>';
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Détail destination</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Détail d'une destination</p>
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
      <?php echo $contenuDestination; ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
