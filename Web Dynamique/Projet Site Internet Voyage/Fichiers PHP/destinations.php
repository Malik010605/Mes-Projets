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

$recherche = isset($_GET["recherche"]) ? trim($_GET["recherche"]) : "";
$categorie = isset($_GET["categorie"]) ? trim($_GET["categorie"]) : "";
$tri = isset($_GET["tri"]) ? trim($_GET["tri"]) : "nom";
$ordreSql = "nom ASC";

if ($tri == "prix_asc") {
  $ordreSql = "prix_base ASC";
} elseif ($tri == "prix_desc") {
  $ordreSql = "prix_base DESC";
}

$destinations = array();
$categories = array("Culture", "Gastronomie", "Mer");

if ($conn) {
  $resultatCategories = mysqli_query($conn, "SELECT DISTINCT categorie FROM destinations WHERE statut = 'validee' ORDER BY categorie");

  if ($resultatCategories) {
    $categories = array();

    while ($ligne = mysqli_fetch_assoc($resultatCategories)) {
      $categories[] = $ligne["categorie"];
    }
  }

  if ($recherche != "" && $categorie != "") {
    $sql = "SELECT id, nom, categorie, description, prix_base, image
            FROM destinations
            WHERE statut = 'validee'
            AND (nom LIKE ? OR description LIKE ?)
            AND categorie = ?
            ORDER BY " . $ordreSql;
    $stmt = mysqli_prepare($conn, $sql);
    $rechercheLike = "%" . $recherche . "%";
    mysqli_stmt_bind_param($stmt, "sss", $rechercheLike, $rechercheLike, $categorie);
    mysqli_stmt_execute($stmt);
    $resultat = mysqli_stmt_get_result($stmt);
  } elseif ($recherche != "") {
    $sql = "SELECT id, nom, categorie, description, prix_base, image
            FROM destinations
            WHERE statut = 'validee'
            AND (nom LIKE ? OR description LIKE ?)
            ORDER BY " . $ordreSql;
    $stmt = mysqli_prepare($conn, $sql);
    $rechercheLike = "%" . $recherche . "%";
    mysqli_stmt_bind_param($stmt, "ss", $rechercheLike, $rechercheLike);
    mysqli_stmt_execute($stmt);
    $resultat = mysqli_stmt_get_result($stmt);
  } elseif ($categorie != "") {
    $sql = "SELECT id, nom, categorie, description, prix_base, image
            FROM destinations
            WHERE statut = 'validee'
            AND categorie = ?
            ORDER BY " . $ordreSql;
    $stmt = mysqli_prepare($conn, $sql);
    mysqli_stmt_bind_param($stmt, "s", $categorie);
    mysqli_stmt_execute($stmt);
    $resultat = mysqli_stmt_get_result($stmt);
  } else {
    $sql = "SELECT id, nom, categorie, description, prix_base, image
            FROM destinations
            WHERE statut = 'validee'
            ORDER BY " . $ordreSql;
    $resultat = mysqli_query($conn, $sql);
  }

  if (isset($resultat) && $resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $destinations[] = $ligne;
    }
  }
}

if (count($destinations) == 0 && $recherche == "" && $categorie == "") {
  $destinations = array(
    array("id" => 1, "nom" => "Paris", "categorie" => "Culture", "description" => "Séjour culturel autour des musées, monuments et quartiers historiques.", "prix_base" => "320.00", "image" => "destination-paris.png"),
    array("id" => 2, "nom" => "Nice", "categorie" => "Mer", "description" => "Séjour détente entre bord de mer, vieille ville et activités en extérieur.", "prix_base" => "410.00", "image" => "destination-nice.png"),
    array("id" => 3, "nom" => "Lyon", "categorie" => "Gastronomie", "description" => "Week-end découverte autour de la ville, des visites et des restaurants.", "prix_base" => "280.00", "image" => "destination-lyon.png")
  );
}

$champRecherche = '<input type="text" id="recherche" name="recherche" placeholder="Exemple : Paris" value="' . e($recherche) . '">';

$selectCategories = '<select id="categorie" name="categorie">';
$selectCategories .= '<option value="">Toutes les catégories</option>';

foreach ($categories as $cat) {
  $selected = "";

  if ($categorie == $cat) {
    $selected = " selected";
  }

  $selectCategories .= '<option value="' . e($cat) . '"' . $selected . '>' . e($cat) . '</option>';
}

$selectCategories .= '</select>';

$selectTri = '<select id="tri" name="tri">';
$optionsTri = array(
  "nom" => "Nom",
  "prix_asc" => "Prix croissant",
  "prix_desc" => "Prix décroissant"
);

foreach ($optionsTri as $valeur => $texte) {
  $selected = "";

  if ($tri == $valeur) {
    $selected = " selected";
  }

  $selectTri .= '<option value="' . e($valeur) . '"' . $selected . '>' . e($texte) . '</option>';
}

$selectTri .= '</select>';

$catalogueHtml = "";

foreach ($destinations as $destination) {
  $id = isset($destination["id"]) ? $destination["id"] : 0;
  $nom = isset($destination["nom"]) ? $destination["nom"] : "";
  $cat = isset($destination["categorie"]) ? $destination["categorie"] : "";
  $description = isset($destination["description"]) ? $destination["description"] : "";
  $prix = isset($destination["prix_base"]) ? $destination["prix_base"] : "0.00";
  $image = isset($destination["image"]) ? cheminImageDestination($destination["image"]) : "assets/images/destination-paris.png";

  $catalogueHtml .= '<article>';
  $catalogueHtml .= '<img class="card-image" src="' . e($image) . '" alt="Vue de ' . e($nom) . '">';
  $catalogueHtml .= '<span class="tag">' . e($cat) . '</span>';
  $catalogueHtml .= '<h3>' . e($nom) . '</h3>';
  $catalogueHtml .= '<p>' . e($description) . '</p>';
  $catalogueHtml .= '<p><strong>Prix indicatif :</strong> ' . e($prix) . ' euros</p>';
  $catalogueHtml .= '<p><a class="button-link" href="destination.php?id=' . e($id) . '">Voir le détail</a></p>';
  $catalogueHtml .= '</article>';
}

$messageRecherche = "";

if (count($destinations) == 0) {
  $messageRecherche = '<p class="info-box">Aucune destination ne correspond à la recherche.</p>';
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Destinations</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Catalogue des destinations</p>
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
      <h2>Destinations disponibles</h2>
      <p>Cette page présente les destinations enregistrées dans la base de données.</p>

      <form method="get" action="destinations.php">
        <label for="recherche">Rechercher une destination :</label>
        <?php echo $champRecherche; ?>

        <label for="categorie">Catégorie :</label>
        <?php echo $selectCategories; ?>

        <label for="tri">Trier par :</label>
        <?php echo $selectTri; ?>

        <button type="submit">Rechercher</button>
      </form>
    </section>

    <section>
      <h2>Catalogue</h2>

      <div class="card-list">
        <?php echo $catalogueHtml; ?>
      </div>

      <?php echo $messageRecherche; ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
