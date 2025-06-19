#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "quoridor.h"
#include "plateau.h"
#include "joueur.h"

void clear() { // Sous programme permettant d'actualiser l'ecran a chaque appel (sous windows)
    system("cls");
}


int verifier_deplacement(Joueur *joueur, int x, int y, Plateau *plateau) { // Sous programme servant a empecher de sortir du cadre de jeu
    // Vérification des limites du plateau
    if (x < 0 || x >= TAILLE_PLATEAU || y < 0 || y >= TAILLE_PLATEAU) {
        return 0; // Déplacement invalide si hors limites
    }
    // Ajoutez d'autres vérifications ici si nécessaire
    return 1; // Déplacement valide
}


void afficher_aide() { // Sous programme affichant l'aide dans le menu pour comprendre les regles du jeu
    printf("Regles du jeu de Quoridor:\n");
    printf("\n");
    printf("Quoridor est un jeu de strategie ou l'objectif est de faire traverser son pion\n");
    printf("du cote oppose du plateau, tout en bloquant les autres joueurs avec des barrieres.\n");
    printf("\n");

    printf("1. Plateau de jeu:\n");
    printf("   - Le plateau est compose de 9x9 cases.\n");
    printf("   - Chaque joueur commence avec un pion situe au centre de la ligne la plus proche de son cote.\n");
    printf("   - Les joueurs peuvent deplacer leur pion sur les cases adjacentes.\n");
    printf("\n");

    printf("2. Deplacement des pions:\n");
    printf("   - Chaque joueur peut deplacer son pion d'une case par tour.\n");
    printf("   - Le pion peut se deplacer dans l'une des 4 directions: Haut (H), Bas (B), Gauche (G), Droite (D).\n");
    printf("   - Le mouvement est limite par les bords du plateau et les barrieres.\n");
    printf("\n");

    printf("3. Barrieres:\n");
    printf("   - Chaque joueur possede un certain nombre de barrieres qu'il peut placer sur le plateau.\n");
    printf("   - Une barriere peut etre placee de maniere verticale ou horizontale, et bloque la progression des pions.\n");
    printf("   - Les barrieres ne peuvent pas se toucher, et ne peuvent pas etre placees sur les cases occupees par des pions.\n");
    printf("\n");

    printf("4. Objectif du jeu:\n");
    printf("   - Le but du jeu est de deplacer son pion du cote oppose du plateau tout en evitant les obstacles et les barrieres.\n");
    printf("   - Le premier joueur a atteindre la ligne opposee avec son pion gagne la partie.\n");
    printf("\n");

    printf("5. Tour de jeu:\n");
    printf("   - Les joueurs jouent a tour de role.\n");
    printf("   - A chaque tour, un joueur peut choisir d'effectuer l'une des actions suivantes:\n");
    printf("     1. Deplacer son pion.\n");
    printf("     2. Placer une barriere.\n");
    printf("     3. Passer son tour.\n");
    printf("\n");

    printf("6. Conditions de victoire:\n");
    printf("   - Un joueur gagne des qu'il atteint la ligne la plus proche de l'adversaire.\n");
    printf("\n");

    printf("7. Raccourcis clavier:\n");
    printf("   - Vous pouvez entrer une direction (H, B, G, D) pour deplacer votre pion.\n");
    printf("   - Vous pouvez entrer les coordonnees (x, y) et l'orientation (1 ou 2) pour placer une barriere.\n");
    printf("   - Vous pouvez choisir l'action a effectuer en entrant le numero de l'action souhaitee (1, 2 ou 3).\n");

    printf("\n");
    printf("Bonne chance et amusez-vous bien !\n");
    printf("\n");
}


int verifier_mouvement(int old_x, int old_y, int new_x, int new_y, const Plateau *p) { // Sous programme qui vient verifier si le mouvement est faisable ou pas
    if (new_x < old_x) {
        return p->cases[old_x - 1][old_y] != '|';
    }
    if (new_x > old_x) {
        return p->cases[old_x + 1][old_y] != '|';
    }
    if (new_y < old_y) {
        return p->cases[old_x][old_y - 1] != '-';
    }
    if (new_y > old_y) {
        return p->cases[old_x][old_y + 1] != '-';
    }
    return 0;
}


int pion_bloque(Joueur *joueur, const Plateau *p) { // Sous programme qu'on utilisera plus tard pour mettre fin a la partie si le joueur est entoure de pions
    int x = joueur->x * 2;
    int y = joueur->y * 2;

    int deplacements[4][2] = {{x - 2, y}, {x + 2, y}, {x, y - 2}, {x, y + 2}};
    for (int i = 0; i < 4; i++) {
        if (deplacements[i][0] >= 0 && deplacements[i][0] < TAILLE_AFFICHAGE &&
            deplacements[i][1] >= 0 && deplacements[i][1] < TAILLE_AFFICHAGE &&
            p->cases[deplacements[i][0]][deplacements[i][1]] == '.' && verifier_mouvement(x, y, deplacements[i][0], deplacements[i][1], p)) {
            return 0; // Le pion n'est pas bloqué, il peut se déplacer
            }
    }
    return 1; // Le pion est bloqué
}


void afficher_scores(Joueur joueurs[], int nb_joueurs) { // Sous programme qu'on appelle dans le menu pour afficher les scores des joueurs passés
    printf("\nScores des joueurs :\n");
    for (int i = 0; i < nb_joueurs; i++) { // Boucle qui vient lire le score d'un joueur apres l'autre
        printf("Joueur %s : Score = %d\n", joueurs[i].nom, joueurs[i].score, joueurs[i].barrieres_restantes);
    }
    printf("\n");
}


void lancer_partie(Plateau *plateau, Joueur joueurs[], int nb_joueurs) {
    // Demander le nombre de joueurs avec validation
    do {
        printf("Nombre de joueurs (2 ou 4): ");
        if (scanf("%d", &nb_joueurs) != 1 || (nb_joueurs != 2 && nb_joueurs != 4)) {
            printf("Erreur : veuillez entrer 2 ou 4 joueurs.\n");
            while (getchar() != '\n'); // vider le buffer
        }
    } while (nb_joueurs != 2 && nb_joueurs != 4);

    initialiser_plateau(plateau); // Initialisation du plateau
    initialiser_joueurs(joueurs, nb_joueurs, plateau); // Initialisation des joueurs

    // Demande des noms et symboles des joueurs
    for (int i = 0; i < nb_joueurs; i++) {
        printf("Entrez le nom du joueur %d: ", i + 1);
        while (scanf("%s", joueurs[i].nom) != 1) {
            printf("Entree invalide. Essayez encore : ");
            while (getchar() != '\n'); // vider le buffer
        }
        joueurs[i].score = 0; // Score initial
    }

    // Attribution des symboles aux joueurs
    // Exemple : joueurs[0].symbole = 'X'; joueurs[1].symbole = 'O';
    attribuer_scores(joueurs, nb_joueurs); // Attribue les scores des joueurs

    srand(time(NULL)); // Initialisation d'un choix au hasard
    int joueur_actuel = rand() % nb_joueurs; // Choisir un joueur au hasard
    int action, x, y, orientation;

    // Boucle principale de la partie
    while (1) {
        clear();
        afficher_plateau_et_scores(plateau, joueurs, nb_joueurs); // Affichage du plateau et des scores
        printf("Joueur %s (%c), choisissez une action:\n", joueurs[joueur_actuel].nom, joueurs[joueur_actuel].symbole);
        printf("1. Deplacer un pion\n");
        printf("2. Placer une barriere\n");
        printf("3. Passer\n");
        printf("4. Sauvegarder et quitter\n");

        // Validation de l'action
        do {
            if (scanf("%d", &action) != 1 || action < 1 || action > 4) {
                printf("Action invalide! Veuillez entrer un chiffre entre 1 et 4.\n");
                while (getchar() != '\n'); // vider le buffer
            }
        } while (action < 1 || action > 4);

        if (action == 1) { // Déplacement du pion
            char direction;
            // Validation de la direction
            do {
                printf("Entrez la direction (H pour Haut, B pour Bas, G pour Gauche, D pour Droite): ");
                scanf(" %c", &direction);
                if (direction != 'H' && direction != 'B' && direction != 'G' && direction != 'D') {
                    printf("Direction invalide! Essayez de nouveau.\n");
                }
            } while (direction != 'H' && direction != 'B' && direction != 'G' && direction != 'D');

            x = joueurs[joueur_actuel].x;
            y = joueurs[joueur_actuel].y;

            // Calcul des nouvelles coordonnées en fonction de la direction
            switch (direction) {
                case 'H': x -= 1; break;
                case 'B': x += 1; break;
                case 'G': y -= 1; break;
                case 'D': y += 1; break;
            }

            // Vérification du déplacement
            if (verifier_deplacement(&joueurs[joueur_actuel], x, y, plateau)) {
                deplacer_pion(&joueurs[joueur_actuel], x, y, plateau);

                // Vérification de la victoire après chaque déplacement
                if (verifier_gagnant(joueurs, nb_joueurs)) {
                    printf("Felicitations, le joueur %s a gagne la partie!\n", joueurs[joueur_actuel].nom);
                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                    clear();
                    return; // Terminer la partie
                }

                // Vérification de la fin de la partie si le joueur est bloqué
                if (pion_bloque(&joueurs[joueur_actuel], plateau)) {
                    printf("Le joueur %s est bloque, la partie est terminee sans gagnant.\n", joueurs[joueur_actuel].nom);
                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                    clear();
                    return; // Terminer la partie
                }

                // Passer au joueur suivant
                joueur_actuel = (joueur_actuel + 1) % nb_joueurs;
            } else {
                printf("Deplacement invalide! Reessayez.\n");
            }
        } else if (action == 2) { // Placement de barrière
            // Validation des coordonnées et de l'orientation de la barrière
            do {
                printf("Entrez les coordonnees pour la barriere (x y) et l'orientation (1 pour verticale, 2 pour horizontale): ");
                if (scanf("%d %d %d", &x, &y, &orientation) != 3 || (orientation != 1 && orientation != 2)) {
                    printf("Entree invalide! Essayez à nouveau.\n");
                    while (getchar() != '\n'); // vider le buffer
                }
            } while (orientation != 1 && orientation != 2);

            if (placer_barriere(plateau, x, y, orientation, &joueurs[joueur_actuel])) {
                // Vérification de la victoire après le placement de la barrière
                if (verifier_gagnant(joueurs, nb_joueurs)) {
                    printf("Felicitations, le joueur %s a gagne la partie!\n", joueurs[joueur_actuel].nom);
                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                    clear();
                    return; // Terminer la partie
                }

                // Vérification de la fin de la partie si le joueur est bloqué
                if (pion_bloque(&joueurs[joueur_actuel], plateau)) {
                    printf("Le joueur %s est bloque, la partie est terminee sans gagnant.\n", joueurs[joueur_actuel].nom);
                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                    clear();
                    return; // Terminer la partie
                }

                // Passer au joueur suivant
                joueur_actuel = (joueur_actuel + 1) % nb_joueurs;
            } else {
                printf("Placement de barriere invalide! Reessayez.\n");
            }
        } else if (action == 3) { // Passer son tour
            printf("Le joueur %s passe son tour.\n", joueurs[joueur_actuel].nom);
            // Passer au joueur suivant
            joueur_actuel = (joueur_actuel + 1) % nb_joueurs;
        } else if (action == 4) { // Sauvegarder et quitter
            sauvegarder_partie(plateau, joueurs, nb_joueurs, joueur_actuel);
            printf("Partie sauvegardee et terminee. Vous pouvez la reprendre plus tard.\n");
            sleep(2);  // Attendre quelques secondes avant de quitter
            clear();
            return; // Quitter la partie
        } else {
            printf("Action invalide! Reessayez.\n");
        }
    }
}









void sauvegarder_partie(Plateau *plateau, Joueur joueurs[], int nb_joueurs, int joueur_actuel) { // Sous programme enregistrant la partie sur un fichier texte appele 'sauvegarde'
    FILE *fichier = fopen("sauvegarde.txt", "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde.\n");
        return;
    }



    // Sauvegarde du nombre de joueurs
    fprintf(fichier, "%d\n", nb_joueurs);

    // Sauvegarde des informations des joueurs
    for (int i = 0; i < nb_joueurs; i++) {
        fprintf(fichier, "%s %c %d %d %d %d\n", joueurs[i].nom, joueurs[i].symbole, joueurs[i].x, joueurs[i].y, joueurs[i].score, joueurs[i].barrieres_restantes);
    }

    // Sauvegarde du joueur actuel
    fprintf(fichier, "%d\n", joueur_actuel);

    fclose(fichier);
    printf("Partie sauvegardee avec succes.\n");
}





int charger_partie(Plateau *plateau, Joueur joueurs[], int *nb_joueurs, int *joueur_actuel) { // Sous programme qui vient lire le sauvegarde.txt pour charger la partie enregistree
    FILE *fichier = fopen("sauvegarde.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde.\n");
        return 0;
    }

    afficher_plateau(plateau);

    // Chargement du nombre de joueurs
    fscanf(fichier, "%d", nb_joueurs);

    // Chargement des informations des joueurs
    for (int i = 0; i < *nb_joueurs; i++) {
        fscanf(fichier, "%s %c %d %d %d %d", joueurs[i].nom, &joueurs[i].symbole, &joueurs[i].x, &joueurs[i].y, &joueurs[i].score, &joueurs[i].barrieres_restantes);
    }

    // Chargement du joueur actuel
    fscanf(fichier, "%d", joueur_actuel);

    fclose(fichier);
    printf("Partie chargee avec succes.\n");
    return 1;
}







void gerer_menu() { // Sous programme qui gere le choix du joueur
    int choix;
    Plateau plateau;
    Joueur joueurs[4];
    int nb_joueurs = 0;
    int joueur_actuel = 0;  // Déclarez joueur_actuel ici

    do {
        // Affichage du menu et choix de l'option
        printf("Menu:\n");
        printf("1. Lancer une nouvelle partie\n");
        printf("2. Reprendre une partie\n");
        printf("3. Afficher l'aide\n");
        printf("4. Afficher les scores\n");
        printf("5. Quitter\n");
        printf("Choisissez une option: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                // Lancer une nouvelle partie (option 1)
                lancer_partie(&plateau, joueurs, nb_joueurs);
                break;

            case 2:
                // Reprendre une partie (option 2)
                if (charger_partie(&plateau, joueurs, &nb_joueurs, &joueur_actuel)) {
                    // Partie chargée avec succès, afficher le plateau et les scores

                    // Boucle pour continuer la partie
                    while (1) {
                        // Afficher l'état de la partie, les joueurs et les scores
                        printf("Joueur %s (%c), choisissez une action:\n", joueurs[joueur_actuel].nom, joueurs[joueur_actuel].symbole);
                        printf("1. Deplacer un pion\n");
                        printf("2. Placer une barriere\n");
                        printf("3. Passer\n");
                        printf("4. Sauvegarder et quitter\n");
                        int action;
                        scanf("%d", &action);

                        if (action == 1) {
                            // Déplacement d'un pion
                            char direction;
                            printf("Entrez la direction (H pour Haut, B pour Bas, G pour Gauche, D pour Droite): ");
                            scanf(" %c", &direction);

                            int x = joueurs[joueur_actuel].x;
                            int y = joueurs[joueur_actuel].y;

                            switch (direction) {
                                case 'H': x -= 1; break;
                                case 'B': x += 1; break;
                                case 'G': y -= 1; break;
                                case 'D': y += 1; break;
                                default:
                                    printf("Direction invalide! Reessayez.\n");
                                    continue;
                            }

                            if (verifier_deplacement(&joueurs[joueur_actuel], x, y, &plateau)) {
                                deplacer_pion(&joueurs[joueur_actuel], x, y, &plateau);
                                // Vérification de la fin de la partie (si le joueur est bloqué)
                                if (pion_bloque(&joueurs[joueur_actuel], &plateau)) {
                                    printf("Le joueur %s est bloque, la partie est terminee sans gagnant.\n", joueurs[joueur_actuel].nom);
                                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                                    clear();
                                    return; // Terminer la partie
                                }
                            } else {
                                printf("Deplacement invalide! Reessayez.\n");
                            }
                        } else if (action == 2) {
                            // Placement d'une barrière
                            int x, y, orientation;
                            printf("Entrez les coordonnees pour la barriere (x y) et l'orientation (1 pour verticale, 2 pour horizontale): ");
                            scanf("%d %d %d", &x, &y, &orientation);

                            if (placer_barriere(&plateau, x, y, orientation, &joueurs[joueur_actuel])) {
                                // Vérification de la fin de la partie (si le joueur est bloqué)
                                if (pion_bloque(&joueurs[joueur_actuel], &plateau)) {
                                    printf("Le joueur %s est bloque, la partie est terminee sans gagnant.\n", joueurs[joueur_actuel].nom);
                                    sauvegarder_scores(joueurs, nb_joueurs); // Sauvegarder les scores
                                    sleep(3); // Attendre quelques secondes avant de fermer la partie
                                    clear();
                                    return; // Terminer la partie
                                }
                            } else {
                                printf("Placement de la barriere invalide! Reessayez.\n");
                            }
                        } else if (action == 3) {
                            // Passer le tour
                            printf("Le joueur %s passe son tour.\n", joueurs[joueur_actuel].nom);
                        } else if (action == 4) {
                            // Sauvegarder et quitter
                            sauvegarder_partie(&plateau, joueurs, nb_joueurs, joueur_actuel);
                            printf("Partie sauvegardee et terminee. Vous pouvez la reprendre plus tard.\n");
                            sleep(2); // Pause pour afficher le message
                            clear();
                            return; // Quitter la partie
                        } else {
                            printf("Action invalide! Reessayez.\n");
                        }

                        // Passer au prochain joueur
                        joueur_actuel = (joueur_actuel + 1) % nb_joueurs;  // Passer au joueur suivant
                        clear();
                        afficher_plateau_et_scores(&plateau, joueurs, nb_joueurs);  // Réafficher le plateau
                    }
                } else {
                    printf("Erreur lors du chargement de la partie. Essayez de nouveau.\n");
                }
                break;

            case 3: // Affiche l'aide quand l'utilisateur choisit 3 dans le menu
                afficher_aide();
                break;

            case 4: // Affiche les scores des joueurs precedents lorsque l'utilisateur choisit 4 dans le menu
                afficher_scores(joueurs, nb_joueurs);
                break;

            case 5: // Ferme le jeu quand l'utilisateur choisit 5 dans le menu
                printf("Merci d'avoir joue! A la prochaine.\n");
                break;

            default: // Affiche un message d'erreur blindant les choix disponibles
                printf("Choix invalide! Veuillez reessayer.\n");
                break;
        }

    } while (choix != 5);  // La boucle continue tant que le choix n'est pas 5 (quitter)
    clear();
}

