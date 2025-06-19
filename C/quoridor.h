#ifndef QUORIDOR_H
#define QUORIDOR_H
#include "joueur.h"


void gerer_menu(); // Sous programme qui gere le choix du joueur
void afficher_aide(); // Sous programme affichant l'aide dans le menu pour comprendre les regles du jeu
void afficher_scores(Joueur joueurs[], int nb_joueurs); // Sous programme qu'on appelle dans le menu pour afficher les scores des joueurs passés
void sauvegarder_partie(Plateau *plateau, Joueur joueurs[], int nb_joueurs, int joueur_actuel); // Sous programme enregistrant la partie sur un fichier texte appele 'sauvegarde'
int charger_partie(Plateau *plateau, Joueur joueurs[], int *nb_joueurs, int *joueur_actuel); // Sous programme qui vient lire le sauvegarde.txt pour charger la partie enregistree
void lancer_partie(Plateau *plateau, Joueur joueurs[], int nb_joueurs); // Sous programme qu'on appelle dans le case 1 du switch 'choix' qui initialise et lance la partie
int pion_bloque(Joueur *joueur, const Plateau *p); // Sous programme qu'on utilisera plus tard pour mettre fin a la partie si le joueur est entoure de pions
int verifier_deplacement(Joueur *joueur, int x, int y, Plateau *plateau); // Sous programme servant a empecher de sortir du cadre de jeu
#endif