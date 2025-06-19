#ifndef PLATEAU_H
#define PLATEAU_H
#include "quoridor.h"
#define TAILLE_PLATEAU 9
#define TAILLE_AFFICHAGE (2 * TAILLE_PLATEAU - 1)



typedef struct {
    char cases[TAILLE_AFFICHAGE][TAILLE_AFFICHAGE]; // Plateau avec affichage et barrières
} Plateau;

void initialiser_plateau(Plateau *p); // Sous programme qui vient initialiser le tableau
void afficher_plateau(const Plateau *p); // Sous programme qui affiche le tableau initialiser precedemment

#endif // PLATEAU_H
