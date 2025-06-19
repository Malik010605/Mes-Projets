#include "joueur.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>




void initialiser_joueurs(Joueur joueurs[], int nb_joueurs, Plateau *p) { // Sous programme qui s'occupe de l'integralite des fonctionnalites liees aux joueurs
    int barrieres_par_joueur;

    // Initialisation du nombre de barrières en fonction du nombre de joueurs
    if (nb_joueurs == 2) { // Si le jeu est joue a deux
        barrieres_par_joueur = 10; // Chaque joueur a 10 barrieres
    } else if (nb_joueurs == 4) { // Si partie jouee a 4
        barrieres_par_joueur = 5; // Chaque joueur a 5 barrieres
    } else { // Sinon
        // Gérer les cas où le nombre de joueurs est incorrect
        printf("Erreur : Le nombre de joueurs doit etre soit 2, soit 4.\n"); // Affichage de message d'erreur indiquant que la partie ne se joue qu'a 2 ou 4 joueurs (blindange de la saisie)
        return;
    }

    // Demander aux joueurs de choisir un symbole pour leur pion
    for (int i = 0; i < nb_joueurs; i++) {
        printf("Joueur %d, choisissez un symbole pour votre pion : ", i + 1);
        char symbole;
        scanf(" %c", &symbole);

        // Initialiser le joueur avec le symbole choisi
        joueurs[i].symbole = symbole;
        joueurs[i].score = 0;
        joueurs[i].barrieres_restantes = barrieres_par_joueur;
    }

    // Placer les joueurs sur le plateau
    if (nb_joueurs == 2) { // Si la partie est jouee a deux joueurs
        // Coordonnes du joueur 1
        joueurs[0].x = 0;
        joueurs[0].y = TAILLE_PLATEAU / 2;
        // Coordonnes du joueur 2
        joueurs[1].x = TAILLE_PLATEAU - 1;
        joueurs[1].y = TAILLE_PLATEAU / 2;


        p->cases[0 * 2][TAILLE_PLATEAU / 2 * 2] = joueurs[0].symbole; // Position du joueur 1
        p->cases[(TAILLE_PLATEAU - 1) * 2][TAILLE_PLATEAU / 2 * 2] = joueurs[1].symbole; // Position du joueur 2

    } else if (nb_joueurs == 4) { // Si la partie est jouee a 4 joueurs
        // Coordonnes du joueur 1
        joueurs[0].x = 0;
        joueurs[0].y = TAILLE_PLATEAU / 2;
        // Coordonnes du joueur 2
        joueurs[1].x = TAILLE_PLATEAU - 1;
        joueurs[1].y = TAILLE_PLATEAU / 2;
        // Coordonnes du joueur 3
        joueurs[2].x = TAILLE_PLATEAU / 2;
        joueurs[2].y = 0;
        // Coordonnes du joueur 4
        joueurs[3].x = TAILLE_PLATEAU / 2;
        joueurs[3].y = TAILLE_PLATEAU - 1;


        p->cases[0 * 2][TAILLE_PLATEAU / 2 * 2] = joueurs[0].symbole; // Position du joueur 1
        p->cases[(TAILLE_PLATEAU - 1) * 2][TAILLE_PLATEAU / 2 * 2] = joueurs[1].symbole; // Position du joueur 2
        p->cases[TAILLE_PLATEAU / 2 * 2][0 * 2] = joueurs[2].symbole; // Position du joueur 3
        p->cases[TAILLE_PLATEAU / 2 * 2][(TAILLE_PLATEAU - 1) * 2] = joueurs[3].symbole; // Position du joueur 4

    }
}


void deplacer_pion(Joueur *joueur, int new_x, int new_y, Plateau *p) { // Sous programme regissant toute la partie du deplacement des pions
    int old_x = joueur->x * 2;
    int old_y = joueur->y * 2;
    int nx = new_x * 2;
    int ny = new_y * 2;

    // Si la case cible est occupée par un autre pion, gestion des sauts
    if (abs(nx - old_x) + abs(ny - old_y) == 2 && p->cases[nx][ny] != '.') {
        char pion_bloquant = p->cases[nx][ny];
        if (pion_bloquant != '|' && pion_bloquant != '-') {
            int jump_x = nx + (nx - old_x);
            int jump_y = ny + (ny - old_y);

            // Vérification du saut
            if (jump_x >= 0 && jump_x < TAILLE_AFFICHAGE &&
                jump_y >= 0 && jump_y < TAILLE_AFFICHAGE &&
                p->cases[jump_x][jump_y] == '.' && verifier_mouvement(nx, ny, jump_x, jump_y, p)) {
                // Vider l'ancienne case
                p->cases[old_x][old_y] = '.';
                // Mettre à jour la position du joueur
                joueur->x = jump_x / 2;
                joueur->y = jump_y / 2;
                // Placer le pion sur la nouvelle case après le saut
                p->cases[jump_x][jump_y] = joueur->symbole;
                return;
                }
        }
    }

    // Vérification du déplacement
    if (abs(nx - old_x) + abs(ny - old_y) == 2 && verifier_mouvement(old_x, old_y, nx, ny, p)) {
        if (p->cases[nx][ny] == '.') {
            // Vider l'ancienne case
            p->cases[old_x][old_y] = '.';
            // Mettre à jour la position du joueur
            joueur->x = new_x;
            joueur->y = new_y;
            // Placer le pion sur la nouvelle case
            p->cases[nx][ny] = joueur->symbole;
        } else {
            printf("Deplacement invalide ! La case est occupee.\n");
        }
    } else {
        printf("Deplacement invalide ! Soit le mouvement est trop grand, soit une barriere empeche le deplacement.\n");
    }
}


int placer_barriere(Plateau *p, int x, int y, int orientation, Joueur *joueur) { // Sous programme regissant toute la partie du placement des barrieres
    // Vérification du nombre de barrières restantes
    if (joueur->barrieres_restantes <= 0) { // Si le joueur essaye de placer une barriere alors qu'il n'en a plus
        printf("Le joueur %c n'a plus de barrieres disponibles.\n", joueur->symbole);
        return 0;
    }

    if (x < 0 || y < 0 || x >= TAILLE_PLATEAU || y >= TAILLE_PLATEAU) { // Si le joueur essaye de mettre des barrieres en dehors de la zone de jeu
        printf("Coordonnees invalides pour la barriere.\n");
        return 0;
    }

    if (orientation == 1) { // Barrière verticale
        if (x < TAILLE_PLATEAU && p->cases[x * 2 + 1][y * 2] == ' ' && p->cases[x * 2 + 3][y * 2] == ' ') {
            p->cases[x * 2 + 1][y * 2] = '|';
            p->cases[x * 2 + 3][y * 2] = '|';
            joueur->barrieres_restantes--; // Réduire le nombre de barrières restantes
            return 1;
        } else {
            printf("Impossible de placer la barriere ! Les cases sont deja occupees.\n");
            return 0;
        }
    } else if (orientation == 2) { // Barrière horizontale
        if (y < TAILLE_PLATEAU && p->cases[x * 2][y * 2 + 1] == ' ' && p->cases[x * 2][y * 2 + 3] == ' ') {
            p->cases[x * 2][y * 2 + 1] = '-';
            p->cases[x * 2][y * 2 + 3] = '-';
            joueur->barrieres_restantes--; // Réduire le nombre de barrières restantes
            return 1;
        } else {
            printf("Impossible de placer la barriere ! Les cases sont deja occupees.\n");
            return 0;
        }
    } else {
        printf("Orientation invalide ! Utilisez 1 pour verticale et 2 pour horizontale.\n");
        return 0;
    }
}


void afficher_plateau_et_scores(const Plateau *p, Joueur joueurs[], int nb_joueurs) { // Affiche simplement le tableau, les joueurs avec leurs pions ainsi que leurs scores
    printf("\nPlateau de jeu:\n");

    printf("   ");
    for (int col = 0; col < TAILLE_AFFICHAGE; col++) { // Boucle affichant les colonnes du tableau de jeu
        if (col % 2 == 0) {
            printf(" %d ", col / 2);
        } else {
            printf("  ");
        }
    }
    printf("\n");

    for (int i = 0; i < TAILLE_AFFICHAGE; i++) { // Boucle qui espace chaque case
        if (i % 2 == 0) {
            printf(" %d ", i / 2);
        } else {
            printf("  ");
        }

        for (int j = 0; j < TAILLE_AFFICHAGE; j++) { // Boucle qui affiche les cases du tableau de jeu
            printf(" %c", p->cases[i][j]);
            if (j % 2 == 0) {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("\nScores et barrieres restantes :\n");
    for (int j = 0; j < nb_joueurs; j++) {
        printf("Joueur (%c): %s (Score: %d, Barrieres restantes: %d)\n", joueurs[j].symbole, joueurs[j].nom, joueurs[j].score, joueurs[j].barrieres_restantes);
    }
    printf("\n");
}


int verifier_gagnant(Joueur *joueurs, int nb_joueurs) {
    // Vérification des conditions de victoire pour chaque joueur
    for (int i = 0; i < nb_joueurs; i++) {
        Joueur *joueur = &joueurs[i];

        // Vérification si le joueur a atteint la ligne d'arrivée (dernière ligne ou colonne)
        if (nb_joueurs == 2) {
            // Vérification de la victoire pour un jeu à 2 joueurs
            // Joueur 1 doit atteindre la dernière ligne (ligne 8)
            if (joueur->x == 8 && joueur->symbole == joueurs[0].symbole) {
                return 1; // Le joueur a gagné
            }
            // Joueur 2 doit atteindre la première ligne (ligne 0)
            if (joueur->x == 0 && joueur->symbole == joueurs[1].symbole) {
                return 1; // Le joueur a gagné
            }
        } else if (nb_joueurs == 4) {
            // Vérification de la victoire pour un jeu à 4 joueurs
            // Joueur 1 doit atteindre la dernière ligne (ligne 8)
            if (joueur->x == 8 && joueur->symbole == joueurs[0].symbole) {
                return 1; // Le joueur a gagné
            }
            // Joueur 2 doit atteindre la première ligne (ligne 0)
            if (joueur->x == 0 && joueur->symbole == joueurs[1].symbole) {
                return 1; // Le joueur a gagné
            }
            // Joueur 3 doit atteindre la dernière ligne (ligne 8)
            if (joueur->y == 8 && joueur->symbole == joueurs[2].symbole) {
                return 1; // Le joueur a gagné
            }
            // Joueur 4 doit atteindre la première ligne (ligne 0)
            if (joueur->y == 0 && joueur->symbole == joueurs[3].symbole) {
                return 1; // Le joueur a gagné
            }
        }
    }

    return 0; // Aucun gagnant
}








void charger_scores(Joueur *joueurs, int *nb_joueurs) { // Sous programme qui lit le fichier texte 'sauvegarde' et le charge dans la partie
    FILE *fichier = fopen("scores.txt", "r"); // Ouvre le fichier en mode lecture ("r")
    if (fichier == NULL) {
        printf("Aucun fichier de scores trouve. Les scores seront initialises à 0.\n");
        return;
    }

    // Variable temporaire pour lire les données du fichier
    char nom[50];
    char symbole;
    int score;

    *nb_joueurs = 0; // Initialiser le nombre de joueurs à 0

    // Lire chaque ligne du fichier et mettre à jour les joueurs
    while (fscanf(fichier, "%s %c %d", nom, &symbole, &score) == 3) {
        // Chercher si ce joueur existe déjà
        int joueur_trouve = 0;
        for (int i = 0; i < *nb_joueurs; i++) {
            if (strcmp(joueurs[i].nom, nom) == 0) { // Comparer les noms des joueurs
                joueurs[i].score = score; // Mettre à jour le score
                joueur_trouve = 1;
                break;
            }
        }

        // Si le joueur n'existe pas, on l'ajoute à la liste des joueurs
        if (!joueur_trouve) {
            strcpy(joueurs[*nb_joueurs].nom, nom);
            joueurs[*nb_joueurs].symbole = symbole;
            joueurs[*nb_joueurs].score = score;
            (*nb_joueurs)++;
        }
    }

    fclose(fichier); // Ferme le fichier
    printf("Scores restaures avec succes.\n");
}


void sauvegarder_scores(Joueur *joueurs, int nb_joueurs) {
    FILE *fichier = fopen("scores.txt", "w"); // Ouvre le fichier en mode écriture ("w")
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour sauvegarder les scores.\n");
        return;
    }

    for (int i = 0; i < nb_joueurs; i++) {
        fprintf(fichier, "%s %c %d\n", joueurs[i].nom, joueurs[i].symbole, joueurs[i].score);
    }

    fclose(fichier); // Ferme le fichier
    printf("Scores sauvegardes avec succes.\n");
}


void attribuer_scores(Joueur joueurs[], int nb_joueurs) { // Sous programme qui attribue un score au nom du joueur qui le detient
    Joueur anciens_joueurs[50];
    int nb_anciens = 0;

    charger_scores(anciens_joueurs, &nb_anciens);

    for (int i = 0; i < nb_joueurs; i++) {
        for (int j = 0; j < nb_anciens; j++) {
            if (strcmp(joueurs[i].nom, anciens_joueurs[j].nom) == 0) {
                joueurs[i].score = anciens_joueurs[j].score;
                break;
            }
        }
    }
}











