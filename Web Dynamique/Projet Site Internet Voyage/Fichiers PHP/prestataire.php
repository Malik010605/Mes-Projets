<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VoyageVista - Prestataire</title>
  <link rel="stylesheet" href="style.css">
</head>

<body>
  <header>
    <div class="header-title">
      <img class="site-logo" src="assets/images/logo-voyagevista.png" alt="Logo VoyageVista">
      <div>
        <h1>VoyageVista</h1>
        <p>Espace prestataire</p>
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
      <h2>Proposer une prestation</h2>
      <img class="page-image" src="assets/images/prestataire-voyage.png" alt="Prestataire qui propose une offre de voyage">
      <p>Les prestataires peuvent proposer une destination, un hébergement ou une activité. La validation sera faite plus tard par l'administrateur.</p>

      <form method="post" action="Fichiers PHP/traitement_prestataire.php">
        <label for="nom_prestataire">Nom du prestataire :</label>
        <input type="text" id="nom_prestataire" name="nom_prestataire" required>

        <label for="email">Email :</label>
        <input type="email" id="email" name="email" required>

        <label for="type_prestation">Type de prestation :</label>
        <select id="type_prestation" name="type_prestation" required>
          <option value="">Choisir un type</option>
          <option value="destination">Destination</option>
          <option value="hebergement">Hébergement</option>
          <option value="activite">Activité</option>
        </select>

        <label for="destination">Destination associee :</label>
        <input type="text" id="destination" name="destination" required>

        <label for="description">Description :</label>
        <textarea id="description" name="description" required></textarea>

        <label for="prix">Prix indicatif :</label>
        <input type="number" id="prix" name="prix" min="0" required>

        <button type="submit" name="submit_prestation">Envoyer la proposition</button>
      </form>
    </section>
  </main>

  <footer>
    <small>Copyright &copy; 2026, VoyageVista - Projet Web dynamique - Malik SAADI, Edgar PRADET et Antoine DAGNEAUX</small>
  </footer>

  <script src="script.js"></script>
</body>
</html>
