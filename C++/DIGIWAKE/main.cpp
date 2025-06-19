#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <LedControl.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// *** Définir les paramètres de la matrice LED ***
#define brocheDeSelection               10  // CS de la matrice LED
#define brochePourLesDonnees            11  // MOSI de la matrice LED
#define brochePourLhorloge              13  // CLK de la matrice LED
#define nombreDeMatricesLedRaccordees   4   // Nombre de matrices connectées
LedControl matriceLed = LedControl(brochePourLesDonnees, brochePourLhorloge, brocheDeSelection, nombreDeMatricesLedRaccordees);

// *** Définir les paramètres de l'écran OLED ***
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// *** Définir le RTC ***
RTC_DS1307 rtc;

// *** Définir les variables globales ***
bool colonState = true;              // État des deux-points
int menuIndex = 0;                   // L'index du menu
const int potentiometerPin = A0;     // Potentiomètre connecté à A0
const int numMenuItems = 6;          // Nombre d'éléments dans le menu

// Variables de format de l'heure
bool format24h = true; // Indiquer si le format est 24h (true) ou 12h (false)

// *** Définir la broche du bouton ***
const int boutonPin = 4;             // Broche connectée au bouton
bool boutonEtatPrecedent = HIGH;     // État précédent du bouton
bool boutonSelectionne = false;      // Indique si un élément a été sélectionné

bool reglageHeureActif = false;     // Indique si on est en mode réglage de l'heure
bool reglageMinutesActif = false;   // Indique si on est en mode réglage des minutes
int nouvelleHeure = 0;              // Stocke la nouvelle heure à définir
int nouvellesMinutes = 0;           // Stocke les nouvelles minutes à définir
int dernierPotentiometerValue = 0;  // Dernière valeur lue du potentiomètre (pour filtrage)

// Ajoutez une variable globale pour suivre l'état du menu
bool modeMenu = true; // `true` si on navigue dans le menu, `false` si on exécute un élément
int itemSelectionne = -1; // Indique l'élément du menu sélectionné (-1 : aucun)

bool reveilActive = true; // `true` pour réveil activé, `false` pour réveil désactivé


// Définir la broche du buzzer
#define BUZZER_PIN 3

// Broches pour le capteur HC-SR04
#define TRIG_PIN 9
#define ECHO_PIN 8

// Temps de snooze en minutes
int SNOOZE_DURATION = 1; // Durée par défaut en minutes

// Mélodies disponibles
const int melody1[][2] = { // Mélodie 1
  {262, 500}, {294, 500}, {330, 500}, {262, 500}
};

const int melody2[][2] = { // Mélodie 2
  {330, 500}, {349, 500}, {392, 500}, {330, 500}
};

const int melody3[][2] = { // Mélodie 3
  {392, 500}, {440, 500}, {494, 500}, {392, 500}
};

// Tailles des mélodies
const int melody1Length = sizeof(melody1) / sizeof(melody1[0]);
const int melody2Length = sizeof(melody2) / sizeof(melody2[0]);
const int melody3Length = sizeof(melody3) / sizeof(melody3[0]);

// Variable pour stocker la mélodie sélectionnée
int selectedMelody = 1;

bool enReglageSnooze = false; // Variable globale pour indiquer si on règle le snooze


// *** Caractères pour la matrice LED ***
byte chiffres[10][8] = {
  {B00111100, B01100110, B01100110, B01100110, B01100110, B01100110, B00111100, B00000000}, // 0
  {B00011000, B00111000, B00011000, B00011000, B00011000, B00011000, B01111110, B00000000}, // 1
  {B00111100, B01100110, B00000110, B00001100, B00011000, B01100000, B01111110, B00000000}, // 2
  {B00111100, B01100110, B00000110, B00011100, B00000110, B01100110, B00111100, B00000000}, // 3
  {B00001100, B00011100, B00101100, B01001100, B01111110, B00001100, B00001100, B00000000}, // 4
  {B01111110, B01100000, B01111100, B00000110, B00000110, B01100110, B00111100, B00000000}, // 5
  {B00111100, B01100110, B01100000, B01111100, B01100110, B01100110, B00111100, B00000000}, // 6
  {B01111110, B01100110, B00000110, B00001100, B00011000, B00011000, B00011000, B00000000}, // 7
  {B00111100, B01100110, B01100110, B00111100, B01100110, B01100110, B00111100, B00000000}, // 8
  {B00111100, B01100110, B01100110, B00111110, B00000110, B01100110, B00111100, B00000000}  // 9
};
byte deuxPoints[8] = {
  B00000000, B00000000, B00000001, B00000001, B00000000, B00000001, B00000001, B00000000
};

// *** Prototypes des fonctions ***
void afficherMenu();
void afficherHeureEtMinutes(int, int);
void afficherCaractere(byte caractere[8], int adresse);
void verifierBouton();
void changerFormatHeure();
void afficherReglageHeure(int valeur, bool estHeure);
void reglerHeure();
void gererHeure();
void reglerHorloge();  // Add this line
void gererMenu();
void buzzer();
void verifierReveil();
void reglerReveil();
long mesurerDistance();
void reglerSnooze();
void onOffReveil();
void choisirMelodie();

// *** Initialisation ***
void setup() {
  Serial.begin(9600);

  // Initialisation de l'écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Impossible d'initialiser l'écran OLED"));
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Erreur OLED"));
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println(F("Vérifiez les connexions."));
    display.display();
    while (1);
  }

  // Initialisation des matrices LED
  for (int i = 0; i < nombreDeMatricesLedRaccordees; i++) {
    matriceLed.shutdown(i, false);
    matriceLed.setIntensity(i, 5);
    matriceLed.clearDisplay(i);
  }

  // Initialisation du RTC
  if (!rtc.begin()) {
    Serial.println("RTC non détecté !");
    while (1);
  }
  if (!rtc.isrunning()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Réglage de l'heure avec la date et l'heure de compilation
  }

  // Configuration de la broche du bouton
  pinMode(boutonPin, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  afficherMenu(); // Afficher le menu initial
}

void loop() {
  // Récupération de l'heure actuelle
  DateTime now = rtc.now();
  int heures = now.hour();
  int minutes = now.minute();

  // Gérer l'heure en fonction du format
  if (!format24h && heures == 0) {
    heures = 12; // 00h -> 12h en format 12h
  } else if (!format24h && heures > 12) {
    heures -= 12; // Convertir l'heure en format 12h
  }



  // Afficher les heures et minutes
  afficherHeureEtMinutes(heures, minutes);

  // Lire la valeur du potentiomètre pour naviguer dans le menu
  int potValue = analogRead(potentiometerPin);
  // Map the potentiometer value to menu index, ensuring it stays within bounds
  menuIndex = constrain(map(potValue, 0, 1023, 0, numMenuItems), 0, numMenuItems);

  verifierBouton();

  // Afficher le menu (met à jour l'affichage de l'élément sélectionné)
  afficherMenu();

  // Gérer la sélection du menu en fonction du bouton
  gererMenu();

  verifierReveil();

  colonState = !colonState; // Clignoter les deux-points
  delay(1000); // Délai court pour le clignotement et la gestion du bouton
}






// *** Fonction pour afficher le réglage de l'heure ou des minutes ***
void afficherReglageHeure(int valeur, bool estHeure) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (estHeure) {
    // Afficher le réglage de l'heure
    display.print("Heure: ");
    if (valeur < 10) {
      display.print("0");
    }
    display.print(valeur);
  } else {
    // Afficher le réglage des minutes
    display.print("Minutes: ");
    if (valeur < 10) {
      display.print("0");
    }
    display.print(valeur);
  }

  display.display();
}

// *** Fonction pour changer le format de l'heure ***
void changerFormatHeure() {
    // Alterner entre 24h et 12h
    format24h = !format24h;
}






unsigned long lastDebounceTime = 0;  // Dernière fois que le bouton a été appuyé
unsigned long debounceDelay = 50;     // Temps de debouncing

void verifierBouton() {
  unsigned long currentMillis = millis();
  bool boutonEtatActuel = digitalRead(boutonPin);

  // Vérifier si l'état du bouton a changé
  if (boutonEtatActuel == LOW && (currentMillis - lastDebounceTime) > debounceDelay) {
    boutonSelectionne = true;
    lastDebounceTime = currentMillis;  // Mettre à jour le temps de debouncing
  }

  boutonEtatPrecedent = boutonEtatActuel; // Mettre à jour l'état précédent
}

// *** Afficher le menu sur l'OLED ***
void afficherMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  for (int i = 0; i < numMenuItems; i++) {
    int yPos = i * 10;
    if (i == menuIndex) {
      display.fillRect(0, yPos, SCREEN_WIDTH, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(0, yPos);
    display.print(i == menuIndex ? "> " : "  ");
    display.print((i == 0) ? "Reglage Heure" :
                  (i == 1) ? "Modifier Reveil" :
                  (i == 2) ? "Format 24h/12h" :
                  (i == 3) ? "On/Off Reveil" :
                  (i == 4) ? "Choisir Melodie" :
                             "Reglage Snooze");
  }
  display.display();
}


// *** Afficher l'heure et les minutes ***
void afficherHeureEtMinutes(int heures, int minutes) {
  // Séparer les chiffres des heures et des minutes
  int heureDizaine = heures / 10;
  int heureUnite = heures % 10;
  int minuteDizaine = minutes / 10;
  int minuteUnite = minutes % 10;

  // Afficher les heures (1ère matrice pour la dizaine des heures)
  afficherCaractere(chiffres[heureDizaine], 3); // Dizaine des heures sur la matrice 1

  // Afficher l'unité des heures avec les deux-points sur la 2e matrice
  byte matriceHeureUnite[8];
  for (int i = 0; i < 8; i++) {
    matriceHeureUnite[i] = chiffres[heureUnite][i]; // Copie de l'unité des heures
    if (colonState) {
      matriceHeureUnite[i] |= deuxPoints[i]; // Superpose les deux-points
    }
  }
  afficherCaractere(matriceHeureUnite, 2); // Unité des heures + deux-points sur la 2e matrice

  // Afficher les dizaines des minutes sur la 3e matrice
  afficherCaractere(chiffres[minuteDizaine], 1); // Dizaine des minutes

  // Afficher les unités des minutes sur la 4e matrice
  afficherCaractere(chiffres[minuteUnite], 0); // Unité des minutes
}

// *** Afficher un caractère sur une matrice spécifique ***
void afficherCaractere(byte caractere[8], int adresse) {
  matriceLed.clearDisplay(adresse);
  if (caractere != nullptr) {
    for (int i = 0; i < 8; i++) {
      matriceLed.setRow(adresse, i, caractere[i]);
    }
  }
}





void reglerHorloge() {
  // Initialisation des variables
  bool ajusterHeure = true;
  bool ajusterMinutes = false;
  int currentHour = rtc.now().hour();
  int currentMinute = rtc.now().minute();
  int newHour = currentHour;  // Nouvelle heure modifiée
  int newMinute = currentMinute;  // Nouvelle minute modifiée
  int lastPotentiometerValue = analogRead(potentiometerPin);
  bool boutonPrecedemmentAppuye = false; // Pour gérer le débouncing

  // Initialiser l'affichage avec l'heure actuelle
  afficherHeureEtMinutes(currentHour, currentMinute);

  // Réglage des heures
  while (ajusterHeure) {
    delay(1000);
    int potValue = analogRead(potentiometerPin);
    
    // Utiliser un seuil pour stabiliser la lecture du potentiomètre
    if (abs(potValue - lastPotentiometerValue) > 50) { // Seuil ajustable pour éviter les changements rapides
      lastPotentiometerValue = potValue;

      // Ajuster l'heure en fonction du potentiomètre
      newHour = map(potValue, 0, 1023, 0, 23);

      // Affichage de l'heure modifiée sur l'écran OLED
      afficherHeureEtMinutes(newHour, newMinute);
    }


    // Vérifier si le bouton est appuyé pour valider et passer à la modification des minutes
    if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
      boutonPrecedemmentAppuye = true;  // Marquer que le bouton a été appuyé
      delay(300); // Débouncing

      ajusterHeure = false;
      ajusterMinutes = true;  // Passer au réglage des minutes
    }

    // Attendre que le bouton soit relâché pour éviter les erreurs de multiple appui
    if (digitalRead(boutonPin) == HIGH) {
      boutonPrecedemmentAppuye = false;
    }
  }

  // Réglage des minutes
  while (ajusterMinutes) {
    int potValue = analogRead(potentiometerPin);

    // Utiliser un seuil pour stabiliser la lecture du potentiomètre
    if (abs(potValue - lastPotentiometerValue) > 50) { // Seuil ajustable pour éviter les changements rapides
      lastPotentiometerValue = potValue;

      // Ajuster les minutes en fonction du potentiomètre
      newMinute = map(potValue, 0, 1023, 0, 59);

      // Affichage des minutes modifiées sur l'écran OLED
      afficherHeureEtMinutes(newHour, newMinute);
    }


    // Vérifier si le bouton est appuyé pour valider les réglages et quitter
    if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
      boutonPrecedemmentAppuye = true;  // Marquer que le bouton a été appuyé
      delay(300); // Débouncing

      ajusterMinutes = false;  // Fin du réglage

      
      // Petite pause pour afficher la confirmation
      delay(2000); // Afficher la confirmation pendant 2 secondes

  rtc.adjust(DateTime(2024, 12, 11, newHour, newMinute, 0));  // Réglez le RTC à une heure précise

  // Vérification de l'heure du RTC après la mise à jour
  DateTime updatedTime = rtc.now();
  newHour = updatedTime.hour();
  newMinute = updatedTime.minute();

  // Mettre à jour la matrice LED avec la nouvelle heure et minutes
  afficherHeureEtMinutes(newHour, newMinute);  // **Important** : mettre à jour la matrice LED
    }

    // Attendre que le bouton soit relâché pour éviter les erreurs de multiple appui
    if (digitalRead(boutonPin) == HIGH) {
      boutonPrecedemmentAppuye = false;
    }
  }
}


void gererMenu() {
  static bool boutonPrecedemmentAppuye = false; // Pour gérer le débouncing
  bool buttonState = digitalRead(boutonPin);

  // Si le bouton est appuyé et qu'il n'était pas déjà pressé
  if (buttonState == LOW && !boutonPrecedemmentAppuye) {
    boutonPrecedemmentAppuye = true; // Marquer que le bouton est pressé

    // Gérer les actions en fonction de l'item du menu sélectionné
    switch (menuIndex) {
      case 0:
        // Appel à une fonction bloquante ou non
        reglerHorloge();
        break;
      case 1:
        reglerReveil();
        break;
      case 2:
        changerFormatHeure();
        break;
      case 3:
        buzzer();
        break;
      case 4:
        break;
      case 6:
        break;
    }

    delay(300); // Anti-rebond
  }

  // Si le bouton est relâché, réinitialiser son état
  if (buttonState == HIGH) {
    boutonPrecedemmentAppuye = false;
  }
}



// Déclaration des variables pour l'heure du réveil
int reveilHeure = 7;   // Heure initiale du réveil
int reveilMinute = 30; // Minute initiale du réveil


// Fonction pour régler l'heure du réveil
void reglerReveil() {
  // Initialisation des variables
  bool ajusterHeure = true;
  bool ajusterMinutes = false;
  int lastPotentiometerValue = analogRead(potentiometerPin);
  bool boutonPrecedemmentAppuye = false; // Pour gérer le débouncing

  // Initialiser l'affichage avec l'heure actuelle du réveil
  afficherHeureEtMinutes(reveilHeure, reveilMinute);

  // Réglage des heures
  while (ajusterHeure) {
    delay(1000);
    int potValue = analogRead(potentiometerPin);

    // Utiliser un seuil pour stabiliser la lecture du potentiomètre
    if (abs(potValue - lastPotentiometerValue) > 50) { // Seuil ajustable pour éviter les changements rapides
      lastPotentiometerValue = potValue;

      // Ajuster l'heure en fonction du potentiomètre
      reveilHeure = map(potValue, 0, 1023, 0, 23);

      // Affichage de l'heure modifiée sur l'écran OLED
      afficherHeureEtMinutes(reveilHeure, reveilMinute);
    }


    // Vérifier si le bouton est appuyé pour valider et passer à la modification des minutes
    if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
      boutonPrecedemmentAppuye = true;  // Marquer que le bouton a été appuyé
      delay(300); // Débouncing

      ajusterHeure = false;
      ajusterMinutes = true;  // Passer au réglage des minutes
    }

    // Attendre que le bouton soit relâché pour éviter les erreurs de multiple appui
    if (digitalRead(boutonPin) == HIGH) {
      boutonPrecedemmentAppuye = false;
    }
  }

  // Réglage des minutes
  while (ajusterMinutes) {
    int potValue = analogRead(potentiometerPin);

    // Utiliser un seuil pour stabiliser la lecture du potentiomètre
    if (abs(potValue - lastPotentiometerValue) > 50) { // Seuil ajustable pour éviter les changements rapides
      lastPotentiometerValue = potValue;

      // Ajuster les minutes en fonction du potentiomètre
      reveilMinute = map(potValue, 0, 1023, 0, 59);

      // Affichage des minutes modifiées sur l'écran OLED
      afficherHeureEtMinutes(reveilHeure, reveilMinute);
    }


    // Vérifier si le bouton est appuyé pour valider les réglages et quitter
    if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
      boutonPrecedemmentAppuye = true;  // Marquer que le bouton a été appuyé
      delay(300); // Débouncing

      ajusterMinutes = false;  // Fin du réglage

      
      // Petite pause pour afficher la confirmation
      delay(2000); // Afficher la confirmation pendant 2 secondes
    }

    // Attendre que le bouton soit relâché pour éviter les erreurs de multiple appui
    if (digitalRead(boutonPin) == HIGH) {
      boutonPrecedemmentAppuye = false;
    }
  }
}



void verifierReveil() {
  // Récupérer l'heure actuelle du RTC
  DateTime currentTime = rtc.now();
  int currentHour = currentTime.hour();
  int currentMinute = currentTime.minute();

  // Vérifier si l'heure actuelle correspond à l'heure du réveil
  if (currentHour == reveilHeure && currentMinute == reveilMinute) {
    unsigned long startMillis = millis();
    bool snoozeActivated = false;

    while (millis() - startMillis < 10000) { // Le buzzer sonne pendant 10 secondes
      tone(BUZZER_PIN, 1000); // Fréquence de 1000 Hz
      delay(500);
      noTone(BUZZER_PIN);
      delay(500);

      // Mesurer la distance avec le HC-SR04
      long distance = mesurerDistance();
      if (distance > 0 && distance <= 10) { // Main détectée à moins de 10 cm
        snoozeActivated = true;
        break; // Quitter la boucle du buzzer
      }
    }

    noTone(BUZZER_PIN); // Arrêter le buzzer

    // Si le snooze est activé, reporter l'heure du réveil
    if (snoozeActivated) {
      DateTime snoozeTime = rtc.now() + TimeSpan(0, 0, SNOOZE_DURATION, 0); // Ajouter SNOOZE_DURATION minutes
      reveilHeure = snoozeTime.hour();
      reveilMinute = snoozeTime.minute();
    }
  }
}




// Fonction pour faire sonner le buzzer pendant 10 secondes
void buzzer() {
  unsigned long startMillis = millis();
  while (millis() - startMillis < 10000) { // 10 secondes
    tone(BUZZER_PIN, 1000);   // Fréquence de 1000 Hz pour faire sonner le buzzer
    delay(500);               // Attendre 500ms
    noTone(BUZZER_PIN);       // Arrêter le son du buzzer
    delay(500);               // Attendre encore 500ms avant de refaire sonner
  }
  noTone(BUZZER_PIN); // Arrêter le buzzer après 10 secondes
}


// Fonction pour mesurer la distance avec le HC-SR04
long mesurerDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Convertir la durée en distance (cm)
  return distance;
}

void reglerSnooze() {
  static bool enReglage = false;         // Indique si on est en mode de réglage
  static int lastPotValue = -1;          // Dernière valeur du potentiomètre
  static int tempSnoozeDuration = SNOOZE_DURATION; // Valeur temporaire
  static bool boutonPrecedemmentAppuye = false;    // Pour gérer le débouncing

  // Activer le mode de réglage lorsque l'utilisateur sélectionne cet item
  if (!enReglage) {
    enReglage = true; // Entrer en mode de réglage
    tempSnoozeDuration = SNOOZE_DURATION; // Initialiser avec la valeur actuelle
  }

  if (enReglage) {
    int potValue = analogRead(potentiometerPin);

    // Détecter un changement significatif de la position du potentiomètre
    if (abs(potValue - lastPotValue) > 50) {
      lastPotValue = potValue;

      // Mapper la valeur du potentiomètre à une durée entre 1 et 15 minutes
      tempSnoozeDuration = map(potValue, 0, 1023, 1, 15);

      // Afficher la durée temporaire du snooze sur la matrice LED
      matriceLed.clearDisplay(1);
      afficherCaractere(chiffres[tempSnoozeDuration / 10], 1); // Dizaine
      afficherCaractere(chiffres[tempSnoozeDuration % 10], 0); // Unité
    }

    // Vérifier si le bouton est pressé pour valider la sélection
    if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
      boutonPrecedemmentAppuye = true; // Bouton pressé
      SNOOZE_DURATION = tempSnoozeDuration; // Appliquer la nouvelle durée
      enReglage = false; // Quitter le mode de réglage

      // Afficher brièvement la confirmation
      matriceLed.clearDisplay(1);
      afficherCaractere(chiffres[SNOOZE_DURATION / 10], 1); // Dizaine
      afficherCaractere(chiffres[SNOOZE_DURATION % 10], 0); // Unité
      delay(1000); // Pause pour confirmer
      matriceLed.clearDisplay(1);
    }

    // Réinitialiser l'état du bouton lorsqu'il est relâché
    if (digitalRead(boutonPin) == HIGH) {
      boutonPrecedemmentAppuye = false;
    }
  }
}



void choisirMelodie() {
  static bool enReglage = false;                 // Indique si on est en mode de réglage
  static int lastPotentiometerValue = -1;        // Dernière valeur lue du potentiomètre
  static bool boutonPrecedemmentAppuye = false; // Pour gérer le débouncing
  static int tempMelody = selectedMelody;        // Mélodie temporaire

  if (!enReglage) {
    // Initialiser les variables au début du réglage
    enReglage = true;
    tempMelody = selectedMelody;
    lastPotentiometerValue = analogRead(potentiometerPin);

    // Effacer l'écran OLED
    display.clearDisplay();
    display.display();
  }

  // Lire la valeur du potentiomètre
  int potValue = analogRead(potentiometerPin);

  // Détecter un changement significatif de la position du potentiomètre
  if (abs(potValue - lastPotentiometerValue) > 50) {
    lastPotentiometerValue = potValue;

    // Mapper la valeur du potentiomètre à une mélodie (1 à 3)
    tempMelody = map(potValue, 0, 1023, 1, 3);

    // Afficher la mélodie temporaire sur l'écran OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Choisir");
    display.setTextSize(3);
    display.setCursor(40, 30);
    display.print(tempMelody); // Afficher le numéro de la mélodie
    display.display();
  }

  // Vérifier si le bouton est pressé pour valider la sélection
  if (digitalRead(boutonPin) == LOW && !boutonPrecedemmentAppuye) {
    boutonPrecedemmentAppuye = true;  // Marquer que le bouton a été appuyé
    selectedMelody = tempMelody;     // Valider la sélection
    enReglage = false;               // Quitter le mode de réglage

    // Afficher la confirmation sur l'écran OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Melodie");
    display.setTextSize(3);
    display.setCursor(40, 30);
    display.print(selectedMelody); // Afficher la mélodie sélectionnée
    display.display();
    delay(1000); // Pause pour confirmer
    display.clearDisplay();
  }

  // Réinitialiser l'état du bouton lorsqu'il est relâché
  if (digitalRead(boutonPin) == HIGH) {
    boutonPrecedemmentAppuye = false;
  }
}





