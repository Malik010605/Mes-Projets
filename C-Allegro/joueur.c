#include "joueur.h"
#include <stdio.h>
#include <string.h>
#include <allegro.h>

void demander_pseudo(Joueur *j) {
    int pos = 0;
    char c;
    clear_to_color(screen, makecol(0, 0, 0));
    textout_ex(screen, font, "Entrez votre pseudo :", 100, 200, makecol(255, 255, 255), -1);
    textout_ex(screen, font, "_", 100, 230, makecol(255, 255, 255), -1);

    while (!key[KEY_ENTER] && pos < MAX_PSEUDO - 1) {
        if (keypressed()) {
            c = readkey() & 0xff;

            if (c >= 32 && c <= 126) {
                j->pseudo[pos++] = c;
                j->pseudo[pos] = '\0';
            } else if (c == 8 && pos > 0) { // Backspace
                pos--;
                j->pseudo[pos] = '\0';
            }

            // Affichage en direct
            clear_to_color(screen, makecol(0, 0, 0));
            textout_ex(screen, font, "Entrez votre pseudo :", 100, 200, makecol(255, 255, 255), -1);
            textout_ex(screen, font, j->pseudo, 100, 230, makecol(255, 255, 255), -1);
            textout_ex(screen, font, "_", 100 + text_length(font, j->pseudo), 230, makecol(255, 255, 255), -1);
        }
    }
    rest(300);
}

void sauvegarder_partie_niveau3(Joueur *j, Personnage *perso, int scroll_x,
                                int bonus_vitesse_actif, int bonus_demulti_actif, int bonus_taille_actif, int malus_petit_actif,
                                int bonus_vitesse_en_cours, int bonus_demulti_en_cours, int bonus_taille_en_cours, int malus_petit_en_cours,
                                int debut_bonus_vitesse, int debut_bonus_demulti, int debut_bonus_taille, int debut_malus_petit,
                                double taille_facteur, int vivants[3], int nombre_oiseaux) {

    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", j->pseudo);
    FILE *f = fopen(nom_fichier, "w");

    if (!f) {
        allegro_message("Erreur ouverture fichier de sauvegarde.");
        return;
    }

    fprintf(f, "%s\n", j->pseudo);
    fprintf(f, "%d\n", j->niveau);
    fprintf(f, "%d\n", j->temps_jeu);
    fprintf(f, "%d %d\n", perso->x_monde, perso->y);
    fprintf(f, "%d %d %d %d\n", perso->vx, perso->vy, perso->current_frame, perso->saute);
    fprintf(f, "%d\n", scroll_x);

    // Nouveaux éléments pour niveau 2
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif);
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours);
    fprintf(f, "%d %d %d %d\n", debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit);
    fprintf(f, "%.2f\n", taille_facteur);
    fprintf(f, "%d %d %d\n", vivants[0], vivants[1], vivants[2]);
    fprintf(f, "%d\n", nombre_oiseaux);


    fprintf(f, "%d\n", perso->vitesse);

    fclose(f);
}


void sauvegarder_partie_niveau2(Joueur *j, Personnage *perso, int scroll_x,
                                int bonus_vitesse_actif, int bonus_demulti_actif, int bonus_taille_actif, int malus_petit_actif,
                                int bonus_vitesse_en_cours, int bonus_demulti_en_cours, int bonus_taille_en_cours, int malus_petit_en_cours,
                                int debut_bonus_vitesse, int debut_bonus_demulti, int debut_bonus_taille, int debut_malus_petit,
                                double taille_facteur, int vivants[3], int nombre_oiseaux) {

    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", j->pseudo);
    FILE *f = fopen(nom_fichier, "w");

    if (!f) {
        allegro_message("Erreur ouverture fichier de sauvegarde.");
        return;
    }

    fprintf(f, "%s\n", j->pseudo);
    fprintf(f, "%d\n", j->niveau);
    fprintf(f, "%d\n", j->temps_jeu);
    fprintf(f, "%d %d\n", perso->x_monde, perso->y);
    fprintf(f, "%d %d %d %d\n", perso->vx, perso->vy, perso->current_frame, perso->saute);
    fprintf(f, "%d\n", scroll_x);

    // Nouveaux éléments pour niveau 2
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif);
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours);
    fprintf(f, "%d %d %d %d\n", debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit);
    fprintf(f, "%.2f\n", taille_facteur);
    fprintf(f, "%d %d %d\n", vivants[0], vivants[1], vivants[2]);
    fprintf(f, "%d\n", nombre_oiseaux);


    fprintf(f, "%d\n", perso->vitesse);

    fclose(f);
}


void sauvegarder_partie_niveau1(Joueur *j, Personnage *perso, int scroll_x) {
    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", j->pseudo);
    FILE *f = fopen(nom_fichier, "w");

    if (!f) {
        allegro_message("Erreur ouverture fichier de sauvegarde (niveau 1)");
        return;
    }

    fprintf(f, "%s\n", j->pseudo);
    fprintf(f, "%d\n", j->niveau);
    fprintf(f, "%d\n", j->temps_jeu);
    fprintf(f, "%d %d\n", perso->x_monde, perso->y);
    fprintf(f, "%d %d %d %d\n", perso->vx, perso->vy, perso->current_frame, perso->saute);
    fprintf(f, "%d\n", scroll_x);

    fclose(f);
}





void sauvegarder_partie_niveau4(Joueur *j, Personnage *perso, int scroll_x,
                                int bonus_vitesse_actif, int bonus_demulti_actif, int bonus_taille_actif, int malus_petit_actif,
                                int bonus_vitesse_en_cours, int bonus_demulti_en_cours, int bonus_taille_en_cours, int malus_petit_en_cours,
                                int debut_bonus_vitesse, int debut_bonus_demulti, int debut_bonus_taille, int debut_malus_petit,
                                double taille_facteur, int vivants[3], int nombre_oiseaux) {

    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", j->pseudo);
    FILE *f = fopen(nom_fichier, "w");

    if (!f) {
        allegro_message("Erreur ouverture fichier de sauvegarde.");
        return;
    }

    fprintf(f, "%s\n", j->pseudo);
    fprintf(f, "%d\n", j->niveau);
    fprintf(f, "%d\n", j->temps_jeu);
    fprintf(f, "%d %d\n", perso->x_monde, perso->y);
    fprintf(f, "%d %d %d %d\n", perso->vx, perso->vy, perso->current_frame, perso->saute);
    fprintf(f, "%d\n", scroll_x);

    // Nouveaux éléments pour niveau 2
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif);
    fprintf(f, "%d %d %d %d\n", bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours);
    fprintf(f, "%d %d %d %d\n", debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit);
    fprintf(f, "%.2f\n", taille_facteur);
    fprintf(f, "%d %d %d\n", vivants[0], vivants[1], vivants[2]);
    fprintf(f, "%d\n", nombre_oiseaux);


    fprintf(f, "%d\n", perso->vitesse);

    fclose(f);
}


