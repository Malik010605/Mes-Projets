<?php
header("Content-Type: text/html; charset=UTF-8");
require_once "Fichiers PHP/config.php";

function e($texte) {
  return htmlspecialchars($texte, ENT_QUOTES, "UTF-8");
}

$transports = array();
$destinations = array();
$depart = isset($_GET["depart"]) ? trim($_GET["depart"]) : "";
$destinationChoisie = isset($_GET["destination"]) ? trim($_GET["destination"]) : "";
$date_depart = isset($_GET["date_depart"]) ? trim($_GET["date_depart"]) : "";

if ($conn) {
  $resultatDestinations = mysqli_query($conn, "SELECT nom FROM destinations WHERE statut = 'validee' ORDER BY nom");

  if ($resultatDestinations) {
    while ($ligne = mysqli_fetch_assoc($resultatDestinations)) {
      $destinations[] = $ligne["nom"];
    }
  }

  $sql = "SELECT t.type_transport, t.ville_depart, t.ville_arrivee, t.duree, t.prix, t.places, t.date_depart, d.nom AS destination
          FROM transports t
          INNER JOIN destinations d ON t.destination_id = d.id
          WHERE (? = '' OR t.ville_depart LIKE ?)
          AND (? = '' OR d.nom = ?)
          AND (? = '' OR t.date_depart = ?)
          ORDER BY t.date_depart, d.nom";
  $stmt = mysqli_prepare($conn, $sql);
  $departLike = "%" . $depart . "%";
  mysqli_stmt_bind_param($stmt, "ssssss", $depart, $departLike, $destinationChoisie, $destinationChoisie, $date_depart, $date_depart);
  mysqli_stmt_execute($stmt);
  $resultat = mysqli_stmt_get_result($stmt);

  if ($resultat) {
    while ($ligne = mysqli_fetch_assoc($resultat)) {
      $transports[] = $ligne;
    }
  }
}

if (count($destinations) == 0) {
  $destinations = array("Paris", "Nice", "Lyon");
}

$champDepart = '<input type="text" id="depart" name="depart" placeholder="Exemple : Toulouse" value="' . e($depart) . '">';
$champDate = '<input type="date" id="date_depart" name="date_depart" value="' . e($date_depart) . '">';

$selectDestinations = '<select id="destination" name="destination">';
$selectDestinations .= '<option value="">Choisir une destination</option>';

foreach ($destinations as $destination) {
  $selected = "";

  if ($destinationChoisie == $destination) {
    $selected = " selected";
  }

  $selectDestinations .= '<option value="' . e($destination) . '"' . $selected . '>' . e($destination) . '</option>';
}

$selectDestinations .= '</select>';

$lignesTransports = "";

foreach ($transports as $transport) {
  $lignesTransports .= '<tr>';
  $lignesTransports .= '<td>' . e($transport["type_transport"]) . '</td>';
  $lignesTransports .= '<td>' . e($transport["destination"]) . '</td>';
  $lignesTransports .= '<td>' . e($transport["duree"]) . '</td>';
  $lignesTransports .= '<td>' . e($transport["date_depart"]) . '</td>';
  $lignesTransports .= '<td>' . e($transport["prix"]) . ' euros</td>';
  $lignesTransports .= '<td>' . e($transport["places"]) . '</td>';
  $lignesTransports .= '<td><a href="sejour.php">Sélectionner</a></td>';
  $lignesTransports .= '</tr>';
}

$messageTransport = "";

if (count($transports) == 0) {
  $messageTransport = '<p class="info-box">Aucun transport ne correspond à la recherche.</p>';
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Transports</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Transport et planification des trajets</p>
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
      <h2>Rechercher un transport</h2>
      <img class="page-image" src="assets/images/transport-voyage.png" alt="Moyens de transport pour planifier un voyage">
      <form method="get" action="transports.php">
        <label for="depart">Ville de départ :</label>
        <?php echo $champDepart; ?>

        <label for="destination">Destination :</label>
        <?php echo $selectDestinations; ?>

        <label for="date_depart">Date de départ :</label>
        <?php echo $champDate; ?>

        <button type="submit">Rechercher</button>
      </form>
    </section>

    <section>
      <h2>Transports disponibles</h2>
      <table>
        <tr>
          <th>Type</th>
          <th>Destination</th>
          <th>Durée</th>
          <th>Date</th>
          <th>Prix</th>
          <th>Places</th>
          <th>Action</th>
        </tr>
        <?php echo $lignesTransports; ?>
      </table>

      <?php echo $messageTransport; ?>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
