#include <Arduino.h>
#include <TimerOne.h>  // Pour le métronome

// === Détection fréquence via interruption (D2)
const int freqPin = 2;
volatile unsigned long prev_time = 0;
volatile unsigned long period = 0;
float frequence = 0;

// === Moyenne glissante
const int tailleMoyenne = 5;
float historique[tailleMoyenne];
int indexHistorique = 0;
bool bufferPlein = false;

// === Boutons navigation
const int btnMonter = 7;
const int btnDescendre = 3;
const int btnValider = 4;
int lastMonter = HIGH;
int lastDescendre = HIGH;
int lastValider = HIGH;

// === Métronome
const int buzzerPin = 8;
int niveau = 1;
int interval = 800;  // en ms, niveau 1 par défaut

// === Notes musicales
String derniereNote = "";

struct Note {
  const char* nom;
  float freq;
};

Note notes[] = {
  {"DO", 261.63},
  {"RE", 293.66},
  {"MI", 329.63},
  {"FA", 349.23},
  {"SOL", 392.00},
  {"LA", 440.00},
  {"SI", 493.88},
  {"DO AIGU", 523.25}
};

const float tolerance = 15.0;

void mesureFrequence();  // interruption fréquence
void tickMetronome();    // métronome

// === Changer tempo selon niveau 1 à 4 ===
void setMetronomePourNiveau(int niveau) {
  if (niveau == 1) interval = 800;
  else if (niveau == 2) interval = 600;
  else if (niveau == 3) interval = 400;
  else if (niveau == 4) interval = 250;
  else interval = 800; // par défaut
  Timer1.initialize(interval * 1000L);
}

void setup() {
  Serial.begin(9600);

  pinMode(freqPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(freqPin), mesureFrequence, RISING);

  pinMode(btnMonter, INPUT_PULLUP);
  pinMode(btnDescendre, INPUT_PULLUP);
  pinMode(btnValider, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  setMetronomePourNiveau(niveau);  // niveau par défaut
  Timer1.attachInterrupt(tickMetronome);
}

void loop() {
  // === Gérer boutons navigation ===
  int etatMonter = digitalRead(btnMonter);
  int etatDescendre = digitalRead(btnDescendre);
  int etatValider = digitalRead(btnValider);

  if (etatMonter == LOW && lastMonter == HIGH) Serial.println("up");
  if (etatDescendre == LOW && lastDescendre == HIGH) Serial.println("down");
  if (etatValider == LOW && lastValider == HIGH) Serial.println("select");

  lastMonter = etatMonter;
  lastDescendre = etatDescendre;
  lastValider = etatValider;

  // === Lire niveau envoyé depuis Processing ===
  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '1' && c <= '4') {
      niveau = c - '0';
      setMetronomePourNiveau(niveau);
    }
  }

  // === Mesure fréquence (moyenne glissante)
  if (period > 0) {
    float mesure = 1000000.0 / period;
    historique[indexHistorique] = mesure;
    indexHistorique = (indexHistorique + 1) % tailleMoyenne;
    if (indexHistorique == 0) bufferPlein = true;

    int n = bufferPlein ? tailleMoyenne : indexHistorique;
    float somme = 0;
    for (int i = 0; i < n; i++) somme += historique[i];
    frequence = somme / n;

    // Comparer aux notes
    String note = "";
    float minDiff = 10000;
    for (int i = 0; i < sizeof(notes) / sizeof(notes[0]); i++) {
      float diff = abs(frequence - notes[i].freq);
      if (diff < minDiff) {
        minDiff = diff;
        note = notes[i].nom;
      }
    }

    if (minDiff < tolerance && note != derniereNote) {
      Serial.println(note);
      derniereNote = note;
    }

    if (minDiff >= tolerance) {
      derniereNote = "";
    }
  }

  delay(10);
}

void mesureFrequence() {
  unsigned long now = micros();
  period = now - prev_time;
  prev_time = now;
}

// === Métronome : son dans l'interruption ===
void tickMetronome() {
  tone(buzzerPin, 1000, 30);  // 1000 Hz pendant 30ms
}
