#ifndef JOUEUR_H
#define JOUEUR_H

#include "plateau.h"

typedef struct {
    char symbole;  // Symbole pour identifier le pion du joueur
    int x, y;      // Coordonnées actuelles du pion
    char nom[50]; // Nom du joueur qui doit faire maximum 50 caracteres
    int score; // Score du joueur
    int barrieres_restantes; // Nombre de barrieres restantes au joueur
} Joueur; // Structure joueur


void initialiser_joueurs(Joueur joueurs[], int nb_joueurs, Plateau *p); // Sous programme qui s'occupe de l'integralite des fonctionnalites liees aux joueurs
void deplacer_pion(Joueur *joueur, int new_x, int new_y, Plateau *p); // Sous programme qui s'occupe de l'integralite des fonctionnalites liees aux joueurs
int placer_barriere(Plateau *p, int x, int y, int orientation, Joueur *joueur); // Sous programme regissant toute la partie du placement des barrieres
int verifier_gagnant(Joueur *joueur,int nb_joueurs); // Sous programme qui verifie la condition de victoire
void sauvegarder_scores(Joueur *joueurs, int nb_joueurs); // Sous programme qui enregistre les scores dans un fichier texte 'scores.txt'
void afficher_plateau_et_scores(const Plateau *plateau, Joueur joueurs[], int nb_joueurs); // Affiche simplement le tableau, les joueurs avec leurs pions ainsi que leurs scores
void charger_scores(Joueur *joueurs, int *nb_joueurs); // Sous programme qui lit le fichier texte 'sauvegarde' et le charge dans la partie
void attribuer_scores(Joueur joueurs[], int nb_joueurs); // Sous programme qui attribue un score au nom du joueur qui le detient

#endif // JOUEUR_H