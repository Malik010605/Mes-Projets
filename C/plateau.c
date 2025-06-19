#include <stdio.h>
#include <stdlib.h>
#include "plateau.h"
#include "joueur.h"

void initialiser_plateau(Plateau *p) { // Sous programme qui vient initialiser le tableau
    for (int i = 0; i < TAILLE_AFFICHAGE; i++) {
        for (int j = 0; j < TAILLE_AFFICHAGE; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                p->cases[i][j] = '.'; // Case vide pour les pions
            } else if (i % 2 == 1 && j % 2 == 0) {
                p->cases[i][j] = ' '; // Espace vertical pour les barrières
            } else if (i % 2 == 0 && j % 2 == 1) {
                p->cases[i][j] = ' '; // Espace horizontal pour les barrières
            } else {
                p->cases[i][j] = ' '; // Intersection des barrières
            }
        }
    }
}


void afficher_plateau(const Plateau *p) { // Sous programme qui affiche le tableau initialiser precedemment
    printf("   ");
    for (int col = 0; col < TAILLE_AFFICHAGE; col++) {
        if (col % 2 == 0) {
            printf(" %d ", col / 2);
        } else {
            printf("   ");
        }
    }
    printf("\n");

    for (int i = 0; i < TAILLE_AFFICHAGE; i++) {
        if (i % 2 == 0) {
            printf(" %d ", i / 2);  // Afficher les indices des lignes
        } else {
            printf("    ");  // Espacement pour les lignes impaires
        }

        for (int j = 0; j < TAILLE_AFFICHAGE; j++) {
            printf(" %c", p->cases[i][j]);  // Afficher chaque case
            if (j % 2 == 0) {
                printf(" ");  // Espacement pour les cases paires
            }
        }
        printf("\n");
    }
}
