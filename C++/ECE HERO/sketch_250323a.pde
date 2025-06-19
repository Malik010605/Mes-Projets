import processing.sound.*;
SoundFile musiqueMenu;

import processing.serial.*;
Serial port;

PImage logo;
PImage fondJeu;

// === ECE Hero – Jeu complet avec interface, pause, niveaux, musiques ===

// Variables de jeu
final int MENU = 0;
final int JEU = 1;
final int FIN = 2;
final int PARAMETRES = 3;
int etat = MENU;

// Musiques et niveaux
String[] musiques = {"Chopin (Nocturne)", "Clair de Lune", "Gymnopédie", "Super Mario 64"};
String[] niveaux = {"Niveau 1", "Niveau 2", "Niveau 3", "Niveau 4"};

int musiqueChoisie = -1;
int niveauChoisi = 0;
int intervalle = 40;
int scoreAffiche = 0;


int nbColonnes = 8;
float[] positionsX = new float[nbColonnes];
float largeurColonne;
String[] nomsNotes = {"DO", "RE", "MI", "FA", "SOL", "LA", "SI", "DO AIGU"};


ArrayList<Note> notes = new ArrayList<Note>();
int compteur = 0;

int score = 0;
int dureeMusique = 30;
int tempsDebut = 0;
int tempsEnPause = 0;
int debutPause = 0;
boolean enPause = false;

String[] themes = {"ECE", "Classique", "Sombre"};
int themeActuel = 0;
boolean dansParametres = false;

boolean survolBouton(float x, float y, float w, float h) {
  return mouseX > x - w/2 && mouseX < x + w/2 &&
    mouseY > y - h/2 && mouseY < y + h/2;
}

boolean enDecompte = false;
int tempsDecompte = 0;
String[] textesDecompte = {"À vos marques ?", "Prêts ?", "Pianottez !!!"};
int[] taillesTexteDecompte = {32, 42, 56};
int indexTexteDecompte = 0;
int tempsAffichageTexte = 1000; // 1 seconde par texte

boolean sonJoue = false;

int selectionMenu = 0;
int totalItemsMenu; // 4 = 3 niveaux + paramètres + quitter
int selectionParametres = 0;

ArrayList<EffetNote> effets = new ArrayList<EffetNote>();
ArrayList<EffetValidation> effetsValidations = new ArrayList<EffetValidation>();
ArrayList<NoteRatee> notesRatees = new ArrayList<NoteRatee>();



int[] flashColonnes = new int[nbColonnes]; // timestamps des derniers flashs
int dureeFlash = 200; // durée en millisecondes pendant laquelle la colonne s’allume
int selectionFin = 0;  // 0 = Rejouer, 1 = Retour menu



// === Partitions des musiques ===
int[] partitionChopin = {
  4, 3, 4, 5, 4, 3, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 2, 3,
  4, 3, 2, 1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
  // Ajout de répétitions/modulations simples pour compléter :
  0, 1, 2, 3, 2, 1, 0,
  1, 2, 3, 4, 3, 2, 1,
  2, 3, 4, 5, 4, 3, 2,
  3, 4, 5, 6, 5, 4, 3,
  4, 5, 6, 7, 6, 5, 4
};

boolean[] notesLonguesChopin = {
  false, false, false, true,  false, false, false, false,
  false, true,  false, false, false, false, false, false,
  false, true,  false, false, false, false, true,  false,
  false, false, true,  false, false, false, true,
  // Pour les nouvelles notes : pattern simple alterné
  false, false, true,  false, false, false, true,
  false, false, false, true,  false, false, false,
  true,  false, false, true,  false, false, false,
  true,  false, false, true,  false, false, false,
  true,  false, false, true,  false, false, false
};

int[] partitionDebussy = {
  4, 3, 2, 1, 2, 3, 4, 5, 4, 3, 2, 1,
  0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 4, 3,
  2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3,
  4, 5, 4, 3, 2, 3, 4, 5, 4, 3, 2
};

boolean[] notesLonguesDebussy = {
  false, false, true,  false, false, true,  false, false,
  true,  false, false, false, true,  false, false, true,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, false, false, false, true,  false,
  false, false, false, true,  false, false, true,  false,
  false, false, true,  false, false, false, false
};



int[] partitionSatie = {
  2, 3, 4, 5, 4, 3, 2, 1,
  2, 3, 4, 5, 6, 5, 4, 3,
  4, 3, 2, 1, 0, 1, 2, 3,
  2, 3, 4, 5, 4, 3, 2, 1,
  2, 3, 4, 5, 4, 3, 2, 1,
  0, 1, 2, 3, 2, 1, 0, 1,
  2, 3, 4, 5, 4, 3, 2, 1,
  2, 1, 0, 1
};


boolean[] notesLonguesSatie = {
  false, false, true,  false, false, true,  false, false,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, false, true,  false, false, true,
  false, false, true,  false, false, true,  false, false,
  false, false, true,  false, false, true,  false, false,
  false, false, false, true,  false, false, true,  false,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, true
};





int[] partitionMario = {
  4, 4, 4, 1, 3, 4, 5, 5,
  5, 5, 3, 4, 2, 2, 2, 1,
  2, 3, 4, 4, 4, 3, 4, 5,
  4, 3, 2, 1, 0, 1, 2, 3,
  2, 3, 4, 4, 4, 4, 3, 2,
  3, 4, 5, 5, 4, 3, 2, 1,
  0, 1, 2, 3, 2, 1, 0, 1,
  2, 3, 4, 5,
  2, 2, 4, 5, 4, 3, 1, 0,
  1, 2, 3, 4, 3, 2, 1, 0,
  1, 2, 3, 4, 5, 4, 3, 2,
  1, 2, 3, 4
};

boolean[] notesLonguesMario = {
  false, false, true,  false, false, true,  false, false,
  false, true,  false, false, true,  false, true,  false,
  false, true,  false, false, false, true,  false, true,
  false, false, false, true,  false, false, true,  false,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, false, true,  false, false, true,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, true,
  false, false, true,  false, false, true,  false, false,
  true,  false, false, true,  false, false, true,  false,
  false, true,  false, false, true,  false, false, true,
  false, false, true,  false
};






// Liste des partitions et longueurs associées
int[][] partitions = {
  partitionChopin,
  partitionDebussy,
  partitionSatie,
  partitionMario
};

boolean[][] longueurs = {
  notesLonguesChopin,
  notesLonguesDebussy,
  notesLonguesSatie,
  notesLonguesMario
};


// Variables de suivi de la partition en jeu
int[] partition = new int[0];
boolean[] notesLongues = new boolean[0];
int prochaineNote = 0;
int tempsDerniereNote = 0;
int delaiNote = 500;
// Délai entre les notes selon le niveau (en millisecondes)
int[] delaiNotesParNiveau = {500, 300, 180}; // Niveaux 1 à 3 pour Chopin
boolean flashActif = false;
int debutFlash = 0;
int dureeFlashLumiere = 150; // en millisecondes







void setup() {
  fullScreen();
  textAlign(CENTER, CENTER);
  textSize(20);

  printArray(Serial.list()); // pour trouver le bon port (ex: COM7)
  port = new Serial(this, "COM7", 9600);
  port.bufferUntil('\n');
  totalItemsMenu = 3 + musiques.length + 2;

  musiqueMenu = new SoundFile(this, "Son Menu.mp3");
  musiqueMenu.loop(); // démarre automatiquement
  scoreAffiche = 0;  // Remettre à zéro avant d'afficher

  logo = loadImage("logo ece.png");


  largeurColonne = width / nbColonnes;
  for (int i = 0; i < nbColonnes; i++) {
    positionsX[i] = i * largeurColonne + largeurColonne / 2;
  }
}

void draw() {
  // Appliquer le fond selon le thème
  if (themeActuel == 0) {
    background(173, 216, 230); // Thème ECE
  } else if (themeActuel == 1) {
    background(60, 0, 100);    // Thème Classique
  } else {
    background(20, 0, 0);      // Thème Sombre
  }

  // Musique de fond uniquement dans le menu principal
  if (etat == MENU && !musiqueMenu.isPlaying()) {
    musiqueMenu.loop();
  } else if (etat != MENU && musiqueMenu.isPlaying()) {
    musiqueMenu.stop();
  }

  // Affichage en fonction de l'état
if (etat == MENU) {
  if (dansParametres) {
    afficherParametres();  // ✅ afficher l’écran des paramètres si demandé
  } else {
    afficherMenu();        // ✅ sinon le menu classique
  }
} else if (etat == JEU) {
  jouer();
} else if (etat == FIN) {
  if (!confettisGeneres) {
    for (int i = 0; i < 100; i++) confettis.add(new Confetti());
    confettisGeneres = true;
  }
  afficherFin();
}
 else if (etat == JEU) {
    jouer();
  } else if (etat == FIN) {
    if (!confettisGeneres) {
      for (int i = 0; i < 100; i++) {
        confettis.add(new Confetti());
      }
      confettisGeneres = true;
    }
    afficherFin();
  }
}


void afficherMenu() {
  appliquerTheme();
  float anim = sin(frameCount * 0.05) * 10;
  textSize(48);
  text("ECE Hero", width / 2, 60 + anim);



  textSize(24);
  text("Choisis ton niveau :", width / 2, 140);
  float totalLargeur = 3 * 120 + 2 * 20;
  float departX = width / 2 - totalLargeur / 2;

  for (int i = 0; i < 3; i++) {
    float bx = departX + i * (180 + 20);
    float by = 190;
    boolean estSelectionne = (selectionMenu == i);
    boolean estActif = (niveauChoisi == i);

    float scale = estSelectionne ? 1.1 : 1.0;
    float w = 180 * scale;
    float h = 50 * scale;

    rectMode(CENTER);

    // Logique de couleur corrigée
    if (estSelectionne && estActif) {
      fill(255, 215, 0); // Jaune : sélectionné + actif
    } else if (estSelectionne) {
      fill(255, 240, 180); // Beige : juste survolé
    } else {
      fill(200, 230, 255); // Bleu clair par défaut
    }

    stroke(0);
    rect(bx, by, w, h, 12);

    fill(0);
    textSize(20 * scale);
    text(niveaux[i], bx, by);
  }
  textSize(20);

  fill(themeActuel == 0 ? 0 : 255); // texte noir pour ECE, blanc pour les autres
  text("Choisis une musique :", width / 2, 280);

  for (int i = 0; i < musiques.length; i++) {
    float bx = width / 2;
    float by = 340 + i * 70;
    boolean survole = (selectionMenu == i + 3);

    float scale = survole ? 1.1 : 1.0;
    float w = 300 * scale;
    float h = 50 * scale;

    rectMode(CENTER);
    fill(survole ? color(255, 240, 180) : color(173, 216, 230));
    stroke(0);
    rect(bx, by, w, h, 12);

    fill(0);
    textSize(20 * scale);
    text(musiques[i], bx, by);
  }
  textSize(20); // réinitialisation



  // Bouton Paramètres (en haut à gauche)
  float bxParam = 95;
  float byParam = 40;
  int indexParam = 3 + musiques.length;

  boolean survoleParam = (selectionMenu == indexParam);
  float scaleParam = survoleParam ? 1.1 : 1.0;

  rectMode(CENTER);
  fill(survoleParam ? color(255, 240, 180) : color(220));
  stroke(0);
  rect(bxParam, byParam, 150 * scaleParam, 40 * scaleParam, 10);

  fill(0);
  textSize(16 * scaleParam);
  textAlign(CENTER, CENTER);
  text("Paramètres", bxParam, byParam);
  textAlign(CENTER, CENTER); // remettre par défaut





  // Bouton Quitter (en bas à droite)
  float bxQuitter = width - 95;
  float byQuitter = height - 40;
  int indexQuitter = 4 + musiques.length;

  boolean survoleQuitter = (selectionMenu == indexQuitter);
  float scaleQuit = survoleQuitter ? 1.1 : 1.0;

  rectMode(CENTER);
  fill(survoleQuitter ? color(255, 240, 180) : color(220));
  stroke(0);
  rect(bxQuitter, byQuitter, 150 * scaleQuit, 40 * scaleQuit, 10);

  fill(0);
  textSize(16 * scaleQuit);
  text("Quitter", bxQuitter, byQuitter);





  // --- Crédits en bas à gauche ---
  textAlign(LEFT, BOTTOM);
  int creditsColor = (themeActuel == 0) ? color(0) : color(255);
  fill(creditsColor);
  text("Crédits :", 20, height - 60);


  // Soulignement de "Crédits :"
  float txtWidth = textWidth("Crédits :");
  stroke(0);
  strokeWeight(1);
  line(20, height - 58, 20 + txtWidth, height - 58);

  noStroke();
  fill(creditsColor);
  text("Malik SAADI", 20, height - 40);
  text("Edgar PRADET", 20, height - 25);
  text("Mael BOUDET-JEAN", 20, height - 10);
  textAlign(CENTER, CENTER); // réinitialisation alignement

  float w = 300;
  float h = 200;
  image(logo, width / 2 - w / 2, height - h - 20, w, h);
}

void jouer() {
background(0);  
// === Colonnes et flashs ===
  stroke(80);
  for (int i = 0; i < nbColonnes; i++) {
    float x = i * largeurColonne;
    int tempsDepuisFlash = millis() - flashColonnes[i];

    if (tempsDepuisFlash < dureeFlash) {
      fill(255, 255, 200, map(tempsDepuisFlash, 0, dureeFlash, 180, 0));
      noStroke();
      rect(x, 0, largeurColonne, height);
    }

    // noms des notes
    fill(255);
    textSize(16);
    text(nomsNotes[i], x + largeurColonne / 2, height - 20);
  }

  // === Affichage métronome visuel (tempo niveau) ===
  int tempo = 800;
  if (niveauChoisi == 1) tempo = 600;
  else if (niveauChoisi == 2) tempo = 400;

  if ((millis() - tempsDebut - tempsEnPause) % tempo < 100) {
    fill(255, 100, 100);
    ellipse(width - 30, 30, 20, 20);
  }

  // === Instructions boutons Arduino ===
  fill(180);
  textSize(14);
  textAlign(RIGHT, TOP);
  text("Bouton UP: Pause  |  Bouton DOWN: Quitter", width - 10, 10);
  textAlign(CENTER, CENTER);

  // === Pause ===
  if (enPause) {
    fill(0, 0, 0, 150);
    rect(0, 0, width, height);
    fill(255, 200, 0);
    textSize(50);
    text("PAUSE", width / 2, height / 2);
    textSize(16);
    fill(255);
    text("Appuyez sur le bouton UP pour reprendre", width / 2, height / 2 + 50);
    return;
  }

  // === Décompte initial ===
  if (enDecompte) {
    int elapsed = millis() - tempsDecompte;
    int index = elapsed / tempsAffichageTexte;

    if (index < textesDecompte.length) {
      float progress = (elapsed % tempsAffichageTexte) / float(tempsAffichageTexte);
      float tailleMin = 30;
      float tailleMax = taillesTexteDecompte[index];
      float tailleActuelle = lerp(tailleMin, tailleMax, progress);

      float opacity = (progress < 0.5) ? map(progress, 0, 0.5, 0, 255)
                                      : map(progress, 0.5, 1.0, 255, 0);

      fill(255, opacity);
      textAlign(CENTER, CENTER);
      textSize(tailleActuelle);
      text(textesDecompte[index], width / 2, height / 2);
    } else {
      enDecompte = false;
      tempsDebut = millis();
    }
    return;
  }

  // === Ajout des notes depuis la partition ===
if (partition.length > 0 && prochaineNote < partition.length && millis() - tempsDerniereNote > delaiNote) {
  int col = partition[prochaineNote];
  boolean estLongue = notesLongues[prochaineNote];
  notes.add(new Note(col, niveauChoisi, estLongue));
  prochaineNote++;
  tempsDerniereNote = millis();
}


  // === Mise à jour et affichage des notes ===
for (int i = notes.size() - 1; i >= 0; i--) {
  Note n = notes.get(i);
  n.update();
  n.afficher();
  if (n.estHorsEcran()) {
    // ➕ Ajoute effet de disparition stylée
    notesRatees.add(new NoteRatee(
      n.colonne * largeurColonne + largeurColonne / 2,
      n.y, n.taille, n.c));
    notes.remove(i);
  }
}


  // === Affichage temps restant ===
  int tempsEcoule = (millis() - tempsDebut - tempsEnPause) / 1000;
  int tempsRestant = max(0, dureeMusique - tempsEcoule);

  fill(255);
  textSize(20);
  textAlign(CENTER, TOP);
  text("Temps restant : " + tempsRestant + "s", width / 2, 10);

  if (tempsRestant <= 0) {
    etat = FIN;
  }

  // === Affichage nom musique ===
  if (musiqueChoisie >= 0 && musiqueChoisie < musiques.length) {
    fill(200);
    textSize(20);
    text("Musique : " + musiques[musiqueChoisie], width / 2, 40);
  }

  // === Effets visuels (bonnes notes) ===
  for (int i = effets.size() - 1; i >= 0; i--) {
    EffetNote e = effets.get(i);
    e.afficher();
    if (e.estTermine()) {
      effets.remove(i);
    }
  }
  
  for (int i = effetsValidations.size() - 1; i >= 0; i--) {
  EffetValidation e = effetsValidations.get(i);
  e.afficher();
  if (e.estTermine()) {
    effetsValidations.remove(i);
  }
}

for (int i = notesRatees.size() - 1; i >= 0; i--) {
  NoteRatee r = notesRatees.get(i);
  r.afficher();
  if (r.estTerminee()) {
    notesRatees.remove(i);
  }
}
  textAlign(CENTER, CENTER); // reset alignement
}




void afficherFin() {
  // Fond animé avec pulsation légère
  background(20 + sin(frameCount * 0.05) * 10);

  // Confettis
  for (int i = confettis.size() - 1; i >= 0; i--) {
    Confetti c = confettis.get(i);
    c.update();
    c.afficher();
    if (c.estHorsEcran()) {
      confettis.remove(i);
      confettis.add(new Confetti());
    }
  }

  textAlign(CENTER, CENTER);

  // === Titre animé ===
  float scaleTitre = map(sin(frameCount * 0.05), -1, 1, 1.1, 1.25);
  textSize(48 * scaleTitre);
  fill(255, 215, 0);
  text("Partie terminée !", width / 2, height / 3 - 30);

  // === Animation score ===
  if (scoreAffiche < score) {
    scoreAffiche += 1;
  }

  textSize(24);
  fill(255);
  text("Tu as marqué :", width / 2, height / 3 + 30);

  textSize(42);
  fill(0, 255, 0);
  text(scoreAffiche + " points", width / 2, height / 3 + 80);

  // === Sélection boutons ===
  boolean rejouerActif = (selectionFin == 0);
  boolean menuActif   = (selectionFin == 1);

  // === Bouton Rejouer ===
  float scaleRejouer = rejouerActif ? 1.1 : 1.0;
  rectMode(CENTER);
  fill(rejouerActif ? color(255, 240, 180) : color(100, 200, 255));
  rect(width / 2, height / 2 + 130, 200 * scaleRejouer, 50 * scaleRejouer, 12);
  fill(0);
  textSize(20 * scaleRejouer);
  text("Rejouer", width / 2, height / 2 + 130);

  // === Bouton Retour Menu ===
  float scaleMenu = menuActif ? 1.1 : 1.0;
  fill(menuActif ? color(255, 240, 180) : color(200));
  rect(width / 2, height / 2 + 200, 200 * scaleMenu, 50 * scaleMenu, 12);
  fill(0);
  textSize(20 * scaleMenu);
  text("Retour au menu", width / 2, height / 2 + 200);
}



void mousePressed() {
  // --- MENU PRINCIPAL ---
  float totalLargeur = 4 * 180 + 3 * 20;
  float departX = width / 2 - totalLargeur / 2 + 95;

  // Sélection du niveau (0 à 2)
  for (int i = 0; i < niveaux.length; i++) {
    float bx = departX + i * (180 + 20);
    float by = 190;
    if (mouseX > bx - 90 && mouseX < bx + 90 &&
        mouseY > by - 25 && mouseY < by + 25) {
      niveauChoisi = i;

      // ✅ ENVOI DU NIVEAU AU MÉTRONOME ARDUINO
      port.write(str(niveauChoisi + 1));

      return;
    }
  }

  // Sélection d'une musique
  for (int i = 0; i < musiques.length; i++) {
    float bx = width / 2;
    float by = 340 + i * 70;
    if (mouseX > bx - 150 && mouseX < bx + 150 &&
        mouseY > by - 25 && mouseY < by + 25) {
      musiqueChoisie = i;

      // ✅ Nouvelle version : initialise tout avec la fonction dédiée
      lancerMusiqueSelectionnee();
      return;
    }
  }

// Bouton Paramètres (haut gauche)
if (mouseX > 20 && mouseX < 170 &&
    mouseY > 20 && mouseY < 60) {
  etat = PARAMETRES;
  selectionParametres = 0;
  return;
}


  // Bouton Quitter (bas droite)
  if (mouseX > width - 170 && mouseX < width - 20 &&
      mouseY > height - 60 && mouseY < height - 20) {
    exit();
  }

  // --- MENU PARAMÈTRES ---
  if (etat == MENU && dansParametres) {
    for (int i = 0; i < themes.length; i++) {
      float by = 200 + i * 70;
      if (mouseX > width / 2 - 125 && mouseX < width / 2 + 125 &&
          mouseY > by - 25 && mouseY < by + 25) {
        themeActuel = i;
        return;
      }
    }

    // Bouton retour
    if (mouseX > width / 2 - 100 && mouseX < width / 2 + 100 &&
        mouseY > height - 80 && mouseY < height - 40) {
  etat = MENU;
      return;
    }
  }

  // --- ÉCRAN DE FIN ---
  if (etat == FIN) {
    float bx = width / 2;

    // Rejouer
    float byRejouer = height / 2 + 60;
    if (mouseX > bx - 100 && mouseX < bx + 100 &&
        mouseY > byRejouer - 25 && mouseY < byRejouer + 25) {
      lancerMusiqueSelectionnee(); // redémarre avec partition correcte
      return;
    }

    // Retour au menu
    float byMenu = height / 2 + 130;
    if (mouseX > bx - 100 && mouseX < bx + 100 &&
        mouseY > byMenu - 25 && mouseY < byMenu + 25) {
      etat = MENU;
      musiqueChoisie = -1;
      notes.clear();
      compteur = 0;
      score = 0;
      dansParametres = false;
      confettis.clear();
      confettisGeneres = false;
      return;
    }
  }
}





class Note {
  int colonne;
  float y;
  float taille; 
  float vitesse;
  color c;
  boolean estLongue;
  float longueur; // longueur visuelle d'une note longue

  Note(int colonne, int niveau, boolean estLongue) {
    this.colonne = colonne;
    this.y = -60;
    this.estLongue = estLongue;

    if (niveau == 0) vitesse = 5;
    else if (niveau == 1) vitesse = 8;
    else vitesse = 12;

    this.c = color(random(100, 255), random(100, 255), random(100, 255));
    this.longueur = 120; // ajustable
  }

  void update() {
    y += vitesse;
  }

  void afficher() {
    float x = positionsX[colonne];

    // === TRAÎNÉE ===
    for (int i = 0; i < 10; i++) {
      float offsetY = i * -10;
      float alpha = map(i, 0, 10, 60, 0);
      fill(red(c), green(c), blue(c), alpha);
      noStroke();
      ellipse(x, y + offsetY, 40, 40);
    }

    // === NOTE LONGUE ===
    if (estLongue) {
      // Barre verticale
      fill(c, 180);
      noStroke();
      float yStart = y;
      float yEnd = y + longueur;

      // Si la queue est encore visible, on affiche la barre
      if (yEnd < height + 60) {
        rectMode(CORNER);
        rect(x - 6, yStart, 12, longueur);
      }

      // Queue de la note
      if (yEnd < height + 60) {
        fill(c);
        stroke(0);
        strokeWeight(2);
        ellipse(x, y + longueur, 40, 40); // bas

        fill(0);
        noStroke();
        ellipse(x, y + longueur, 18, 18);
      }
    }

    // === TÊTE DE NOTE ===
    fill(c);
    stroke(0);
    strokeWeight(2);
    ellipse(x, y, 40, 40); // anneau

    fill(0);
    noStroke();
    ellipse(x, y, 18, 18); // centre noir
  }

  boolean estHorsEcran() {
    return y > height + longueur + 60;
  }
}

class EffetValidation {
  float x, y;
  int debut;
  int duree = 500;
  color c;

  EffetValidation(float x, float y) {
    this.x = x;
    this.y = y;
    this.debut = millis();
    this.c = color(random(200, 255), random(100, 255), random(100, 255));
  }

  void afficher() {
    float t = millis() - debut;
    if (t > duree) return;

    float progress = map(t, 0, duree, 0, 1);
    float alpha = map(t, 0, duree, 255, 0);
    float rayon = 30 + 80 * progress;
    float epaisseur = 5 + 10 * sin(progress * PI);

    noFill();
    strokeWeight(epaisseur);
    stroke(c, alpha);
    ellipse(x, y, rayon, rayon);
  }

  boolean estTermine() {
    return millis() - debut > duree;
  }
}

class NoteRatee {
  float x, y;
  float taille;
  color c;
  int debut;
  int duree = 300;

  NoteRatee(float x, float y, float taille, color c) {
    this.x = x;
    this.y = y;
    this.taille = taille;
    this.c = c;
    this.debut = millis();
  }

  void afficher() {
    float t = millis() - debut;
    float progress = constrain(t / float(duree), 0, 1);
    float rayon = taille + progress * 30;
    float alpha = 255 * (1 - progress);

    noFill();
    stroke(c, alpha);
    strokeWeight(2);
    ellipse(x, y, rayon, rayon);
  }

  boolean estTerminee() {
    return millis() - debut > duree;
  }
}


// === Classe Confetti ===
class Confetti {
  float x, y;
  float vitesseY, vitesseX;
  color c;
  float taille;
  float angle, rotationSpeed;

  Confetti() {
    x = random(width);
    y = random(-100, -10);
    vitesseY = random(2, 5);
    vitesseX = random(-1.5, 1.5);
    c = color(random(255), random(255), random(255));
    taille = random(5, 12);
    angle = random(TWO_PI);
    rotationSpeed = random(-0.1, 0.1);
  }

  void update() {
    x += vitesseX;
    y += vitesseY;
    angle += rotationSpeed;
  }

  void afficher() {
    pushMatrix();
    translate(x, y);
    rotate(angle);
    fill(c);
    noStroke();
    rectMode(CENTER);
    rect(0, 0, taille, taille);
    popMatrix();
  }

  boolean estHorsEcran() {
    return y > height + 20;
  }
}

ArrayList<Confetti> confettis = new ArrayList<Confetti>();
boolean confettisGeneres = false;

color getCouleurFond() {
  return themeActuel == 0 ? color(230, 240, 255)
    : themeActuel == 1 ? color(255)
    : color(30);
}

color getCouleurTexte() {
  return themeActuel == 2 ? color(255) : color(0);
}


void afficherParametres() {
  background(getCouleurFond());

  textAlign(CENTER, CENTER);
  textSize(30);
  fill(getCouleurTexte());
  text("Choisis un thème :", width / 2, 100);

  for (int i = 0; i < themes.length; i++) {
    float by = 200 + i * 70;
    float bx = width / 2;
    boolean survole = (selectionParametres == i);

    float scale = survole ? 1.1 : 1.0;
    float w = 250 * scale;
    float h = 50 * scale;

    rectMode(CENTER);
    fill(themeActuel == i ? color(255, 215, 0) : (survole ? color(230) : color(200)));
    stroke(0);
    rect(bx, by, w, h, 12);

    fill(0);
    textSize(20 * scale);
    text(themes[i], bx, by);
  }

  float bxRetour = width / 2;
  float byRetour = 200 + themes.length * 70;

  boolean survoleRetour = (selectionParametres == themes.length); // ✅ définir d'abord
  float scaleRetour = survoleRetour ? 1.1 : 1.0;                    // ✅ ensuite le scale

  rectMode(CENTER);
  fill(survoleRetour ? color(255, 240, 180) : color(220));
  stroke(0);
  rect(bxRetour, byRetour, 200 * scaleRetour, 40 * scaleRetour, 12);

  fill(0);
  textSize(20 * scaleRetour);
  text("Retour", bxRetour, byRetour);
}


void appliquerTheme() {
  if (themeActuel == 0) {
    background(120, 180, 220); // Thème ECE (bleu un peu plus foncé)
    fill(0);                   // Texte noir
  } else if (themeActuel == 1) {
    background(60, 0, 100);    // Thème Classique
    fill(255);                 // Texte blanc
  } else {
    background(20, 0, 0);      // Thème Sombre
    fill(255);                 // Texte blanc
  }
}

void lancerMusiqueSelectionnee() {
  etat = JEU;
  notes.clear();
  compteur = 0;
  score = 0;
  tempsEnPause = 0;
  enPause = false;
  enDecompte = true;
  indexTexteDecompte = 0;
  tempsDecompte = millis();
  confettis.clear();
  confettisGeneres = false;

  // Initialisation de la partition et des longueurs
  partition = partitions[musiqueChoisie];
  notesLongues = longueurs[musiqueChoisie];
  prochaineNote = 0;
  tempsDerniereNote = millis();

  // Définir le bon délai selon la musique choisie
  if (musiqueChoisie == 0) { // Chopin (Nocturne)
    delaiNote = delaiNotesParNiveau[niveauChoisi];
  } else if (musiqueChoisie == 1) { // Debussy
    delaiNote = 500;
  } else if (musiqueChoisie == 2) { // Satie
    delaiNote = 600;
  } else if (musiqueChoisie == 3) { // Mario
    delaiNote = 350;
  } else {
    delaiNote = 500; // Sécurité
  }

  println("Musique choisie : " + musiqueChoisie + " → " + musiques[musiqueChoisie]);
}





int tempsPauseDebut = 0;

void serialEvent(Serial port) {
  String data = port.readStringUntil('\n');
  if (data != null) {
    data = data.trim();
    println("Reçu : " + data);

    if (etat == JEU && !enPause && !enDecompte) {
      int indexNote = getIndexNote(data);
      if (indexNote != -1) {
        gererAppuiNote(indexNote);
      }
    }
    
    int maxSelection = 4 + musiques.length; // 3 niveaux + musiques + paramètres + quitter
    
        // === Menu de fin ===
if (etat == FIN) {
  if (data.equals("up")) {
    selectionFin = max(0, selectionFin - 1);
  } else if (data.equals("down")) {
    selectionFin = min(1, selectionFin + 1);
  } else if (data.equals("select")) {
    if (selectionFin == 0) {
      // Rejouer
      etat = JEU;
      notes.clear();
      compteur = 0;
      score = 0;
      tempsEnPause = 0;
      enPause = false;
      tempsDebut = millis();
      confettis.clear();
      confettisGeneres = false;
      prochaineNote = 0;
      tempsDerniereNote = millis();
    } else {
      // Retour au menu
      etat = MENU;
      musiqueChoisie = -1;
      notes.clear();
      compteur = 0;
      score = 0;
      dansParametres = false;
      confettis.clear();
      confettisGeneres = false;
    }
  }
}



    // Navigation dans menu
if (etat == MENU) {
  if (dansParametres) {
    // Navigation dans les paramètres
    if (data.equals("up")) selectionParametres = max(0, selectionParametres - 1);
    else if (data.equals("down")) selectionParametres = min(themes.length, selectionParametres + 1);
    else if (data.equals("select")) {
      if (selectionParametres < themes.length) {
        themeActuel = selectionParametres;
        println("Thème sélectionné : " + themes[selectionParametres]);
      } else {
        dansParametres = false;
        println("Retour au menu principal");
      }
    }
  } else {
    // Navigation dans le menu principal
    if (data.equals("up")) selectionMenu = max(0, selectionMenu - 1);
    else if (data.equals("down")) selectionMenu = min(maxSelection, selectionMenu + 1);
    else if (data.equals("select")) lancerSelection(selectionMenu);
  }
}


    if (etat == JEU && data.equals("up")) { // pause
      enPause = !enPause;
      if (enPause) debutPause = millis();
      else tempsEnPause += (millis() - debutPause);
    }

if (etat == JEU && data.equals("down")) { // quitter
  etat = MENU;
  musiqueChoisie = -1;
  notes.clear();
  partition = new int[0];
  notesLongues = new boolean[0];
  prochaineNote = 0;
  compteur = 0;
  score = 0;
  enPause = false;
  enDecompte = false;
  tempsEnPause = 0;
  tempsDerniereNote = 0;
  println("Retour au menu.");
}

  }
}



int getIndexNote(String note) {
  note = note.trim().toUpperCase().replace("É", "E");

  if (note.equals("DO")) return 0;
  if (note.equals("RE")) return 1;
  if (note.equals("MI")) return 2;
  if (note.equals("FA")) return 3;
  if (note.equals("SOL")) return 4;
  if (note.equals("LA")) return 5;
  if (note.equals("SI")) return 6;
  if (note.equals("DO AIGU")) return 7;

  return -1;
}





String derniereNote = "";

void gererAppuiNote(int indexNote) {
  if (indexNote < 0 || indexNote >= nbColonnes) return;  // sécurité

  derniereNote = nomsNotes[indexNote]; // Pour affichage
  println("Note jouée : " + derniereNote);

  boolean noteTrouvee = false;

  for (int i = notes.size() - 1; i >= 0; i--) {
    Note n = notes.get(i);

    // Debug pour visualiser les notes à l'écran
    println("→ Note à l'écran : " + nomsNotes[n.colonne] + " à y = " + n.y);

    // Condition de validation plus tolérante
    if (n.colonne == indexNote && abs(n.y - (height - 40)) < 150) {
      notes.remove(i);
      score += 2;
      flashColonnes[indexNote] = millis();
      float x = n.colonne * largeurColonne + largeurColonne / 2;
      float y = height - 40;
      effetsValidations.add(new EffetValidation(x, y));
      println("Note validée !");
      flashActif = true;
      debutFlash = millis();
      noteTrouvee = true;
      break;
    }
  }

  if (!noteTrouvee) {
    println("⚠️ Aucune note validée.");
  }
}




class EffetNote {
  int colonne;
  float x, y;
  float rayon = 0;
  int duree = 300;
  int debut;

  EffetNote(int col) {
    this.colonne = col;
    this.x = colonne * largeurColonne + largeurColonne / 2;
    this.y = height - 40;
    this.debut = millis();
  }

  void afficher() {
    float t = millis() - debut;
    if (t < duree) {
      float alpha = map(t, 0, duree, 255, 0);
      float r = map(t, 0, duree, 10, 60);
      noStroke();
      fill(255, 255, 0, alpha);
      ellipse(x, y, r, r);
    }
  }

  boolean estTermine() {
    return millis() - debut > duree;
  }
}

void lancerSelection(int index) {
  if (dansParametres) {
    themeActuel = index;
    println("Thème changé : " + index);
    return;
  }

  if (index >= 0 && index <= 2) {
    // ✅ Niveau sélectionné
    niveauChoisi = index;
    port.write(str(niveauChoisi + 1));  // envoyer à l'Arduino
    println("Niveau choisi : " + niveauChoisi);
    return;
  }

if (index >= 0 && index < 3) {
  // Choix du niveau
  niveauChoisi = index;
  port.write(str(niveauChoisi + 1));
  println("Niveau choisi : " + niveauChoisi);
} 
else if (index >= 3 && index < 3 + musiques.length) {
  // Choix de la musique
  musiqueChoisie = index - 3;
  lancerMusiqueSelectionnee();
} 
else if (index == 3 + musiques.length) {
  // Paramètres
dansParametres = true;
selectionParametres = 0;
  println("Ouverture du menu paramètres");
} 
else if (index == 4 + musiques.length) {
  // Quitter
  println("Fermeture du jeu.");
  exit();
}
}


void chargerMusique(int index) {
  musiqueChoisie = index;
  etat = JEU;
  notes.clear();
  compteur = 0;
  score = 0;
  enPause = false;
  enDecompte = true;
  confettis.clear();
  confettisGeneres = false;
  tempsEnPause = 0;
  tempsDecompte = millis();
  indexTexteDecompte = 0;
}
