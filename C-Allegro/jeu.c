#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "personnage.h"
#include "joueur.h"
#include "decor.h"
#include "jeu.h"

#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600




int lancer_jeu(Joueur *joueur) {
    // Charger l'image de fond du menu
    BITMAP *fond_menu = load_bitmap("fondmenubis.bmp", NULL);
    if (!fond_menu) {
        allegro_message("Erreur chargement fondmenubis.bmp");
        return 0;
    }

    demander_pseudo(joueur);

    while (1) {
        // Afficher l'image de fond
        blit(fond_menu, screen, 0, 0, 0, 0, fond_menu->w, fond_menu->h);

        // Afficher les choix de niveau par-dessus l'image
        textout_ex(screen, font, "Selectionnez un niveau :", 100, 150, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "1. Niveau 1 (Didacticiel)", 100, 180, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "2. Niveau 2 (Vitesse Bonus)", 100, 210, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "3. Niveau 3", 100, 240, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "4. Niveau 4", 100, 270, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "5. Reprendre niveau", 100, 300, makecol(255, 255, 255), -1);
        textout_ex(screen, font, "6. Quitter le jeu", 100, 330, makecol(255, 255, 255), -1);

        int choix = 0;

        while (1) {
            if (key[KEY_1]) { joueur->niveau = 1; choix = 1; break; }
            if (key[KEY_2]) { joueur->niveau = 2; choix = 2; break; }
            if (key[KEY_3]) { joueur->niveau = 3; choix = 3; break; }
            if (key[KEY_4]) { joueur->niveau = 4; choix = 4; break; }
            if (key[KEY_5]) { choix = 5; break; }
            if (key[KEY_6] || key[KEY_ESC]) {
                destroy_bitmap(fond_menu);
                return 0;
            }
        }

        rest(200);

        if (choix == 5) {
            char nom_fichier[100];
            sprintf(nom_fichier, "sauvegarde_%s.txt", joueur->pseudo);
            FILE *f = fopen(nom_fichier, "r");
            if (!f) {
                allegro_message("Aucune sauvegarde trouvée.");
                continue;
            }
            fscanf(f, "%*s"); // ignorer le pseudo
            fscanf(f, "%d", &joueur->niveau);
            fclose(f);

            if (joueur->niveau == 1) reprendre_niveau1(joueur);
            else if (joueur->niveau == 2) reprendre_niveau2(joueur);
            else if (joueur->niveau == 3) reprendre_niveau3(joueur);
            else if (joueur->niveau == 4) reprendre_niveau4(joueur);
            else allegro_message("Niveau non reconnu dans la sauvegarde.");
            continue;
        }

        int retour = jouer_niveau(joueur);
        if (retour == 0) continue;
        else {
            destroy_bitmap(fond_menu);
            return 0;
        }
    }

    // Libérer l'image de fond après utilisation (sécurité)
    destroy_bitmap(fond_menu);
}




int jouer_niveau(Joueur *joueur) {
    if (joueur->niveau == 1) {
        return niveau1(joueur);
    }
    if (joueur->niveau == 2) {
        return niveau2(joueur);
    }
    if (joueur->niveau == 3) {
        return niveau3(joueur);
    }
    if (joueur->niveau == 4) {
        return niveau4(joueur);
    }
    return 0;
}

void niveau_intro(int num_niveau) {
    const char* nom_fond;

    switch (num_niveau) {
        case 1: nom_fond = "fond.bmp"; break;
        case 2: nom_fond = "fondlave.bmp"; break;
        case 3: nom_fond = "fondespace.bmp"; break;
        case 4: nom_fond = "fond_nuit.bmp"; break;
    }

    BITMAP* intro_map = load_bitmap(nom_fond, NULL);
    if (!intro_map) {
        allegro_message("Erreur chargement %s", nom_fond);
        exit(EXIT_FAILURE);
    }

    BITMAP* page = create_bitmap(800, 600);
    if (!page) {
        allegro_message("Erreur creation page bitmap");
        exit(EXIT_FAILURE);
    }

    init_noirs_intro();

    Personnage p;
    switch (num_niveau) {
        case 1: init_personnage(&p); break;
        case 2: init_personnage2(&p); break;
        case 3: init_personnage3(&p); break;
        case 4: init_personnage4(&p); break;
        default: init_personnage(&p); break;
    }

    p.x_monde = 0;
    p.y = 300;
    p.current_frame = 1;
    p.saute = 1;

    int scroll_x = 0;
    double vitesse = 6.0;
    int compteur_frame = 0;
    int frame_anim = 1;
    int cible_x = 800;

    while (p.x_monde < cible_x) {
        p.x_monde += (int)vitesse;

        compteur_frame++;
        if (compteur_frame >= 15) {
            compteur_frame = 0;
            frame_anim = (frame_anim == 1) ? 2 : 1;
            p.current_frame = frame_anim;
        }

        blit(intro_map, page, scroll_x, 0, 0, 0, 800, 600);
        mettre_a_jour_noirs();
        afficher_noirs(page, scroll_x);
        afficher_personnage(&p, page, scroll_x);

        blit(page, screen, 0, 0, 0, 0, 800, 600);
        rest(20);
    }

    destroy_bitmap(page);
    destroy_bitmap(intro_map);
}





void animer_intro(Personnage *perso, BITMAP *fond, BITMAP *page, int num_niveau) {
    init_noirs_animation();
    if (num_niveau == 1 ) {
        init_piquants_animation();
    }
    perso->x_monde = 0;
    perso->y = 300;
    perso->current_frame = 1;
    perso->saute = 1;

    int compteur = 0, frame = 1;

    while (perso->x_monde < 300) {
        perso->x_monde += 3;

        compteur++;
        if (compteur >= 10) {
            compteur = 0;
            frame = (frame == 1) ? 2 : 1;
            perso->current_frame = frame;
        }
        mettre_a_jour_noirs();
        clear_bitmap(page);
        blit(fond, page, 0, 0, 0, 0, 800, 600);

        afficher_noirs(page, 0);

        afficher_personnage(perso, page, 0);
        blit(page, screen, 0, 0, 0, 0, 800, 600);
        rest(20);
    }

    // Attente de la touche espace avec message
    int frame_piquants = 0;

    while (!key[KEY_SPACE]) {
        mettre_a_jour_noirs();
        if (num_niveau == 1 ) {
            mettre_a_jour_piquants();
        }


        clear_bitmap(page);
        blit(fond, page, 0, 0, 0, 0, 800, 600);

        afficher_noirs(page, 0);

        // Affiche les piquants pendant 50 frames sur 100
        if (frame_piquants < 50) {
            if ((num_niveau == 1 ) && frame_piquants < 50) {
                afficher_piquants(page, 0);
            }
        }

        afficher_personnage(perso, page, 0);
        textout_ex(page, font, "Appuyez sur ESPACE pour commencer", 220, 450, makecol(255, 255, 255), -1);

        blit(page, screen, 0, 0, 0, 0, 800, 600);
        rest(20);

        frame_piquants = (frame_piquants + 1) % 100;  // boucle de 0 à 99

    }

    detruire_noirs();
    detruire_piquants();




    clear_keybuf();  // Évite un appui immédiat derrière
}



int niveau1(Joueur *joueur) {
    niveau_intro(1);  // animation de démarrage
    int rejouer = 1;

    while (rejouer) {
        BITMAP *page = create_bitmap(800, 600);
        BITMAP *fond = load_bitmap("fond.bmp", NULL);
        if (!page || !fond) {
            allegro_message("Erreur chargement fond/page");
            return 1;
        }

        Personnage perso;

        init_personnage(&perso);

        animer_intro(&perso, fond, page, 1);
        init_noirs();
        init_scies();
        init_piquants();
        init_stalactites();
        init_diagonales();
        init_pics();



        int scroll_x = 0;

        int timer = 56500;
        int debut = clock();


        int collision = 0;
        int fin_partie = 0;
        int temps_pause_total = 0;

        while (!key[KEY_ESC] && !fin_partie) {
            scroll_x += 2;
            if (scroll_x >= fond->w) scroll_x = 0;

            clear_bitmap(page);
            // Scroll horizontal du fond
            blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
            blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

            // Affichage obstacles
            afficher_scies(page, scroll_x);
            mettre_a_jour_piquants();
            afficher_piquants(page, scroll_x);
            afficher_stalactites(page, scroll_x);
            afficher_personnage(&perso, page, scroll_x);
            afficher_diagonales(page, scroll_x);
            mettre_a_jour_diagonales();
            mettre_a_jour_pics();
            afficher_pics(page, scroll_x);
            afficher_noirs(page, scroll_x);
            mettre_a_jour_noirs();




            mettre_a_jour_scies();
            mettre_a_jour_stalactites();

            // Mouvements personnage
            deplacer_personnage(&perso);
            animer_personnage(&perso);
            afficher_personnage(&perso, page, scroll_x);

            // Pause si la touche P est appuyée
            if (key[KEY_P]) {
                int debut_pause = clock();  // mémorise le début de la pause

                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
                textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        temps_pause_total += clock() - debut_pause;  // ajoute durée de pause
                        break;
                    }
                    if (key[KEY_2]) {
                        clear_keybuf();
                        temps_pause_total += clock() - debut_pause;
                        joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);

                        sauvegarder_partie_niveau1(joueur, &perso, scroll_x);
                        // destruction

                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_noirs();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_portails();
                        detruire_portails2();
                        detruire_laves();
                        detruire_gris();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        rest(200);
                        return 0;
                    }

                }
            }

// Collision pixel noir
            int x_ecran = perso.x_monde - scroll_x;
            int x_collision = x_ecran;  // Coordonnées exactes de la collision
            int y_collision = perso.y;
            if (collision_pixel_noir(page, x_ecran, perso.y, perso.image[0]->w / 3)) {
                collision = 1;
                x_collision = x_ecran;
                y_collision = perso.y;
            }


// Si collision → explosion + chute + Game Over
            if (collision) {
                // Charger le personnage squelette
                BITMAP *squelette = load_bitmap("squelette.bmp", NULL);
                if (!squelette) {
                    allegro_message("Erreur : impossible de charger squelette.bmp");
                    return 5;
                }

                // --- Animation de l'explosion ---
                BITMAP *explosion[5];
                for (int i = 0; i < 5; i++) {
                    char nom_explosion[20];
                    sprintf(nom_explosion, "explo%d.bmp", i);
                    explosion[i] = load_bitmap(nom_explosion, NULL);
                }

                // Affichage de l'explosion (5 images)
                for (int i = 0; i < 5; i++) {
                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_personnage(&perso, page, scroll_x);

                    // Afficher l'explosion à l'endroit de la collision
                    if (explosion[i]) {
                        draw_sprite(page, explosion[i], x_collision, y_collision);
                    }

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(100);
                }

                // Libération des bitmaps de l'explosion
                for (int i = 0; i < 5; i++) {
                    if (explosion[i]) {
                        destroy_bitmap(explosion[i]);
                    }
                }

                // --- Animation de chute du squelette ---
                while (y_collision < 580) {
                    y_collision += 5;  // Chute accélérée
                    clear_bitmap(page);

                    // Afficher l'arrière-plan
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w, 600);

                    // Afficher les autres éléments
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);

                    // Afficher le squelette à la place du personnage
                    draw_sprite(page, squelette, x_collision, y_collision);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Libérer le bitmap squelette
                destroy_bitmap(squelette);

                // --- Écran de Game Over ---
                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 260, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Retour menu", 300, 290, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);

                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_noirs();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_portails();
                        detruire_portails2();
                        detruire_laves();
                        detruire_gris();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                }
                break;
            }


            // Fin normale (timer atteint)
            if (clock() - debut - temps_pause_total >= timer) {
                int cible_y = 300;
                double vitesse_y = 5.0;
                int compteur_frame = 0;
                int etat_anim = 1;

                perso.current_frame = 1;



                // Phase 1 : mouvement vertical vers le centre
                while (perso.y != cible_y) {
                    if (perso.y < cible_y) {
                        perso.y += (int)vitesse_y;
                        if (perso.y > cible_y) perso.y = cible_y;
                    } else if (perso.y > cible_y) {
                        perso.y -= (int)vitesse_y;
                        if (perso.y < cible_y) perso.y = cible_y;
                    }

                    compteur_frame++;
                    if (compteur_frame >= 15) {  // Change de frame toutes les 5 itérations
                        compteur_frame = 0;
                        etat_anim = (etat_anim == 1) ? 2 : 1;  // alterne entre frame 1 et 2
                        perso.current_frame = etat_anim;
                    }


                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);


                    afficher_personnage(&perso, page, scroll_x);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Phase 2 : déplacement horizontal accéléré
                int cible_x = perso.x_monde + 800;
                double vitesse_anim = 1.0;
                clock_t last_accel_time = clock();
                perso.saute = 1;


                while (perso.x_monde < cible_x) {
                    if ((clock() - last_accel_time) >= 100) {
                        vitesse_anim += 0.8 ;
                        last_accel_time = clock();
                    }

                    compteur_frame++;
                    if (compteur_frame >= 12) {  // Change de frame toutes les 5 itérations
                        compteur_frame = 0;
                        etat_anim = (etat_anim == 1) ? 2 : 1;  // alterne entre frame 1 et 2
                        perso.current_frame = etat_anim;
                    }


                    perso.x_monde += (int)vitesse_anim;

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);


                    afficher_personnage(&perso, page, scroll_x);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Message de fin
                clear_to_color(page, makecol(0, 255, 0));
                textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
                textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
                textout_ex(page, font, "3. Passer au Niveau 2", 300, 290, makecol(0, 0, 0), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);


                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_noirs();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_portails();
                        detruire_portails2();
                        detruire_laves();
                        detruire_gris();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                    if (key[KEY_3]) {
                        rest(200);
                        // Passer directement au Niveau 2

                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_noirs();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_portails();
                        detruire_portails2();
                        detruire_laves();
                        detruire_gris();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);

                        // Charger Niveau 2 directement
                        return niveau2(joueur); // Assure-toi que la fonction niveau2() est définie
                    }
                }
                break;
            }

            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(20);
        }


        detruire_scies();
        detruire_piquants();
        detruire_stalactites();
        detruire_noirs();
        detruire_diagonales();
        detruire_pics();
        detruire_portails();
        detruire_portails2();
        detruire_laves();
        detruire_gris();
        detruire_canons(canons);

        destroy_bitmap(page);
        destroy_bitmap(fond);
    }

    return 0;
}

int niveau2(Joueur *joueur) {


    niveau_intro(2);

    int rejouer = 1;

    while (rejouer) {
        BITMAP *page = create_bitmap(800, 600);
        BITMAP *fond = load_bitmap("fondlave.bmp", NULL);
        BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
        BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
        BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
        BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);
        if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
            allegro_message("Erreur chargement bitmaps");
            return 1;
        }

        Personnage perso2;
        init_personnage2(&perso2);
        animer_intro(&perso2, fond, page, 2);



        init_scies2();
        init_piquants2();
        init_stalactites2();
        init_diagonales2();
        init_pics2();
        init_noirs2();
        init_laves();
        init_portails2();



        int scroll_x = 0;
        int timer = 56500;
        int debut = clock();
        int collision = 0;
        int fin_partie = 0;
        int temps_pause_total = 0;

        // Positions des bonus
        int x_bonus_vitesse = 9000, y_bonus_vitesse = 340;
        int x_bonus_demulti = 12850, y_bonus_demulti = 340;
        int x_bonus_taille = 16000, y_bonus_taille = 340;
        int x_malus_petit = 19000, y_malus_petit = 340;

        // États des bonus/malus
        int bonus_vitesse_actif = 1, bonus_demulti_actif = 1, bonus_taille_actif = 1, malus_petit_actif = 1;
        int bonus_vitesse_en_cours = 0, bonus_demulti_en_cours = 0, bonus_taille_en_cours = 0, malus_petit_en_cours = 0;
        int debut_bonus_vitesse = 0, debut_bonus_demulti = 0, debut_bonus_taille = 0, debut_malus_petit = 0;
        const int duree_bonus = 10000;

        double taille_facteur = 1.0;
        int nombre_oiseaux = 1;
        int vivants[3] = {1, 0, 0};

        int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
        int pause_bonus_taille = 0, pause_malus_petit = 0;


        while (!key[KEY_ESC] && !fin_partie) {
            scroll_x += 2 * perso2.vitesse;
            if (scroll_x >= fond->w) scroll_x = 0;

            clear_bitmap(page);
            blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
            blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

            afficher_scies(page, scroll_x);
            mettre_a_jour_piquants();
            afficher_piquants(page, scroll_x);
            afficher_stalactites(page, scroll_x);
            afficher_diagonales(page, scroll_x);
            mettre_a_jour_diagonales();
            mettre_a_jour_pics();
            afficher_pics(page, scroll_x);
            afficher_noirs(page, scroll_x);
            mettre_a_jour_noirs();
            mettre_a_jour_scies();
            mettre_a_jour_stalactites();
            mettre_a_jour_laves();
            afficher_laves(page, scroll_x);
            afficher_portails2(page, scroll_x);
            mettre_a_jour_portails2();


            // BONUS VITESSE
            int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
            if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
                stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
                if (perso2.x_monde >= x_bonus_vitesse && perso2.x_monde <= x_bonus_vitesse + 50 &&
                    perso2.y >= y_bonus_vitesse && perso2.y <= y_bonus_vitesse + 50) {
                    bonus_vitesse_actif = 0;
                    perso2.vitesse *= 2;
                    perso2.vx *= 2;

                    bonus_vitesse_en_cours = 1;
                    debut_bonus_vitesse = clock();
                }
            }

            // BONUS DEMULTI
            int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
            if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
                stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
                if (perso2.x_monde >= x_bonus_demulti && perso2.x_monde <= x_bonus_demulti + 50 &&
                    perso2.y >= y_bonus_demulti && perso2.y <= y_bonus_demulti + 50)
                {
                    bonus_demulti_actif = 0;
                    nombre_oiseaux = 3;
                    vivants[0] = vivants[1] = vivants[2] = 1;
                    bonus_demulti_en_cours = 1;
                    debut_bonus_demulti = clock();
                }
            }

            // BONUS TAILLE
            int bonus_taille_ecran = x_bonus_taille - scroll_x;
            if (bonus_taille_actif && bonus_taille_ecran >= -50 && bonus_taille_ecran <= 850) {
                stretch_sprite(page, bonus_taille, bonus_taille_ecran, y_bonus_taille, 50, 50);
                if (perso2.x_monde >= x_bonus_taille && perso2.x_monde <= x_bonus_taille + 50 &&
                    perso2.y >= y_bonus_taille && perso2.y <= y_bonus_taille + 50) {
                    bonus_taille_actif = 0;
                    taille_facteur = 2.0;
                    bonus_taille_en_cours = 1;
                    debut_bonus_taille = clock();
                    perso2.taille_facteur = 2.0;


                }
            }

            // MALUS PETIT
            int malus_petit_ecran = x_malus_petit - scroll_x;
            if (malus_petit_actif && malus_petit_ecran >= -50 && malus_petit_ecran <= 850) {
                stretch_sprite(page, malus_petit, malus_petit_ecran, y_malus_petit, 50, 50);
                if (perso2.x_monde >= x_malus_petit && perso2.x_monde <= x_malus_petit + 50 &&
                    perso2.y >= y_malus_petit && perso2.y <= y_malus_petit + 50){
                    malus_petit_actif = 0;
                    taille_facteur = 0.5;

                    malus_petit_en_cours = 1;
                    debut_malus_petit = clock();
                    perso2.taille_facteur = 0.5;
                }
            }

            // Expiration des effets
            if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
                perso2.vitesse /= 2;
                perso2.vx /= 2;
                bonus_vitesse_en_cours = 0;
            }
            if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
                bonus_demulti_en_cours = 0;
                nombre_oiseaux = 1;
                vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
            }
            if (bonus_taille_en_cours && clock() - debut_bonus_taille - pause_bonus_taille >= duree_bonus) {
                bonus_taille_en_cours = 0;
                taille_facteur = 1.0;
                perso2.taille_facteur = 1.0;
            }
            if (malus_petit_en_cours && clock() - debut_malus_petit - pause_malus_petit >= duree_bonus) {
                malus_petit_en_cours = 0;
                taille_facteur = 1.0;
                perso2.taille_facteur = 1.0;
            }


            deplacer_personnage(&perso2);
            animer_personnage(&perso2);
            int old_x = perso2.x_monde, old_y = perso2.y;

            if (nombre_oiseaux == 1) {
                int x_ecran = perso2.x_monde - scroll_x;

                if (taille_facteur == 1.0)
                    afficher_personnage(&perso2, page, scroll_x);
                else
                    stretch_sprite(page, perso2.image[perso2.current_frame],
                                   (int)(x_ecran - perso2.image[0]->w * taille_facteur / 2),
                                   (int)(perso2.y - perso2.image[0]->h * taille_facteur / 2),
                                   (int)(perso2.image[0]->w * taille_facteur),
                                   (int)(perso2.image[0]->h * taille_facteur));

            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    if (i == 0) { perso2.x_monde = old_x; perso2.y = old_y; }
                    if (i == 1) { perso2.x_monde = old_x + 50; perso2.y = old_y - 50; }
                    if (i == 2) { perso2.x_monde = old_x + 50; perso2.y = old_y + 50; }

                    int x_ecran = perso2.x_monde - scroll_x;

                    if (taille_facteur == 1.0) {
                        afficher_personnage(&perso2, page, scroll_x);
                    } else {
                        stretch_sprite(page, perso2.image[perso2.current_frame],
                                       (int)(x_ecran - perso2.image[0]->w * taille_facteur / 2),
                                       (int)(perso2.y - perso2.image[0]->h * taille_facteur / 2),
                                       (int)(perso2.image[0]->w * taille_facteur),
                                       (int)(perso2.image[0]->h * taille_facteur));
                    }

                }
                perso2.x_monde = old_x;
                perso2.y = old_y;
            }

            // --- COLLISIONS ---
            int rayon_collision = (int)(perso2.image[0]->w * taille_facteur / 3);

            if (nombre_oiseaux == 1) {
                if (collision_pixel_noir(page, perso2.x_monde, perso2.y, rayon_collision)) {
                    collision = 1;
                }



            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    int cx = old_x, cy = old_y;
                    if (i == 1) { cx += 30; cy -= 30; }
                    if (i == 2) { cx += 30; cy += 30; }
                    if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                        vivants[i] = 0;
                    }
                }

                int vivants_restants = 0;
                for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
                nombre_oiseaux = vivants_restants;

                if (nombre_oiseaux == 0) collision = 1;
            }

            // --- JAUGES ---
            int xj = 570, yj = 20, hj = 20, lj = 200;
            if (bonus_vitesse_en_cours) {
                int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
                yj += 25;
            }
            if (bonus_demulti_en_cours) {
                int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
                yj += 25;
            }
            if (bonus_taille_en_cours) {
                int t = clock() - debut_bonus_taille - pause_bonus_taille;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
                yj += 25;
            }
            if (malus_petit_en_cours) {
                int t = clock() - debut_malus_petit - pause_malus_petit;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
                yj += 25;
            }

            // Pause si la touche P est appuyée
            if (key[KEY_P]) {
                int debut_pause = clock();

                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
                textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        break;
                    }
                    if (key[KEY_2]) {
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                        if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                        if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                        if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                        temps_pause_total += clock() - debut_pause;
                        joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                        sauvegarder_partie_niveau2(joueur, &perso2, scroll_x,
                                                   bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                                   bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                                   debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                                   taille_facteur, vivants, nombre_oiseaux);
                        clear_keybuf();
                        // destruction des objets graphiques
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        detruire_laves();
                        detruire_portails2();


                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        rest(200);
                        return 0;
                    }
                }
            }

            // Collision pixel noir
            int x_ecran = perso2.x_monde - scroll_x;
            int x_collision = x_ecran;  // Coordonnées exactes de la collision
            int y_collision = perso2.y;
            // Rayon de collision simplifié (petit rectangle autour du personnage)
            int largeur_collision = perso2.image[0]->w / 3;
            int hauteur_collision = perso2.image[0]->h;


            // Collision pixel noir (zone restreinte autour du personnage)
            collision = 0; // Reset de la variable collision

            for (int dx = 0; dx < largeur_collision; dx++) {
                for (int dy = 0; dy < hauteur_collision; dy++) {
                    if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                        collision = 1;
                        x_collision += dx;
                        y_collision += dy;
                        break;
                    }
                }
                if (collision) break;
            }


// Si collision → explosion + chute + Game Over
            if (collision) {
                // Charger le personnage squelette
                BITMAP *squelette = load_bitmap("squelette.bmp", NULL);
                if (!squelette) {
                    allegro_message("Erreur : impossible de charger squelette.bmp");
                    return 3;
                }

                // --- Animation de l'explosion ---
                BITMAP *explosion[5];
                for (int i = 0; i < 5; i++) {
                    char nom_explosion[20];
                    sprintf(nom_explosion, "explo%d.bmp", i);
                    explosion[i] = load_bitmap(nom_explosion, NULL);
                }

                // Affichage de l'explosion (5 images)
                for (int i = 0; i < 5; i++) {
                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_personnage(&perso2, page, scroll_x);

                    // Afficher l'explosion à l'endroit de la collision
                    if (explosion[i]) {
                        draw_sprite(page, explosion[i], x_collision, y_collision);
                    }

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(100);
                }

                // Libération des bitmaps de l'explosion
                for (int i = 0; i < 5; i++) {
                    if (explosion[i]) {
                        destroy_bitmap(explosion[i]);
                    }
                }

                // --- Animation de chute du squelette ---
                while (y_collision < 580) {
                    y_collision += 5;  // Chute accélérée
                    clear_bitmap(page);

                    // Afficher l'arrière-plan
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w, 600);

                    // Afficher les autres éléments
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);

                    // Afficher le squelette à la place du personnage
                    draw_sprite(page, squelette, x_collision, y_collision);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Libérer le bitmap squelette
                destroy_bitmap(squelette);
                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 260, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Retour menu", 300, 290, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                while (1) {
                    if (key[KEY_1]) { rest(200); fin_partie = 1; rejouer = 1; break; }

                }
                break;
            }


            if (perso2.x_monde >= 1780 && perso2.x_monde <= 1820 && perso2.y >= 100 && perso2.y <= 275) {

                // Animation de secousse
                for (int i = 0; i < 10; i++) {  // 10 secousses rapides
                    clear_bitmap(page);

                    // Valeurs de secousse aléatoire ou alternée
                    int dx = (i % 2 == 0) ? 5 : -5;
                    int dy = (i % 2 == 0) ? 5 : -5;

                    // Affichage du fond avec décalage
                    blit(fond, page, scroll_x, 0, dx, dy, 800, 600);

                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(120);  // durée de chaque frame de secousse
                }

                for (int fondu = 0; fondu < 14; fondu++) {
                    clear_bitmap(page);

                    // Fond avec scroll + éléments
                    blit(fond, page, scroll_x, 0, 0, 0, 800, 600);
                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    // Appliquer fondu coloré (du violet foncé vers rouge intense)
                    int alpha = fondu * 20 + 30;  // Alpha va de 60 à 240
                    if (alpha > 255) alpha = 255;

                    set_trans_blender(0, 0, 0, alpha);  // alpha-blending progressif
                    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                    rectfill(page, 0, 0, 800, 600, makecol(120, 0, 60)); // violet foncé
                    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0); // réinitialiser

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(80);
                }
                perso2.x_monde = 2160;
                perso2.y = 350;
                scroll_x += 360;

            }


            if (perso2.x_monde >= 3400 && perso2.x_monde <= 3550 && perso2.y >= 400 && perso2.y <= 550) {

                // Animation de secousse
                for (int i = 0; i < 10; i++) {  // 10 secousses rapides
                    clear_bitmap(page);

                    // Valeurs de secousse aléatoire ou alternée
                    int dx = (i % 2 == 0) ? 5 : -5;
                    int dy = (i % 2 == 0) ? 5 : -5;

                    // Affichage du fond avec décalage
                    blit(fond, page, scroll_x, 0, dx, dy, 800, 600);

                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(120);  // durée de chaque frame de secousse
                }

                for (int fondu = 0; fondu < 14; fondu++) {
                    clear_bitmap(page);

                    // Fond avec scroll + éléments
                    blit(fond, page, scroll_x, 0, 0, 0, 800, 600);
                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    // Appliquer fondu coloré (du violet foncé vers rouge intense)
                    int alpha = fondu * 20 + 30;  // Alpha va de 60 à 240
                    if (alpha > 255) alpha = 255;

                    set_trans_blender(0, 0, 0, alpha);  // alpha-blending progressif
                    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                    rectfill(page, 0, 0, 800, 600, makecol(120, 0, 60)); // violet foncé
                    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0); // réinitialiser

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(80);
                }
                perso2.x_monde = 3680;
                perso2.y = 330;
                scroll_x += 150;

            }

            int cible_x = 3915;


            // Nouvelle condition pour déclencher l'animation de fin
            if (perso2.x_monde >= cible_x) {
                int cible_y = 300;
                double vitesse_y = 5.0;
                int compteur_frame = 0;

                perso2.current_frame = 0;

                // Phase 1 : mouvement vertical vers le centre
                while (perso2.y != cible_y) {
                    if (perso2.y < cible_y) {
                        perso2.y += (int)vitesse_y;
                        if (perso2.y > cible_y) perso2.y = cible_y;
                    } else if (perso2.y > cible_y) {
                        perso2.y -= (int)vitesse_y;
                        if (perso2.y < cible_y) perso2.y = cible_y;
                    }

                    compteur_frame++;
                    if (compteur_frame >= 25) {
                        perso2.current_frame = (perso2.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    blit(fond, page, scroll_x, 0, 0, 0, 800, 600);
                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    afficher_personnage(&perso2, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Phase 2 : déplacement horizontal accéléré
                int cible_x_fin = perso2.x_monde + 800;
                double vitesse_anim = 1.0;
                clock_t last_accel_time = clock();
                perso2.saute = 1;

                while (perso2.x_monde < cible_x_fin) {
                    if ((clock() - last_accel_time) >= 100) {
                        vitesse_anim += 0.8 ;
                        last_accel_time = clock();
                    }

                    compteur_frame++;
                    if (compteur_frame >= 13) {
                        perso2.current_frame = (perso2.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    perso2.x_monde += (int)vitesse_anim;

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    blit(fond, page, scroll_x, 0, 0, 0, 800, 600);
                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    mettre_a_jour_laves();
                    afficher_laves(page, scroll_x);
                    afficher_portails2(page, scroll_x);
                    mettre_a_jour_portails2();

                    afficher_personnage(&perso2, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Message de fin
                clear_to_color(page, makecol(0, 255, 0));
                textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
                textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
                textout_ex(page, font, "3. Passer au Niveau 3", 300, 290, makecol(0, 0, 0), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        detruire_portails2();
                        detruire_laves();

                        return 0;
                    }
                    if (key[KEY_3]) {
                        rest(200);
                        // Passer directement au Niveau 2
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        detruire_laves();
                        detruire_portails2();

                        destroy_bitmap(page);
                        destroy_bitmap(fond);

                        // Charger Niveau 3 directement
                        return niveau3(joueur);
                    }
                }
                break;
            }

            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(20);
        }

        nettoyer:
        detruire_scies();
        detruire_piquants();
        detruire_stalactites();
        detruire_diagonales();
        detruire_pics();
        detruire_noirs();
        detruire_laves();
        detruire_portails2();

        destroy_bitmap(page);
        destroy_bitmap(fond);
        destroy_bitmap(bonus_vitesse);
        destroy_bitmap(bonus_demulti);
        destroy_bitmap(bonus_taille);
        destroy_bitmap(malus_petit);
    }

    return 0;
}
int reprendre_niveau1(Joueur *joueur) {
    int rejouer = 1;

    // Lecture de la sauvegarde
    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", joueur->pseudo);
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        allegro_message("Aucune partie jouée.");
        return 1; // On retourne simplement au menu
    }

    if (!f) {
        allegro_message("Impossible de lire le fichier de sauvegarde.");
        return 1;
    }

    fscanf(f, "%s", joueur->pseudo);
    fscanf(f, "%d", &joueur->niveau);
    fscanf(f, "%d", &joueur->temps_jeu);


    Personnage perso;
    fscanf(f, "%d %d", &perso.x_monde, &perso.y);
    fscanf(f, "%d %d %d %d", &perso.vx, &perso.vy, &perso.current_frame, &perso.saute);
    int scroll_x;
    fscanf(f, "%d", &scroll_x);
    fclose(f);

    for (int i = 0; i < 4; i++) {
        char nom_fichier[50];
        sprintf(nom_fichier, "perso%dbis.bmp", i);
        perso.image[i] = load_bitmap(nom_fichier, NULL);
        if (!perso.image[i]) {
            allegro_message("Erreur chargement image personnage");
            return 1;
        }
    }
    perso.vitesse = 1;
    perso.compteur_frame = 0;
    perso.delai_frame = 5;

    while (rejouer) {
        BITMAP *page = create_bitmap(800, 600);
        BITMAP *fond = load_bitmap("fond.bmp", NULL);
        if (!page || !fond) {
            allegro_message("Erreur chargement fond/page");
            return 1;
        }

        init_scies();
        init_piquants();
        init_stalactites();
        init_diagonales();
        init_pics();
        init_noirs();

        int timer = joueur->temps_jeu;

        int debut = clock();
        int collision = 0;
        int fin_partie = 0;
        int temps_pause_total = 0;

        while (!key[KEY_ESC] && !fin_partie) {
            scroll_x += 2;
            if (scroll_x >= fond->w) scroll_x = 0;

            clear_bitmap(page);
            blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
            blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

            afficher_scies(page, scroll_x);
            mettre_a_jour_piquants();
            afficher_piquants(page, scroll_x);
            afficher_stalactites(page, scroll_x);
            afficher_personnage(&perso, page, scroll_x);
            afficher_diagonales(page, scroll_x);
            mettre_a_jour_diagonales();
            mettre_a_jour_pics();
            afficher_pics(page, scroll_x);
            afficher_noirs(page, scroll_x);
            mettre_a_jour_noirs();
            mettre_a_jour_scies();
            mettre_a_jour_stalactites();

            deplacer_personnage(&perso);
            animer_personnage(&perso);
            afficher_personnage(&perso, page, scroll_x);

            if (key[KEY_P]) {
                int debut_pause = clock();
                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
                textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        temps_pause_total += clock() - debut_pause;
                        break;
                    }
                    if (key[KEY_2]) {
                        clear_keybuf();
                        temps_pause_total += clock() - debut_pause;
                        joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);

                        sauvegarder_partie_niveau1(joueur, &perso, scroll_x);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        rest(200);
                        return 0;
                    }
                }
            }

            if (collision_pixel_noir(page, perso.x_monde, perso.y, perso.image[0]->w / 3)) {
                collision = 1;
            }

            if (collision) {
                while (perso.y < 580) {
                    perso.vy += 1;
                    perso.y += perso.vy;

                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_personnage(&perso, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 260, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Retour menu", 300, 290, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                }
                break;
            }

            int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


// Nouvelle condition pour déclencher l'animation de fin
            if (perso.x_monde >= cible_x) {
                int cible_y = 300;
                double vitesse_y = 5.0;
                int compteur_frame = 0;

                perso.current_frame = 0;

                // Phase 1 : mouvement vertical vers le centre
                while (perso.y != cible_y) {
                    if (perso.y < cible_y) {
                        perso.y += (int)vitesse_y;
                        if (perso.y > cible_y) perso.y = cible_y;
                    } else if (perso.y > cible_y) {
                        perso.y -= (int)vitesse_y;
                        if (perso.y < cible_y) perso.y = cible_y;
                    }

                    compteur_frame++;
                    if (compteur_frame >= 25) {
                        perso.current_frame = (perso.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);

                    afficher_personnage(&perso, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Phase 2 : déplacement horizontal accéléré
                int cible_x_fin = perso.x_monde + 800;
                double vitesse_anim = 1.0;
                clock_t last_accel_time = clock();
                perso.saute = 1;

                while (perso.x_monde < cible_x_fin) {
                    if ((clock() - last_accel_time) >= 100) {
                        vitesse_anim += 0.8 ;
                        last_accel_time = clock();
                    }

                    compteur_frame++;
                    if (compteur_frame >= 13) {
                        perso.current_frame = (perso.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    perso.x_monde += (int)vitesse_anim;

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);

                    afficher_personnage(&perso, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Message de fin
                clear_to_color(page, makecol(0, 255, 0));
                textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
                textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);


                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                }
                break;
            }

            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(20);
        }

        detruire_scies();
        detruire_piquants();
        detruire_stalactites();
        detruire_diagonales();
        detruire_pics();
        detruire_noirs();
        destroy_bitmap(page);
        destroy_bitmap(fond);
    }

    return 0;
}



int reprendre_niveau2(Joueur *joueur) {
    int rejouer = 1;


    // Lecture de la sauvegarde
    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", joueur->pseudo);
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        allegro_message("Aucune partie jouée.");
        return 1;
    }

    fscanf(f, "%s", joueur->pseudo);
    fscanf(f, "%d", &joueur->niveau);
    fscanf(f, "%d", &joueur->temps_jeu);

    Personnage perso2;
    init_personnage2(&perso2);
    fscanf(f, "%d %d", &perso2.x_monde, &perso2.y);
    fscanf(f, "%d %d %d %d", &perso2.vx, &perso2.vy, &perso2.current_frame, &perso2.saute);
    int scroll_x;
    fscanf(f, "%d", &scroll_x);

    int bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif;
    int bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours;
    int debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit;
    double taille_facteur;
    int vivants[3];
    int nombre_oiseaux;



    fscanf(f, "%d %d %d %d", &bonus_vitesse_actif, &bonus_demulti_actif, &bonus_taille_actif, &malus_petit_actif);
    fscanf(f, "%d %d %d %d", &bonus_vitesse_en_cours, &bonus_demulti_en_cours, &bonus_taille_en_cours, &malus_petit_en_cours);
    fscanf(f, "%d %d %d %d", &debut_bonus_vitesse, &debut_bonus_demulti, &debut_bonus_taille, &debut_malus_petit);
    fscanf(f, "%lf", &taille_facteur);
    fscanf(f, "%d %d %d", &vivants[0], &vivants[1], &vivants[2]);

    fscanf(f, "%d", &nombre_oiseaux);

    fscanf(f, "%d", &perso2.vitesse);

    fclose(f);






    perso2.compteur_frame = 0;
    perso2.delai_frame = 5;

    BITMAP *page = create_bitmap(800, 600);
    BITMAP *fond = load_bitmap("fondlave.bmp", NULL);
    BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
    BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
    BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
    BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);
    if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
        allegro_message("Erreur chargement bitmaps");
        return 1;
    }

    init_scies2();
    init_piquants2();
    init_stalactites2();
    init_diagonales2();
    init_pics2();
    init_noirs2();

    int timer = joueur->temps_jeu;
    int x_bonus_vitesse = 900, y_bonus_vitesse = 340;
    int x_bonus_demulti = 1285, y_bonus_demulti = 340;
    int x_bonus_taille = 1600, y_bonus_taille = 340;
    int x_malus_petit = 1900, y_malus_petit = 340;

    int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
    int pause_bonus_taille = 0, pause_malus_petit = 0;

    int debut = clock();


    if (bonus_vitesse_en_cours) debut_bonus_vitesse = debut;
    if (bonus_demulti_en_cours) debut_bonus_demulti = debut;
    if (bonus_taille_en_cours)  debut_bonus_taille = debut;
    if (malus_petit_en_cours)   debut_malus_petit = debut;

    pause_bonus_vitesse = 0;
    pause_bonus_demulti = 0;
    pause_bonus_taille = 0;
    pause_malus_petit = 0;


    int collision = 0;
    int fin_partie = 0;
    int temps_pause_total = 0;
    const int duree_bonus = 10000;



    while (!key[KEY_ESC] && !fin_partie) {
        scroll_x += 2 * perso2.vitesse;
        if (scroll_x >= fond->w) scroll_x = 0;

        clear_bitmap(page);
        blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
        blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

        afficher_scies(page, scroll_x);
        mettre_a_jour_piquants();
        afficher_piquants(page, scroll_x);
        afficher_stalactites(page, scroll_x);
        afficher_diagonales(page, scroll_x);
        mettre_a_jour_diagonales();
        mettre_a_jour_pics();
        afficher_pics(page, scroll_x);
        afficher_noirs(page, scroll_x);
        mettre_a_jour_noirs();
        mettre_a_jour_scies();
        mettre_a_jour_stalactites();

        // BONUS VITESSE
        int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
        if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
            stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
            if (perso2.x_monde >= x_bonus_vitesse && perso2.x_monde <= x_bonus_vitesse + 50 &&
                perso2.y >= y_bonus_vitesse && perso2.y <= y_bonus_vitesse + 50) {
                bonus_vitesse_actif = 0;
                perso2.vitesse *= 2;
                perso2.vx *= 2;
                bonus_vitesse_en_cours = 1;
                debut_bonus_vitesse = clock();
            }
        }

// BONUS DEMULTI
        int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
        if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
            stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
            if (perso2.x_monde >= x_bonus_demulti && perso2.x_monde <= x_bonus_demulti + 50 &&
                perso2.y >= y_bonus_demulti && perso2.y <= y_bonus_demulti + 50) {
                bonus_demulti_actif = 0;
                nombre_oiseaux = 3;
                vivants[0] = vivants[1] = vivants[2] = 1;
                bonus_demulti_en_cours = 1;
                debut_bonus_demulti = clock();
            }
        }

// BONUS TAILLE
        int bonus_taille_ecran = x_bonus_taille - scroll_x;
        if (bonus_taille_actif && bonus_taille_ecran >= -50 && bonus_taille_ecran <= 850) {
            stretch_sprite(page, bonus_taille, bonus_taille_ecran, y_bonus_taille, 50, 50);
            if (perso2.x_monde >= x_bonus_taille && perso2.x_monde <= x_bonus_taille + 50 &&
                perso2.y >= y_bonus_taille && perso2.y <= y_bonus_taille + 50) {
                bonus_taille_actif = 0;
                taille_facteur = 2.0;
                bonus_taille_en_cours = 1;
                debut_bonus_taille = clock();
                perso2.taille_facteur = 2.0;

            }
        }

// MALUS PETIT
        int malus_petit_ecran = x_malus_petit - scroll_x;
        if (malus_petit_actif && malus_petit_ecran >= -50 && malus_petit_ecran <= 850) {
            stretch_sprite(page, malus_petit, malus_petit_ecran, y_malus_petit, 50, 50);
            if (perso2.x_monde >= x_malus_petit && perso2.x_monde <= x_malus_petit + 50 &&
                perso2.y >= y_malus_petit && perso2.y <= y_malus_petit + 50) {
                malus_petit_actif = 0;
                taille_facteur = 0.5;
                malus_petit_en_cours = 1;
                debut_malus_petit = clock();
            }
        }

        // Expiration des effets bonus/malus
        if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
            perso2.vitesse /= 2;
            perso2.vx /= 2;
            bonus_vitesse_en_cours = 0;
        }
        if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
            bonus_demulti_en_cours = 0;
            nombre_oiseaux = 1;
            vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
        }
        if (bonus_taille_en_cours && clock() - debut_bonus_taille - pause_bonus_taille >= duree_bonus) {
            bonus_taille_en_cours = 0;
            taille_facteur = 1.0;
            perso2.taille_facteur = 1.0;
        }
        if (malus_petit_en_cours && clock() - debut_malus_petit - pause_malus_petit >= duree_bonus) {
            malus_petit_en_cours = 0;
            taille_facteur = 1.0;
            perso2.taille_facteur = 1.0;
        }

        deplacer_personnage(&perso2);
        animer_personnage(&perso2);

        // --- PAUSE ---
        if (key[KEY_P]) {
            int debut_pause = clock();

            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
            textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
            textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;
                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;
                    break;
                }

                if (key[KEY_2]) {
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;

                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                    if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                    if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                    if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                    if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                    temps_pause_total += clock() - debut_pause;
                    joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                    sauvegarder_partie_niveau2(joueur, &perso2, scroll_x,
                                               bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                               bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                               debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                               taille_facteur, vivants, nombre_oiseaux);
                    clear_keybuf();
                    // destruction des objets graphiques
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    detruire_diagonales();
                    detruire_pics();
                    detruire_noirs();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    rest(200);
                    return 0;
                }
            }
        }


        int old_x = perso2.x_monde;
        int old_y = perso2.y;

        if (nombre_oiseaux == 1) {
            int x_ecran = perso2.x_monde - scroll_x;
            if (taille_facteur == 1.0) {
                afficher_personnage(&perso2, page, scroll_x);
            } else {
                stretch_sprite(page, perso2.image[perso2.current_frame],
                               (int)(x_ecran - perso2.image[0]->w * taille_facteur / 2),
                               (int)(perso2.y - perso2.image[0]->h * taille_facteur / 2),
                               (int)(perso2.image[0]->w * taille_facteur),
                               (int)(perso2.image[0]->h * taille_facteur));
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                if (i == 1) { perso2.x_monde = old_x + 50; perso2.y = old_y - 50; }
                if (i == 2) { perso2.x_monde = old_x + 50; perso2.y = old_y + 50; }
                int x_ecran = perso2.x_monde - scroll_x;
                if (taille_facteur == 1.0) {
                    afficher_personnage(&perso2, page, scroll_x);
                } else {
                    stretch_sprite(page, perso2.image[perso2.current_frame],
                                   (int)(x_ecran - perso2.image[0]->w * taille_facteur / 2),
                                   (int)(perso2.y - perso2.image[0]->h * taille_facteur / 2),
                                   (int)(perso2.image[0]->w * taille_facteur),
                                   (int)(perso2.image[0]->h * taille_facteur));
                }
            }
            perso2.x_monde = old_x;
            perso2.y = old_y;
        }

        int rayon_collision = (int)(perso2.image[0]->w * taille_facteur / 3);

        if (nombre_oiseaux == 1) {
            if (collision_pixel_noir(page, perso2.x_monde, perso2.y, rayon_collision)) {
                collision = 1;
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                int cx = old_x, cy = old_y;
                if (i == 1) { cx += 30; cy -= 30; }
                if (i == 2) { cx += 30; cy += 30; }
                if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                    vivants[i] = 0;
                }
            }
            int vivants_restants = 0;
            for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
            nombre_oiseaux = vivants_restants;
            if (nombre_oiseaux == 0) collision = 1;
        }

        // Collision pixel noir
        int x_ecran = perso2.x_monde - scroll_x;
        int x_collision = x_ecran;  // Coordonnées exactes de la collision
        int y_collision = perso2.y;
        // Rayon de collision simplifié (petit rectangle autour du personnage)
        int largeur_collision = perso2.image[0]->w / 3;
        int hauteur_collision = perso2.image[0]->h;


        // Collision pixel noir (zone restreinte autour du personnage)
        collision = 0; // Reset de la variable collision

        for (int dx = 0; dx < largeur_collision; dx++) {
            for (int dy = 0; dy < hauteur_collision; dy++) {
                if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                    collision = 1;
                    x_collision += dx;
                    y_collision += dy;
                    break;
                }
            }
            if (collision) break;
        }


        if (collision) {
            while (perso2.y < 580) {
                perso2.vy += 1;
                perso2.y += perso2.vy;
                clear_bitmap(page);
                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                afficher_personnage(&perso2, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }
            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(1000);
            break;
        }

        int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


// Nouvelle condition pour déclencher l'animation de fin
        if (perso2.x_monde >= cible_x) {
            int cible_y = 300;
            double vitesse_y = 5.0;
            int compteur_frame = 0;

            perso2.current_frame = 0;

            // Phase 1 : mouvement vertical vers le centre
            while (perso2.y != cible_y) {
                if (perso2.y < cible_y) {
                    perso2.y += (int)vitesse_y;
                    if (perso2.y > cible_y) perso2.y = cible_y;
                } else if (perso2.y > cible_y) {
                    perso2.y -= (int)vitesse_y;
                    if (perso2.y < cible_y) perso2.y = cible_y;
                }

                compteur_frame++;
                if (compteur_frame >= 25) {
                    perso2.current_frame = (perso2.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso2, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Phase 2 : déplacement horizontal accéléré
            int cible_x_fin = perso2.x_monde + 800;
            double vitesse_anim = 1.0;
            clock_t last_accel_time = clock();
            perso2.saute = 1;

            while (perso2.x_monde < cible_x_fin) {
                if ((clock() - last_accel_time) >= 100) {
                    vitesse_anim += 0.8 ;
                    last_accel_time = clock();
                }

                compteur_frame++;
                if (compteur_frame >= 13) {
                    perso2.current_frame = (perso2.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                perso2.x_monde += (int)vitesse_anim;

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso2, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Message de fin
            clear_to_color(page, makecol(0, 255, 0));
            textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
            textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
            textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    fin_partie = 1;
                    rejouer = 1;
                    break;
                }
                if (key[KEY_2]) {
                    rest(200);
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    return 0;
                }
            }
            break;
        }



        // --- JAUGES ---
        int xj = 570, yj = 20, hj = 20, lj = 200;
        if (bonus_vitesse_en_cours) {
            int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
            yj += 25;
        }
        if (bonus_demulti_en_cours) {
            int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
            yj += 25;
        }
        if (bonus_taille_en_cours) {
            int t = clock() - debut_bonus_taille - pause_bonus_taille;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
            yj += 25;
        }
        if (malus_petit_en_cours) {
            int t = clock() - debut_malus_petit - pause_malus_petit;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
            yj += 25;
        }


        blit(page, screen, 0, 0, 0, 0, 800, 600);
        rest(20);
    }

    detruire_scies();
    detruire_piquants();
    detruire_stalactites();
    detruire_diagonales();
    detruire_pics();
    detruire_noirs();
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(bonus_vitesse);
    destroy_bitmap(bonus_demulti);
    destroy_bitmap(bonus_taille);
    destroy_bitmap(malus_petit);

    return 0;
}




int niveau3(Joueur *joueur) {

    niveau_intro(3);

    int rejouer = 1;

    while (rejouer) {
        BITMAP *page = create_bitmap(800, 600);
        BITMAP *fond = load_bitmap("fondespace.bmp", NULL);
        BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
        BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
        BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
        BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);
        if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
            allegro_message("Erreur chargement bitmaps");
            return 1;
        }

        Personnage perso;
        init_personnage3(&perso);
        animer_intro(&perso, fond, page, 3);


        init_scies3();
        init_piquants3();
        init_stalactites3();
        init_diagonales3();
        init_pics3();
        init_noirs3();
        init_canons(canons);
        init_gris();

        int scroll_x = 0;
        int timer = 56500;
        int debut = clock();
        int collision = 0;
        int fin_partie = 0;
        int temps_pause_total = 0;

        // Positions des bonus
        int x_bonus_vitesse = 2200, y_bonus_vitesse = 200;
        int x_bonus_demulti = 1000, y_bonus_demulti = 300;
        int x_bonus_taille = 1600, y_bonus_taille = 340;
        int x_malus_petit = 1900, y_malus_petit = 340;

        // États des bonus/malus
        int bonus_vitesse_actif = 1, bonus_demulti_actif = 1, bonus_taille_actif = 1, malus_petit_actif = 1;
        int bonus_vitesse_en_cours = 0, bonus_demulti_en_cours = 0, bonus_taille_en_cours = 0, malus_petit_en_cours = 0;
        int debut_bonus_vitesse = 0, debut_bonus_demulti = 0, debut_bonus_taille = 0, debut_malus_petit = 0;
        const int duree_bonus = 10000;

        double taille_facteur = 1.0;
        int nombre_oiseaux = 1;
        int vivants[3] = {1, 0, 0};

        int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
        int pause_bonus_taille = 0, pause_malus_petit = 0;


        int effet_bonus_actif = 0;
        int debut_effet_bonus = 0;
        int couleur_effet_bonus = makecol(255, 255, 255);  // par défaut blanc
        const int duree_effet_bonus = 300;  // en ms


        while (!key[KEY_ESC] && !fin_partie) {
            scroll_x += 2 * perso.vitesse;
            if (scroll_x >= fond->w) scroll_x = 0;

            clear_bitmap(page);
            blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
            blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

            afficher_scies(page, scroll_x);
            mettre_a_jour_piquants();
            afficher_piquants(page, scroll_x);
            afficher_stalactites(page, scroll_x);
            afficher_diagonales(page, scroll_x);
            mettre_a_jour_diagonales();
            mettre_a_jour_pics();
            afficher_pics(page, scroll_x);
            afficher_noirs(page, scroll_x);
            mettre_a_jour_noirs();
            mettre_a_jour_scies();
            mettre_a_jour_stalactites();
            afficher_gris(page, scroll_x);
            afficher_canons(page, canons, scroll_x);
            mettre_a_jour_canons(canons);

            // BONUS VITESSE
            int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
            if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
                stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
                if (perso.x_monde >= x_bonus_vitesse && perso.x_monde <= x_bonus_vitesse + 50 &&
                    perso.y >= y_bonus_vitesse && perso.y <= y_bonus_vitesse + 50) {
                    bonus_vitesse_actif = 0;
                    perso.vitesse *= 2;
                    perso.vx *= 2;

                    bonus_vitesse_en_cours = 1;
                    debut_bonus_vitesse = clock();

                    effet_bonus_actif = 1;
                    debut_effet_bonus = clock();
                    couleur_effet_bonus = makecol(255, 255, 255);  // BLANC
                }
            }

            int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
            if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
                stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
                if (perso.x_monde >= x_bonus_demulti && perso.x_monde <= x_bonus_demulti + 50 &&
                    perso.y >= y_bonus_demulti && perso.y <= y_bonus_demulti + 50) {
                    bonus_demulti_actif = 0;
                    nombre_oiseaux = 3;
                    vivants[0] = vivants[1] = vivants[2] = 1;
                    bonus_demulti_en_cours = 1;
                    debut_bonus_demulti = clock();
                }
            }

            if (bonus_vitesse_en_cours) {
                set_trans_blender(0, 0, 0, 80);  // transparence douce
                drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                rectfill(page, 0, 0, 800, 600, makecol(55, 30, 60) );
                drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);  // retour normal

            }       // source : https://liballeg.org/stabledocs/en/alleg020.html

            // Expiration des effets
            if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
                perso.vitesse /= 2;
                perso.vx /= 2;
                bonus_vitesse_en_cours = 0;
            }
            if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
                bonus_demulti_en_cours = 0;
                nombre_oiseaux = 3;
                vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
            }


            deplacer_personnage(&perso);
            animer_personnage(&perso);
            int old_x = perso.x_monde, old_y = perso.y;

            if (nombre_oiseaux == 1) {
                if (taille_facteur == 1.0)
                    afficher_personnage(&perso, page, scroll_x);
                else
                    stretch_sprite(page, perso.image[perso.current_frame],
                                   (int)(perso.x_monde - perso.image[0]->w * taille_facteur / 2),
                                   (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                   (int)(perso.image[0]->w * taille_facteur),
                                   (int)(perso.image[0]->h * taille_facteur));
            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    if (i == 0) { perso.x_monde = old_x; perso.y = old_y; }
                    if (i == 1) { perso.x_monde = old_x + 50; perso.y = old_y - 30; }
                    if (i == 2) { perso.x_monde = old_x + 50; perso.y = old_y + 30; }

                    if (taille_facteur == 1.0)
                        afficher_personnage(&perso, page, scroll_x);
                    else
                        stretch_sprite(page, perso.image[perso.current_frame],
                                       (int)(perso.x_monde - perso.image[0]->w * taille_facteur / 2),
                                       (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                       (int)(perso.image[0]->w * taille_facteur),
                                       (int)(perso.image[0]->h * taille_facteur));
                }
                perso.x_monde = old_x;
                perso.y = old_y;
            }


            int rayon_collision = (int)(perso.image[0]->w * taille_facteur / 3);

            if (nombre_oiseaux == 1) {
                if (collision_pixel_noir(page, perso.x_monde, perso.y, rayon_collision)) {
                    collision = 1;
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    int cx = old_x, cy = old_y;
                    if (i == 1) { cx += 30; cy -= 30; }
                    if (i == 2) { cx += 30; cy += 30; }
                    if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                        vivants[i] = 0;
                    }
                }

                int vivants_restants = 0;
                for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
                nombre_oiseaux = vivants_restants;

                if (nombre_oiseaux == 0) collision = 1;
            }






            // --- JAUGES ---
            int xj = 570, yj = 20, hj = 20, lj = 200;
            if (bonus_vitesse_en_cours) {
                int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
                yj += 25;
            }
            if (bonus_demulti_en_cours) {
                int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
                yj += 25;
            }
            if (bonus_taille_en_cours) {
                int t = clock() - debut_bonus_taille - pause_bonus_taille;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
                yj += 25;
            }
            if (malus_petit_en_cours) {
                int t = clock() - debut_malus_petit - pause_malus_petit;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
                yj += 25;
            }

            // Pause si la touche P est appuyée
            if (key[KEY_P]) {
                int debut_pause = clock();

                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
                textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        break;
                    }
                    if (key[KEY_2]) {
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                        if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                        if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                        if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                        temps_pause_total += clock() - debut_pause;
                        joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                        sauvegarder_partie_niveau3(joueur, &perso, scroll_x,
                                                   bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                                   bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                                   debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                                   taille_facteur, vivants, nombre_oiseaux);
                        clear_keybuf();
                        // destruction des objets graphiques
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        detruire_gris();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        rest(200);
                        return 0;
                    }
                }
            }


            // Collision pixel noir
            int x_ecran = perso.x_monde - scroll_x;
            int x_collision = x_ecran;  // Coordonnées exactes de la collision
            int y_collision = perso.y;
            // Rayon de collision simplifié (petit rectangle autour du personnage)
            int largeur_collision = perso.image[0]->w / 3;
            int hauteur_collision = perso.image[0]->h;


            // Collision pixel noir (zone restreinte autour du personnage)
            collision = 0; // Reset de la variable collision

            for (int dx = 0; dx < largeur_collision; dx++) {
                for (int dy = 0; dy < hauteur_collision; dy++) {
                    if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                        collision = 1;
                        x_collision += dx;
                        y_collision += dy;
                        break;
                    }
                }
                if (collision) break;
            }

            if (perso.x_monde < scroll_x) {
                // Le personnage a été dépassé par le bord gauche de l'écran
                collision = 1; // Collision déclenche le Game Over
            }

            collision_bloc_gris(&perso,scroll_x);

            // Si collision → explosion + chute + Game Over
            if (collision) {
                // Charger le personnage squelette
                BITMAP *squelette = load_bitmap("squelette.bmp", NULL);
                if (!squelette) {
                    allegro_message("Erreur : impossible de charger squelette.bmp");
                    return 5;
                }

                // --- Animation de l'explosion ---
                BITMAP *explosion[5];
                for (int i = 0; i < 5; i++) {
                    char nom_explosion[20];
                    sprintf(nom_explosion, "explo%d.bmp", i);
                    explosion[i] = load_bitmap(nom_explosion, NULL);
                }

                // Affichage de l'explosion (5 images)
                for (int i = 0; i < 5; i++) {
                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_personnage(&perso, page, scroll_x);

                    // Afficher l'explosion à l'endroit de la collision
                    if (explosion[i]) {
                        draw_sprite(page, explosion[i], x_collision, y_collision);
                    }

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(100);
                }

                // Libération des bitmaps de l'explosion
                for (int i = 0; i < 5; i++) {
                    if (explosion[i]) {
                        destroy_bitmap(explosion[i]);
                    }
                }

                // --- Animation de chute du squelette ---
                while (y_collision < 580) {
                    y_collision += 5;  // Chute accélérée
                    clear_bitmap(page);

                    // Afficher l'arrière-plan
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w, 600);

                    // Afficher les autres éléments
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);

                    // Afficher le squelette à la place du personnage
                    draw_sprite(page, squelette, x_collision, y_collision);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Libérer le bitmap squelette
                destroy_bitmap(squelette);
                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 260, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Retour menu", 300, 290, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                while (1) {
                    if (key[KEY_1]) { rest(200); fin_partie = 1; rejouer = 1; break; }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_canons(canons);
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }                }
                break;
            }

            int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


            // Nouvelle condition pour déclencher l'animation de fin
            if (perso.x_monde >= cible_x) {
                int cible_y = 300;
                double vitesse_y = 5.0;
                int compteur_frame = 0;

                perso.current_frame = 0;

                // Phase 1 : mouvement vertical vers le centre
                while (perso.y != cible_y) {
                    if (perso.y < cible_y) {
                        perso.y += (int)vitesse_y;
                        if (perso.y > cible_y) perso.y = cible_y;
                    } else if (perso.y > cible_y) {
                        perso.y -= (int)vitesse_y;
                        if (perso.y < cible_y) perso.y = cible_y;
                    }

                    compteur_frame++;
                    if (compteur_frame >= 25) {
                        perso.current_frame = (perso.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);

                    afficher_personnage(&perso, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Phase 2 : déplacement horizontal accéléré
                int cible_x_fin = perso.x_monde + 800;
                double vitesse_anim = 1.0;
                clock_t last_accel_time = clock();
                perso.saute = 1;

                while (perso.x_monde < cible_x_fin) {
                    if ((clock() - last_accel_time) >= 100) {
                        vitesse_anim += 0.8 ;
                        last_accel_time = clock();
                    }

                    compteur_frame++;
                    if (compteur_frame >= 13) {
                        perso.current_frame = (perso.current_frame + 1) % 4;
                        compteur_frame = 0;
                    }

                    perso.x_monde += (int)vitesse_anim;

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);

                    afficher_personnage(&perso, page, scroll_x);
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Message de fin
                clear_to_color(page, makecol(0, 255, 0));
                textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
                textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
                textout_ex(page, font, "3. Passer au Niveau 4", 300, 290, makecol(0, 0, 0), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_gris();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                    if (key[KEY_3]) {
                        rest(200);
                        // Passer directement au Niveau 2
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        detruire_gris();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);

                        // Charger Niveau 2 directement
                        return niveau4(joueur); // Assure-toi que la fonction niveau2() est définie
                    }
                }
                break;
            }

            // Éclat visuel si un bonus vient d'être ramassé
            if (effet_bonus_actif) {
                int temps_effet = clock() - debut_effet_bonus;
                if (temps_effet < duree_effet_bonus) {
                    int x_ecran = perso.x_monde - scroll_x;
                    int intensite = 255 - (255 * temps_effet / duree_effet_bonus);
                    if (intensite < 0) intensite = 0;

                    int r = getr(couleur_effet_bonus);
                    int g = getg(couleur_effet_bonus);
                    int b = getb(couleur_effet_bonus);

                    circlefill(page, x_ecran, perso.y, 50, makecol(
                            r * intensite / 255,
                            g * intensite / 255,
                            b * intensite / 255
                    ));

                } else {
                    effet_bonus_actif = 0;
                }
            }

            int tremble_x = 0;
            int tremble_y = 0;

            if (bonus_vitesse_en_cours && rand() % 5 == 0) {
                tremble_x = (rand() % 5) - 2;
            }

            blit(page, screen, 0, 0, tremble_x, tremble_y, 800, 600);
            rest(20);

        }

        nettoyer:
        detruire_scies();
        detruire_piquants();
        detruire_stalactites();
        detruire_diagonales();
        detruire_pics();
        detruire_noirs();
        detruire_gris();
        destroy_bitmap(page);
        destroy_bitmap(fond);
        destroy_bitmap(bonus_vitesse);
        destroy_bitmap(bonus_demulti);
        destroy_bitmap(bonus_taille);
        destroy_bitmap(malus_petit);
    }

    return 0;
}



int reprendre_niveau3(Joueur *joueur) {
    int rejouer = 1;

    // Lecture de la sauvegarde
    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", joueur->pseudo);
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        allegro_message("Aucune partie jouée.");
        return 1;
    }

    fscanf(f, "%s", joueur->pseudo);
    fscanf(f, "%d", &joueur->niveau);
    fscanf(f, "%d", &joueur->temps_jeu);

    Personnage perso;
    init_personnage3(&perso);
    fscanf(f, "%d %d", &perso.x_monde, &perso.y);
    fscanf(f, "%d %d %d %d", &perso.vx, &perso.vy, &perso.current_frame, &perso.saute);
    int scroll_x;
    fscanf(f, "%d", &scroll_x);

    int bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif;
    int bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours;
    int debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit;
    double taille_facteur;
    int vivants[3];
    int nombre_oiseaux;


    fscanf(f, "%d %d %d %d", &bonus_vitesse_actif, &bonus_demulti_actif, &bonus_taille_actif, &malus_petit_actif);
    fscanf(f, "%d %d %d %d", &bonus_vitesse_en_cours, &bonus_demulti_en_cours, &bonus_taille_en_cours, &malus_petit_en_cours);
    fscanf(f, "%d %d %d %d", &debut_bonus_vitesse, &debut_bonus_demulti, &debut_bonus_taille, &debut_malus_petit);
    fscanf(f, "%lf", &taille_facteur);
    fscanf(f, "%d %d %d", &vivants[0], &vivants[1], &vivants[2]);

    fscanf(f, "%d", &nombre_oiseaux);

    fscanf(f, "%d", &perso.vitesse);

    fclose(f);






    perso.compteur_frame = 0;
    perso.delai_frame = 5;

    BITMAP *page = create_bitmap(800, 600);
    BITMAP *fond = load_bitmap("fondespace.bmp", NULL);
    BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
    BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
    BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
    BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);

    if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
        allegro_message("Erreur chargement bitmaps");
        return 1;
    }

    init_scies3();
    init_piquants3();
    init_stalactites3();
    init_diagonales3();
    init_pics3();
    init_noirs3();

    int timer = joueur->temps_jeu;
    int x_bonus_vitesse = 900, y_bonus_vitesse = 340;
    int x_bonus_demulti = 1285, y_bonus_demulti = 340;
    int x_bonus_taille = 1600, y_bonus_taille = 340;
    int x_malus_petit = 1900, y_malus_petit = 340;

    int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
    int pause_bonus_taille = 0, pause_malus_petit = 0;

    int debut = clock();


    if (bonus_vitesse_en_cours) debut_bonus_vitesse = debut;
    if (bonus_demulti_en_cours) debut_bonus_demulti = debut;
    if (bonus_taille_en_cours)  debut_bonus_taille = debut;
    if (malus_petit_en_cours)   debut_malus_petit = debut;

    pause_bonus_vitesse = 0;
    pause_bonus_demulti = 0;
    pause_bonus_taille = 0;
    pause_malus_petit = 0;


    int collision = 0;
    int fin_partie = 0;
    int temps_pause_total = 0;
    const int duree_bonus = 10000;



    while (!key[KEY_ESC] && !fin_partie) {
        scroll_x += 2 * perso.vitesse;
        if (scroll_x >= fond->w) scroll_x = 0;

        clear_bitmap(page);
        blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
        blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

        afficher_scies(page, scroll_x);
        mettre_a_jour_piquants();
        afficher_piquants(page, scroll_x);
        afficher_stalactites(page, scroll_x);
        afficher_diagonales(page, scroll_x);
        mettre_a_jour_diagonales();
        mettre_a_jour_pics();
        afficher_pics(page, scroll_x);
        afficher_noirs(page, scroll_x);
        mettre_a_jour_noirs3();
        mettre_a_jour_scies();
        mettre_a_jour_stalactites();

        // BONUS VITESSE
        int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
        if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
            stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
            if (perso.x_monde >= x_bonus_vitesse && perso.x_monde <= x_bonus_vitesse + 50 &&
                perso.y >= y_bonus_vitesse && perso.y <= y_bonus_vitesse + 50) {
                bonus_vitesse_actif = 0;
                perso.vitesse *= 2;
                perso.vx *= 2;
                bonus_vitesse_en_cours = 1;
                debut_bonus_vitesse = clock();
            }
        }

// BONUS DEMULTI
        int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
        if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
            stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
            if (perso.x_monde >= x_bonus_demulti && perso.x_monde <= x_bonus_demulti + 50 &&
                perso.y >= y_bonus_demulti && perso.y <= y_bonus_demulti + 50) {
                bonus_demulti_actif = 0;
                nombre_oiseaux = 3;
                vivants[0] = vivants[1] = vivants[2] = 1;
                bonus_demulti_en_cours = 1;
                debut_bonus_demulti = clock();
            }
        }

// BONUS TAILLE
        int bonus_taille_ecran = x_bonus_taille - scroll_x;
        if (bonus_taille_actif && bonus_taille_ecran >= -50 && bonus_taille_ecran <= 850) {
            stretch_sprite(page, bonus_taille, bonus_taille_ecran, y_bonus_taille, 50, 50);
            if (perso.x_monde >= x_bonus_taille && perso.x_monde <= x_bonus_taille + 50 &&
                perso.y >= y_bonus_taille && perso.y <= y_bonus_taille + 50) {
                bonus_taille_actif = 0;
                taille_facteur = 2.0;
                bonus_taille_en_cours = 1;
                debut_bonus_taille = clock();
                perso.taille_facteur = 2.0;

            }
        }

// MALUS PETIT
        int malus_petit_ecran = x_malus_petit - scroll_x;
        if (malus_petit_actif && malus_petit_ecran >= -50 && malus_petit_ecran <= 850) {
            stretch_sprite(page, malus_petit, malus_petit_ecran, y_malus_petit, 50, 50);
            if (perso.x_monde >= x_malus_petit && perso.x_monde <= x_malus_petit + 50 &&
                perso.y >= y_malus_petit && perso.y <= y_malus_petit + 50) {
                malus_petit_actif = 0;
                taille_facteur = 0.5;
                malus_petit_en_cours = 1;
                debut_malus_petit = clock();
            }
        }


        if (bonus_vitesse_en_cours) {
            set_trans_blender(0, 0, 0, 80);  // transparence douce
            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
            rectfill(page, 0, 0, 800, 600, makecol(55, 30, 60) );
            drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);  // retour normal

        }




        // Expiration des effets bonus/malus
        if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
            perso.vitesse /= 2;
            perso.vx /= 2;
            bonus_vitesse_en_cours = 0;
        }
        if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
            bonus_demulti_en_cours = 0;
            nombre_oiseaux = 1;
            vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
        }
        if (bonus_taille_en_cours && clock() - debut_bonus_taille - pause_bonus_taille >= duree_bonus) {
            bonus_taille_en_cours = 0;
            taille_facteur = 1.0;
            perso.taille_facteur = 1.0;
        }
        if (malus_petit_en_cours && clock() - debut_malus_petit - pause_malus_petit >= duree_bonus) {
            malus_petit_en_cours = 0;
            taille_facteur = 1.0;
            perso.taille_facteur = 1.0;
        }

        deplacer_personnage(&perso);
        animer_personnage(&perso);

        // --- PAUSE ---
        if (key[KEY_P]) {
            int debut_pause = clock();

            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
            textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
            textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;
                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;
                    break;
                }

                if (key[KEY_2]) {
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;

                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                    if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                    if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                    if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                    if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                    temps_pause_total += clock() - debut_pause;
                    joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                    sauvegarder_partie_niveau3(joueur, &perso, scroll_x,
                                               bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                               bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                               debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                               taille_facteur, vivants, nombre_oiseaux);
                    clear_keybuf();
                    // destruction des objets graphiques
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    detruire_diagonales();
                    detruire_pics();
                    detruire_noirs();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    rest(200);
                    return 0;
                }
            }
        }


        int old_x = perso.x_monde;
        int old_y = perso.y;

        if (nombre_oiseaux == 1) {
            int x_ecran = perso.x_monde - scroll_x;
            if (taille_facteur == 1.0) {
                afficher_personnage(&perso, page, scroll_x);
            } else {
                stretch_sprite(page, perso.image[perso.current_frame],
                               (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                               (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                               (int)(perso.image[0]->w * taille_facteur),
                               (int)(perso.image[0]->h * taille_facteur));
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                if (i == 1) { perso.x_monde = old_x + 50; perso.y = old_y - 50; }
                if (i == 2) { perso.x_monde = old_x + 50; perso.y = old_y + 50; }
                int x_ecran = perso.x_monde - scroll_x;
                if (taille_facteur == 1.0) {
                    afficher_personnage(&perso, page, scroll_x);
                } else {
                    stretch_sprite(page, perso.image[perso.current_frame],
                                   (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                                   (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                   (int)(perso.image[0]->w * taille_facteur),
                                   (int)(perso.image[0]->h * taille_facteur));
                }
            }
            perso.x_monde = old_x;
            perso.y = old_y;
        }

        int rayon_collision = (int)(perso.image[0]->w * taille_facteur / 3);

        if (nombre_oiseaux == 1) {
            if (collision_pixel_noir(page, perso.x_monde, perso.y, rayon_collision)) {
                collision = 1;
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                int cx = old_x, cy = old_y;
                if (i == 1) { cx += 30; cy -= 30; }
                if (i == 2) { cx += 30; cy += 30; }
                if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                    vivants[i] = 0;
                }
            }
            int vivants_restants = 0;
            for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
            nombre_oiseaux = vivants_restants;
            if (nombre_oiseaux == 0) collision = 1;
        }

        // Collision pixel noir
        int x_ecran = perso.x_monde - scroll_x;
        int x_collision = x_ecran;  // Coordonnées exactes de la collision
        int y_collision = perso.y;
        // Rayon de collision simplifié (petit rectangle autour du personnage)
        int largeur_collision = perso.image[0]->w / 3;
        int hauteur_collision = perso.image[0]->h;


        // Collision pixel noir (zone restreinte autour du personnage)
        collision = 0; // Reset de la variable collision

        for (int dx = 0; dx < largeur_collision; dx++) {
            for (int dy = 0; dy < hauteur_collision; dy++) {
                if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                    collision = 1;
                    x_collision += dx;
                    y_collision += dy;
                    break;
                }
            }
            if (collision) break;
        }


        if (collision) {
            while (perso.y < 580) {
                perso.vy += 1;
                perso.y += perso.vy;
                clear_bitmap(page);
                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }
            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(1000);
            break;
        }

        int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


// Nouvelle condition pour déclencher l'animation de fin
        if (perso.x_monde >= cible_x) {
            int cible_y = 300;
            double vitesse_y = 5.0;
            int compteur_frame = 0;

            perso.current_frame = 0;

            // Phase 1 : mouvement vertical vers le centre
            while (perso.y != cible_y) {
                if (perso.y < cible_y) {
                    perso.y += (int)vitesse_y;
                    if (perso.y > cible_y) perso.y = cible_y;
                } else if (perso.y > cible_y) {
                    perso.y -= (int)vitesse_y;
                    if (perso.y < cible_y) perso.y = cible_y;
                }

                compteur_frame++;
                if (compteur_frame >= 25) {
                    perso.current_frame = (perso.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Phase 2 : déplacement horizontal accéléré
            int cible_x_fin = perso.x_monde + 800;
            double vitesse_anim = 1.0;
            clock_t last_accel_time = clock();
            perso.saute = 1;

            while (perso.x_monde < cible_x_fin) {
                if ((clock() - last_accel_time) >= 100) {
                    vitesse_anim += 0.8 ;
                    last_accel_time = clock();
                }

                compteur_frame++;
                if (compteur_frame >= 13) {
                    perso.current_frame = (perso.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                perso.x_monde += (int)vitesse_anim;

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Message de fin
            clear_to_color(page, makecol(0, 255, 0));
            textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
            textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
            textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    fin_partie = 1;
                    rejouer = 1;
                    break;
                }
                if (key[KEY_2]) {
                    rest(200);
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    return 0;
                }
            }
            break;
        }



        // --- JAUGES ---
        int xj = 570, yj = 20, hj = 20, lj = 200;
        if (bonus_vitesse_en_cours) {
            int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
            yj += 25;
        }
        if (bonus_demulti_en_cours) {
            int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
            yj += 25;
        }
        if (bonus_taille_en_cours) {
            int t = clock() - debut_bonus_taille - pause_bonus_taille;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
            yj += 25;
        }
        if (malus_petit_en_cours) {
            int t = clock() - debut_malus_petit - pause_malus_petit;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
            yj += 25;
        }


        int tremble_x = 0;
        int tremble_y = 0;

        if (bonus_vitesse_en_cours && rand() % 5 == 0) {
            tremble_x = (rand() % 5) - 2;
        }

        blit(page, screen, 0, 0, tremble_x, tremble_y, 800, 600);
        rest(20);
    }

    detruire_scies();
    detruire_piquants();
    detruire_stalactites();
    detruire_diagonales();
    detruire_pics();
    detruire_noirs();
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(bonus_vitesse);
    destroy_bitmap(bonus_demulti);
    destroy_bitmap(bonus_taille);
    destroy_bitmap(malus_petit);

    return 0;
}









int niveau4(Joueur *joueur) {

    niveau_intro(4);


    int rejouer = 1;

    while (rejouer) {
        BITMAP *page = create_bitmap(800, 600);
        BITMAP *fond = load_bitmap("fond_nuit.bmp", NULL);
        BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
        BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
        BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
        BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);
        if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
            allegro_message("Erreur chargement bitmaps");
            return 1;
        }

        Personnage perso;
        init_personnage4(&perso);
        animer_intro(&perso, fond, page, 4);

        init_scies4();
        init_piquants4();
        init_stalactites4();
        init_diagonales4();
        init_pics4();
        init_noirs4();
        init_portails();


        int scroll_x = 0;
        int timer = 56500;
        int debut = clock();
        int collision = 0;
        int fin_partie = 0;
        int temps_pause_total = 0;

        // Positions des bonus
        int x_bonus_vitesse = 9000, y_bonus_vitesse = 340;
        int x_bonus_demulti = 12850, y_bonus_demulti = 340;
        int x_bonus_taille = 1875, y_bonus_taille = 350;
        int x_malus_petit = 500, y_malus_petit = 340;


        // États des bonus/malus
        int bonus_vitesse_actif = 1, bonus_demulti_actif = 1, bonus_taille_actif = 1, malus_petit_actif = 1;
        int bonus_vitesse_en_cours = 0, bonus_demulti_en_cours = 0, bonus_taille_en_cours = 0, malus_petit_en_cours = 0;
        int debut_bonus_vitesse = 0, debut_bonus_demulti = 0, debut_bonus_taille = 0, debut_malus_petit = 0;
        const int duree_bonus_P = 21000;
        const int duree_bonus = 15000;

        double taille_facteur = 1.0;
        int nombre_oiseaux = 1;
        int vivants[3] = {1, 0, 0};

        int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
        int pause_bonus_taille = 0, pause_malus_petit = 0;

        int effet_bonus_actif = 0;
        int debut_effet_bonus = 0;
        int couleur_effet_bonus = makecol(255, 255, 255);  // par défaut blanc
        const int duree_effet_bonus = 300;  // en ms




        while (!key[KEY_ESC] && !fin_partie) {
            scroll_x += 2 * perso.vitesse;
            if (scroll_x >= fond->w) scroll_x = 0;

            clear_bitmap(page);
            blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
            blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

            afficher_scies(page, scroll_x);
            mettre_a_jour_piquants();
            afficher_piquants(page, scroll_x);
            afficher_stalactites(page, scroll_x);
            afficher_diagonales(page, scroll_x);
            mettre_a_jour_diagonales();
            mettre_a_jour_pics();
            afficher_pics(page, scroll_x);
            afficher_noirs(page, scroll_x);
            mettre_a_jour_noirs4();
            mettre_a_jour_scies();
            mettre_a_jour_stalactites();
            afficher_portails(page, scroll_x);
            mettre_a_jour_portails();


            // BONUS VITESSE
            int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
            if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
                stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
                if (perso.x_monde >= x_bonus_vitesse && perso.x_monde <= x_bonus_vitesse + 50 &&
                    perso.y >= y_bonus_vitesse && perso.y <= y_bonus_vitesse + 50) {
                    bonus_vitesse_actif = 0;
                    perso.vitesse *= 2;
                    perso.vx *= 2;

                    bonus_vitesse_en_cours = 1;
                    debut_bonus_vitesse = clock();


                    effet_bonus_actif = 1;
                    debut_effet_bonus = clock();
                    couleur_effet_bonus = makecol(255, 255, 255);  // BLANC
                }
            }

            // BONUS DEMULTI
            int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
            if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
                stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
                if (perso.x_monde >= x_bonus_demulti && perso.x_monde <= x_bonus_demulti + 50 &&
                    perso.y >= y_bonus_demulti && perso.y <= y_bonus_demulti + 50)
                {
                    bonus_demulti_actif = 0;
                    nombre_oiseaux = 3;
                    vivants[0] = vivants[1] = vivants[2] = 1;
                    bonus_demulti_en_cours = 1;
                    debut_bonus_demulti = clock();


                    effet_bonus_actif = 1;
                    debut_effet_bonus = clock();
                    couleur_effet_bonus = makecol(255, 0, 0);  // ROUGE
                }
            }

            // BONUS TAILLE
            int bonus_taille_ecran = x_bonus_taille - scroll_x;
            if (bonus_taille_actif && bonus_taille_ecran >= -50 && bonus_taille_ecran <= 850) {
                stretch_sprite(page, bonus_taille, bonus_taille_ecran, y_bonus_taille, 50, 50);
                if (perso.x_monde >= x_bonus_taille && perso.x_monde <= x_bonus_taille + 50 &&
                    perso.y >= y_bonus_taille && perso.y <= y_bonus_taille + 50) {
                    bonus_taille_actif = 0;
                    taille_facteur = 1.35;
                    bonus_taille_en_cours = 1;
                    debut_bonus_taille = clock();
                    perso.taille_facteur = 1.35;


                    effet_bonus_actif = 1;
                    debut_effet_bonus = clock();
                    couleur_effet_bonus = makecol(0, 255, 0);  // VERT
                }
            }

            // MALUS PETIT
            int malus_petit_ecran = x_malus_petit - scroll_x;
            if (malus_petit_actif && malus_petit_ecran >= -50 && malus_petit_ecran <= 850) {
                stretch_sprite(page, malus_petit, malus_petit_ecran, y_malus_petit, 50, 50);
                if (perso.x_monde >= x_malus_petit && perso.x_monde <= x_malus_petit + 50 &&
                    perso.y >= y_malus_petit && perso.y <= y_malus_petit + 50){
                    malus_petit_actif = 0;
                    taille_facteur = 0.5;
                    perso.taille_facteur = 0.5;

                    malus_petit_en_cours = 1;
                    debut_malus_petit = clock();

                    effet_bonus_actif = 1;
                    debut_effet_bonus = clock();
                    couleur_effet_bonus = makecol(255, 165, 0);  // ORANGE
                }
            }

            // Expiration des effets
            if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
                perso.vitesse /= 2;
                perso.vx /= 2;
                bonus_vitesse_en_cours = 0;
            }
            if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
                bonus_demulti_en_cours = 0;
                nombre_oiseaux = 1;
                vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
            }
            if (bonus_taille_en_cours && clock() - debut_bonus_taille - pause_bonus_taille >= duree_bonus) {
                bonus_taille_en_cours = 0;
                taille_facteur = 1.0;
                perso.taille_facteur = 1.0;
            }
            if (malus_petit_en_cours && clock() - debut_malus_petit - pause_malus_petit >= duree_bonus_P) {
                malus_petit_en_cours = 0;
                taille_facteur = 1.0;
                perso.taille_facteur = 1.0;
            }


            deplacer_personnage(&perso);
            animer_personnage(&perso);
            int old_x = perso.x_monde, old_y = perso.y;

            if (nombre_oiseaux == 1) {
                int x_ecran = perso.x_monde - scroll_x;

                if (taille_facteur == 1.0)
                    afficher_personnage(&perso, page, scroll_x);
                else
                    stretch_sprite(page, perso.image[perso.current_frame],
                                   (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                                   (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                   (int)(perso.image[0]->w * taille_facteur),
                                   (int)(perso.image[0]->h * taille_facteur));

            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    if (i == 0) { perso.x_monde = old_x; perso.y = old_y; }
                    if (i == 1) { perso.x_monde = old_x + 50; perso.y = old_y - 50; }
                    if (i == 2) { perso.x_monde = old_x + 50; perso.y = old_y + 50; }

                    int x_ecran = perso.x_monde - scroll_x;

                    if (taille_facteur == 1.0) {
                        afficher_personnage(&perso, page, scroll_x);
                    } else {
                        stretch_sprite(page, perso.image[perso.current_frame],
                                       (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                                       (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                       (int)(perso.image[0]->w * taille_facteur),
                                       (int)(perso.image[0]->h * taille_facteur));
                    }

                }
                perso.x_monde = old_x;
                perso.y = old_y;
            }

            // --- COLLISIONS ---
            int rayon_collision = (int)(perso.image[0]->w * taille_facteur / 3);

            if (nombre_oiseaux == 1) {
                if (collision_pixel_noir(page, perso.x_monde, perso.y, rayon_collision)) {
                    collision = 1;
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    if (!vivants[i]) continue;
                    int cx = old_x, cy = old_y;
                    if (i == 1) { cx += 30; cy -= 30; }
                    if (i == 2) { cx += 30; cy += 30; }
                    if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                        vivants[i] = 0;
                    }
                }

                int vivants_restants = 0;
                for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
                nombre_oiseaux = vivants_restants;

                if (nombre_oiseaux == 0) collision = 1;
            }

            // --- JAUGES ---
            int xj = 570, yj = 20, hj = 20, lj = 200;
            if (bonus_vitesse_en_cours) {
                int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
                yj += 25;
            }
            if (bonus_demulti_en_cours) {
                int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
                yj += 25;
            }
            if (bonus_taille_en_cours) {
                int t = clock() - debut_bonus_taille - pause_bonus_taille;
                int w = lj * (duree_bonus - t) / duree_bonus;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
                yj += 25;
            }
            if (malus_petit_en_cours) {
                int t = clock() - debut_malus_petit - pause_malus_petit;
                int w = lj * (duree_bonus_P - t) / duree_bonus_P;
                if (w < 0) w = 0;
                rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
                rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
                yj += 25;
            }

            // Pause si la touche P est appuyée
            if (key[KEY_P]) {
                int debut_pause = clock();

                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
                textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        break;
                    }
                    if (key[KEY_2]) {
                        int duree_pause = clock() - debut_pause;
                        temps_pause_total += duree_pause;

                        if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                        if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                        if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                        if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                        if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                        if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                        if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                        if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                        temps_pause_total += clock() - debut_pause;
                        joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                        sauvegarder_partie_niveau4(joueur, &perso, scroll_x,
                                                   bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                                   bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                                   debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                                   taille_facteur, vivants, nombre_oiseaux);
                        clear_keybuf();
                        // destruction des objets graphiques
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        detruire_diagonales();
                        detruire_pics();
                        detruire_noirs();
                        detruire_portails();

                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        rest(200);
                        return 0;
                    }
                }
            }


// Collision pixel noir
            int x_ecran = perso.x_monde - scroll_x;
            int x_collision = x_ecran;  // Coordonnées exactes de la collision
            int y_collision = perso.y;
            // Rayon de collision simplifié (petit rectangle autour du personnage)
            int largeur_collision, hauteur_collision;

            if (perso.taille_facteur < 1.0) {
                // Taille réduite → on applique le facteur directement
                largeur_collision = (int)((perso.image[0]->w * perso.taille_facteur) / 3);
                hauteur_collision = (int)(perso.image[0]->h * perso.taille_facteur);
            } else if (perso.taille_facteur > 1.0) {
                // Taille augmentée → on fixe manuellement la zone de collision
                largeur_collision = perso.image[0]->w * 0.3;  // Exemple : 1/2 largeur
                hauteur_collision = perso.image[0]->h * 0.8;      // Hauteur normale
            } else {
                // Taille normale
                largeur_collision = perso.image[0]->w / 3;
                hauteur_collision = perso.image[0]->h;
            }




            // Collision pixel noir (zone restreinte autour du personnage)
            collision = 0; // Reset de la variable collision

            for (int dx = 0; dx < largeur_collision; dx++) {
                for (int dy = 0; dy < hauteur_collision; dy++) {
                    if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                        collision = 1;
                        x_collision += dx;
                        y_collision += dy;
                        break;
                    }
                }
                if (collision) break;
            }


// Si collision → explosion + chute + Game Over
            if (collision) {
                // Charger le personnage squelette
                BITMAP *squelette = load_bitmap("squelette.bmp", NULL);
                if (!squelette) {
                    allegro_message("Erreur : impossible de charger squelette.bmp");
                    return 5;
                }

                // --- Animation de l'explosion ---
                BITMAP *explosion[5];
                for (int i = 0; i < 5; i++) {
                    char nom_explosion[20];
                    sprintf(nom_explosion, "explo%d.bmp", i);
                    explosion[i] = load_bitmap(nom_explosion, NULL);
                }

                // Affichage de l'explosion (5 images)
                for (int i = 0; i < 5; i++) {
                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_personnage(&perso, page, scroll_x);

                    // Afficher l'explosion à l'endroit de la collision
                    if (explosion[i]) {
                        draw_sprite(page, explosion[i], x_collision, y_collision);
                    }

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(100);
                }

                // Libération des bitmaps de l'explosion
                for (int i = 0; i < 5; i++) {
                    if (explosion[i]) {
                        destroy_bitmap(explosion[i]);
                    }
                }

                // --- Animation de chute du squelette ---
                while (y_collision < 580) {
                    y_collision += 5;  // Chute accélérée
                    clear_bitmap(page);

                    // Afficher l'arrière-plan
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w, 600);

                    // Afficher les autres éléments
                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);

                    // Afficher le squelette à la place du personnage
                    draw_sprite(page, squelette, x_collision, y_collision);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Libérer le bitmap squelette
                destroy_bitmap(squelette);
                clear_to_color(page, makecol(0, 0, 0));
                textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 260, makecol(255, 255, 255), -1);
                textout_ex(page, font, "2. Retour menu", 300, 290, makecol(255, 255, 255), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                while (1) {
                    if (key[KEY_1]) { rest(200); fin_partie = 1; rejouer = 1; break; }

                }
                break;
            }


            // Condition spéciale : retour au niveau 1 si le joueur atteint un portail
            if (perso.x_monde >= 1660 && perso.x_monde <= 1720 && perso.y >= 0 && perso.y <= 275) {



                for (int i = 0; i < 60; i++) {  // Affiche pendant environ 1,2 seconde (60 * 20 ms)
                    clear_bitmap(page);
                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs4();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    afficher_portails(page, scroll_x);
                    mettre_a_jour_portails();

                    textout_centre_ex(page, font, "PORTAIL DE RETOUR AU NIVEAU 1", 400, 280, makecol(255, 255, 255), -1);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }


                // Animation de secousse
                for (int i = 0; i < 40; i++) {  // 10 secousses rapides
                    clear_bitmap(page);

                    // Valeurs de secousse aléatoire ou alternée
                    int dx = (i % 2 == 0) ? 5 : -5;
                    int dy = (i % 2 == 0) ? 5 : -5;

                    // Affichage du fond avec décalage
                    blit(fond, page, scroll_x, 0, dx, dy, 800, 600);

                    // Tous les éléments à afficher
                    afficher_scies(page, scroll_x);
                    mettre_a_jour_piquants();
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    mettre_a_jour_diagonales();
                    mettre_a_jour_pics();
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);
                    mettre_a_jour_noirs4();
                    mettre_a_jour_scies();
                    mettre_a_jour_stalactites();
                    afficher_portails(page, scroll_x);
                    mettre_a_jour_portails();

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(30);  // durée de chaque frame de secousse
                }




                for (int i = 0; i < 10; i++) {
                    clear_bitmap(page);
                    int taille = perso.image[0]->w + i * 10;
                    int x = perso.x_monde - scroll_x;
                    int y = perso.y;
                    stretch_sprite(page, perso.image[perso.current_frame], x - taille / 2, y - taille / 2, taille, taille);

                    rectfill(page, 0, 0, 800, 600, makecol(i*25, i*25, i*25));  // fondu
                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(100);
                }

                rest(200);  // petit délai pour éviter déclenchement immédiat
                return niveau1(joueur);  // redémarre niveau 1
            }


            int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


            // Nouvelle condition pour déclencher l'animation de fin
            if (perso.x_monde >= cible_x) {
                int cible_y = 300;
                double vitesse_y = 5.0;
                int compteur_frame = 0;
                int etat_anim = 1;

                perso.current_frame = 1;



                // Phase 1 : mouvement vertical vers le centre
                while (perso.y != cible_y) {
                    if (perso.y < cible_y) {
                        perso.y += (int)vitesse_y;
                        if (perso.y > cible_y) perso.y = cible_y;
                    } else if (perso.y > cible_y) {
                        perso.y -= (int)vitesse_y;
                        if (perso.y < cible_y) perso.y = cible_y;
                    }

                    compteur_frame++;
                    if (compteur_frame >= 15) {  // Change de frame toutes les 5 itérations
                        compteur_frame = 0;
                        etat_anim = (etat_anim == 1) ? 2 : 1;  // alterne entre frame 1 et 2
                        perso.current_frame = etat_anim;
                    }


                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);


                    afficher_personnage(&perso, page, scroll_x);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Phase 2 : déplacement horizontal accéléré
                int cible_x = perso.x_monde + 800;
                double vitesse_anim = 1.0;
                clock_t last_accel_time = clock();
                perso.saute = 1;


                while (perso.x_monde < cible_x) {
                    if ((clock() - last_accel_time) >= 100) {
                        vitesse_anim += 0.8 ;
                        last_accel_time = clock();
                    }

                    compteur_frame++;
                    if (compteur_frame >= 12) {  // Change de frame toutes les 5 itérations
                        compteur_frame = 0;
                        etat_anim = (etat_anim == 1) ? 2 : 1;  // alterne entre frame 1 et 2
                        perso.current_frame = etat_anim;
                    }


                    perso.x_monde += (int)vitesse_anim;

                    blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                    blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                    afficher_scies(page, scroll_x);
                    afficher_piquants(page, scroll_x);
                    afficher_stalactites(page, scroll_x);
                    afficher_diagonales(page, scroll_x);
                    afficher_pics(page, scroll_x);
                    afficher_noirs(page, scroll_x);


                    afficher_personnage(&perso, page, scroll_x);

                    blit(page, screen, 0, 0, 0, 0, 800, 600);
                    rest(20);
                }

                // Message de fin
                clear_to_color(page, makecol(0, 255, 0));
                textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
                textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
                textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
                blit(page, screen, 0, 0, 0, 0, 800, 600);

                while (1) {
                    if (key[KEY_1]) {
                        rest(200);
                        fin_partie = 1;
                        rejouer = 1;
                        break;
                    }
                    if (key[KEY_2]) {
                        rest(200);
                        detruire_scies();
                        detruire_piquants();
                        detruire_stalactites();
                        destroy_bitmap(page);
                        destroy_bitmap(fond);
                        return 0;
                    }
                }
                break;
            }

            // Éclat visuel si un bonus vient d'être ramassé
            if (effet_bonus_actif) {
                int temps_effet = clock() - debut_effet_bonus;
                if (temps_effet < duree_effet_bonus) {
                    int x_ecran = perso.x_monde - scroll_x;
                    int intensite = 255 - (255 * temps_effet / duree_effet_bonus);
                    if (intensite < 0) intensite = 0;

                    int r = getr(couleur_effet_bonus);
                    int g = getg(couleur_effet_bonus);
                    int b = getb(couleur_effet_bonus);

                    circlefill(page, x_ecran, perso.y, 50, makecol(
                            r * intensite / 255,
                            g * intensite / 255,
                            b * intensite / 255
                    ));

                } else {
                    effet_bonus_actif = 0;
                }
            }


            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(20);
        }

        nettoyer:
        detruire_scies();
        detruire_piquants();
        detruire_stalactites();
        detruire_diagonales();
        detruire_pics();
        detruire_noirs();
        destroy_bitmap(page);
        destroy_bitmap(fond);
        destroy_bitmap(bonus_vitesse);
        destroy_bitmap(bonus_demulti);
        destroy_bitmap(bonus_taille);
        destroy_bitmap(malus_petit);
    }

    return 0;
}



int reprendre_niveau4(Joueur *joueur) {
    int rejouer = 1;

    // Lecture de la sauvegarde
    char nom_fichier[100];
    sprintf(nom_fichier, "sauvegarde_%s.txt", joueur->pseudo);
    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        allegro_message("Aucune partie jouée.");
        return 1;
    }

    fscanf(f, "%s", joueur->pseudo);
    fscanf(f, "%d", &joueur->niveau);
    fscanf(f, "%d", &joueur->temps_jeu);

    Personnage perso;
    init_personnage4(&perso);
    fscanf(f, "%d %d", &perso.x_monde, &perso.y);
    fscanf(f, "%d %d %d %d", &perso.vx, &perso.vy, &perso.current_frame, &perso.saute);
    int scroll_x;
    fscanf(f, "%d", &scroll_x);

    int bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif;
    int bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours;
    int debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit;
    double taille_facteur;
    int vivants[3];
    int nombre_oiseaux;


    fscanf(f, "%d %d %d %d", &bonus_vitesse_actif, &bonus_demulti_actif, &bonus_taille_actif, &malus_petit_actif);
    fscanf(f, "%d %d %d %d", &bonus_vitesse_en_cours, &bonus_demulti_en_cours, &bonus_taille_en_cours, &malus_petit_en_cours);
    fscanf(f, "%d %d %d %d", &debut_bonus_vitesse, &debut_bonus_demulti, &debut_bonus_taille, &debut_malus_petit);
    fscanf(f, "%lf", &taille_facteur);
    fscanf(f, "%d %d %d", &vivants[0], &vivants[1], &vivants[2]);

    fscanf(f, "%d", &nombre_oiseaux);

    fscanf(f, "%d", &perso.vitesse);

    fclose(f);






    perso.compteur_frame = 0;
    perso.delai_frame = 5;

    BITMAP *page = create_bitmap(800, 600);
    BITMAP *fond = load_bitmap("fond_nuit.bmp", NULL);
    BITMAP *bonus_vitesse = load_bitmap("vitesse+.bmp", NULL);
    BITMAP *bonus_demulti = load_bitmap("demulti.bmp", NULL);
    BITMAP *bonus_taille = load_bitmap("taille+.bmp", NULL);
    BITMAP *malus_petit = load_bitmap("taille-.bmp", NULL);

    if (!page || !fond || !bonus_vitesse || !bonus_demulti || !bonus_taille || !malus_petit) {
        allegro_message("Erreur chargement bitmaps");
        return 1;
    }

    init_scies4();
    init_piquants4();
    init_stalactites4();
    init_diagonales4();
    init_pics4();
    init_noirs4();

    int timer = joueur->temps_jeu;
    int x_bonus_vitesse = 900, y_bonus_vitesse = 340;
    int x_bonus_demulti = 1285, y_bonus_demulti = 340;
    int x_bonus_taille = 1600, y_bonus_taille = 340;
    int x_malus_petit = 1900, y_malus_petit = 340;

    int pause_bonus_vitesse = 0, pause_bonus_demulti = 0;
    int pause_bonus_taille = 0, pause_malus_petit = 0;

    int debut = clock();


    if (bonus_vitesse_en_cours) debut_bonus_vitesse = debut;
    if (bonus_demulti_en_cours) debut_bonus_demulti = debut;
    if (bonus_taille_en_cours)  debut_bonus_taille = debut;
    if (malus_petit_en_cours)   debut_malus_petit = debut;

    pause_bonus_vitesse = 0;
    pause_bonus_demulti = 0;
    pause_bonus_taille = 0;
    pause_malus_petit = 0;


    int collision = 0;
    int fin_partie = 0;
    int temps_pause_total = 0;
    const int duree_bonus = 10000;



    while (!key[KEY_ESC] && !fin_partie) {
        scroll_x += 2 * perso.vitesse;
        if (scroll_x >= fond->w) scroll_x = 0;

        clear_bitmap(page);
        blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
        blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

        afficher_scies(page, scroll_x);
        mettre_a_jour_piquants();
        afficher_piquants(page, scroll_x);
        afficher_stalactites(page, scroll_x);
        afficher_diagonales(page, scroll_x);
        mettre_a_jour_diagonales();
        mettre_a_jour_pics();
        afficher_pics(page, scroll_x);
        afficher_noirs(page, scroll_x);
        mettre_a_jour_noirs4();
        mettre_a_jour_scies();
        mettre_a_jour_stalactites();

        // BONUS VITESSE
        int bonus_vitesse_ecran = x_bonus_vitesse - scroll_x;
        if (bonus_vitesse_actif && bonus_vitesse_ecran >= -50 && bonus_vitesse_ecran <= 850) {
            stretch_sprite(page, bonus_vitesse, bonus_vitesse_ecran, y_bonus_vitesse, 50, 50);
            if (perso.x_monde >= x_bonus_vitesse && perso.x_monde <= x_bonus_vitesse + 50 &&
                perso.y >= y_bonus_vitesse && perso.y <= y_bonus_vitesse + 50) {
                bonus_vitesse_actif = 0;
                perso.vitesse *= 2;
                perso.vx *= 2;
                bonus_vitesse_en_cours = 1;
                debut_bonus_vitesse = clock();
            }
        }

// BONUS DEMULTI
        int bonus_demulti_ecran = x_bonus_demulti - scroll_x;
        if (bonus_demulti_actif && bonus_demulti_ecran >= -50 && bonus_demulti_ecran <= 850) {
            stretch_sprite(page, bonus_demulti, bonus_demulti_ecran, y_bonus_demulti, 50, 50);
            if (perso.x_monde >= x_bonus_demulti && perso.x_monde <= x_bonus_demulti + 50 &&
                perso.y >= y_bonus_demulti && perso.y <= y_bonus_demulti + 50) {
                bonus_demulti_actif = 0;
                nombre_oiseaux = 3;
                vivants[0] = vivants[1] = vivants[2] = 1;
                bonus_demulti_en_cours = 1;
                debut_bonus_demulti = clock();
            }
        }

// BONUS TAILLE
        int bonus_taille_ecran = x_bonus_taille - scroll_x;
        if (bonus_taille_actif && bonus_taille_ecran >= -50 && bonus_taille_ecran <= 850) {
            stretch_sprite(page, bonus_taille, bonus_taille_ecran, y_bonus_taille, 50, 50);
            if (perso.x_monde >= x_bonus_taille && perso.x_monde <= x_bonus_taille + 50 &&
                perso.y >= y_bonus_taille && perso.y <= y_bonus_taille + 50) {
                bonus_taille_actif = 0;
                taille_facteur = 2.0;
                bonus_taille_en_cours = 1;
                debut_bonus_taille = clock();
                perso.taille_facteur = 2.0;

            }
        }

// MALUS PETIT
        int malus_petit_ecran = x_malus_petit - scroll_x;
        if (malus_petit_actif && malus_petit_ecran >= -50 && malus_petit_ecran <= 850) {
            stretch_sprite(page, malus_petit, malus_petit_ecran, y_malus_petit, 50, 50);
            if (perso.x_monde >= x_malus_petit && perso.x_monde <= x_malus_petit + 50 &&
                perso.y >= y_malus_petit && perso.y <= y_malus_petit + 50) {
                malus_petit_actif = 0;
                taille_facteur = 0.5;
                malus_petit_en_cours = 1;
                debut_malus_petit = clock();
            }
        }

        // Expiration des effets bonus/malus
        if (bonus_vitesse_en_cours && clock() - debut_bonus_vitesse - pause_bonus_vitesse >= duree_bonus) {
            perso.vitesse /= 2;
            perso.vx /= 2;
            bonus_vitesse_en_cours = 0;
        }
        if (bonus_demulti_en_cours && clock() - debut_bonus_demulti - pause_bonus_demulti >= duree_bonus) {
            bonus_demulti_en_cours = 0;
            nombre_oiseaux = 1;
            vivants[0] = 1; vivants[1] = 0; vivants[2] = 0;
        }
        if (bonus_taille_en_cours && clock() - debut_bonus_taille - pause_bonus_taille >= duree_bonus) {
            bonus_taille_en_cours = 0;
            taille_facteur = 1.0;
            perso.taille_facteur = 1.0;
        }
        if (malus_petit_en_cours && clock() - debut_malus_petit - pause_malus_petit >= duree_bonus) {
            malus_petit_en_cours = 0;
            taille_facteur = 1.0;
            perso.taille_facteur = 1.0;
        }

        deplacer_personnage(&perso);
        animer_personnage(&perso);

        // --- PAUSE ---
        if (key[KEY_P]) {
            int debut_pause = clock();

            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "PAUSE", 360, 200, makecol(255, 255, 0), -1);
            textout_ex(page, font, "1. Continuer", 330, 240, makecol(255, 255, 255), -1);
            textout_ex(page, font, "2. Enregistrer et quitter", 330, 270, makecol(255, 255, 255), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;
                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;
                    break;
                }

                if (key[KEY_2]) {
                    int duree_pause = clock() - debut_pause;
                    temps_pause_total += duree_pause;

                    if (bonus_vitesse_en_cours) pause_bonus_vitesse += duree_pause;
                    if (bonus_demulti_en_cours) pause_bonus_demulti += duree_pause;
                    if (bonus_taille_en_cours) pause_bonus_taille += duree_pause;
                    if (malus_petit_en_cours) pause_malus_petit += duree_pause;

                    if (bonus_vitesse_en_cours) debut_bonus_vitesse += pause_bonus_vitesse;
                    if (bonus_demulti_en_cours) debut_bonus_demulti += pause_bonus_demulti;
                    if (bonus_taille_en_cours) debut_bonus_taille += pause_bonus_taille;
                    if (malus_petit_en_cours) debut_malus_petit += pause_malus_petit;




                    temps_pause_total += clock() - debut_pause;
                    joueur->temps_jeu = timer - (clock() - debut - temps_pause_total);
                    sauvegarder_partie_niveau4(joueur, &perso, scroll_x,
                                               bonus_vitesse_actif, bonus_demulti_actif, bonus_taille_actif, malus_petit_actif,
                                               bonus_vitesse_en_cours, bonus_demulti_en_cours, bonus_taille_en_cours, malus_petit_en_cours,
                                               debut_bonus_vitesse, debut_bonus_demulti, debut_bonus_taille, debut_malus_petit,
                                               taille_facteur, vivants, nombre_oiseaux);
                    clear_keybuf();
                    // destruction des objets graphiques
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    detruire_diagonales();
                    detruire_pics();
                    detruire_noirs();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    rest(200);
                    return 0;
                }
            }
        }


        int old_x = perso.x_monde;
        int old_y = perso.y;

        if (nombre_oiseaux == 1) {
            int x_ecran = perso.x_monde - scroll_x;
            if (taille_facteur == 1.0) {
                afficher_personnage(&perso, page, scroll_x);
            } else {
                stretch_sprite(page, perso.image[perso.current_frame],
                               (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                               (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                               (int)(perso.image[0]->w * taille_facteur),
                               (int)(perso.image[0]->h * taille_facteur));
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                if (i == 1) { perso.x_monde = old_x + 50; perso.y = old_y - 50; }
                if (i == 2) { perso.x_monde = old_x + 50; perso.y = old_y + 50; }
                int x_ecran = perso.x_monde - scroll_x;
                if (taille_facteur == 1.0) {
                    afficher_personnage(&perso, page, scroll_x);
                } else {
                    stretch_sprite(page, perso.image[perso.current_frame],
                                   (int)(x_ecran - perso.image[0]->w * taille_facteur / 2),
                                   (int)(perso.y - perso.image[0]->h * taille_facteur / 2),
                                   (int)(perso.image[0]->w * taille_facteur),
                                   (int)(perso.image[0]->h * taille_facteur));
                }
            }
            perso.x_monde = old_x;
            perso.y = old_y;
        }

        int rayon_collision = (int)(perso.image[0]->w * taille_facteur / 3);

        if (nombre_oiseaux == 1) {
            if (collision_pixel_noir(page, perso.x_monde, perso.y, rayon_collision)) {
                collision = 1;
            }
        } else {
            for (int i = 0; i < 3; i++) {
                if (!vivants[i]) continue;
                int cx = old_x, cy = old_y;
                if (i == 1) { cx += 30; cy -= 30; }
                if (i == 2) { cx += 30; cy += 30; }
                if (collision_pixel_noir(page, cx, cy, rayon_collision)) {
                    vivants[i] = 0;
                }
            }
            int vivants_restants = 0;
            for (int i = 0; i < 3; i++) if (vivants[i]) vivants_restants++;
            nombre_oiseaux = vivants_restants;
            if (nombre_oiseaux == 0) collision = 1;
        }

        // Collision pixel noir
        int x_ecran = perso.x_monde - scroll_x;
        int x_collision = x_ecran;  // Coordonnées exactes de la collision
        int y_collision = perso.y;
        // Rayon de collision simplifié (petit rectangle autour du personnage)
        int largeur_collision = perso.image[0]->w / 3;
        int hauteur_collision = perso.image[0]->h;


        // Collision pixel noir (zone restreinte autour du personnage)
        collision = 0; // Reset de la variable collision

        for (int dx = 0; dx < largeur_collision; dx++) {
            for (int dy = 0; dy < hauteur_collision; dy++) {
                if (collision_pixel_noir(page, x_collision + dx, y_collision + dy, 1)) {
                    collision = 1;
                    x_collision += dx;
                    y_collision += dy;
                    break;
                }
            }
            if (collision) break;
        }


        if (collision) {
            while (perso.y < 580) {
                perso.vy += 1;
                perso.y += perso.vy;
                clear_bitmap(page);
                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);
                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }
            clear_to_color(page, makecol(0, 0, 0));
            textout_ex(page, font, "GAME OVER", 330, 220, makecol(255, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);
            rest(1000);
            break;
        }

        int cible_x = 3900; // Ajuste cette valeur à la fin de ton niveau


// Nouvelle condition pour déclencher l'animation de fin
        if (perso.x_monde >= cible_x) {
            int cible_y = 300;
            double vitesse_y = 5.0;
            int compteur_frame = 0;

            perso.current_frame = 0;

            // Phase 1 : mouvement vertical vers le centre
            while (perso.y != cible_y) {
                if (perso.y < cible_y) {
                    perso.y += (int)vitesse_y;
                    if (perso.y > cible_y) perso.y = cible_y;
                } else if (perso.y > cible_y) {
                    perso.y -= (int)vitesse_y;
                    if (perso.y < cible_y) perso.y = cible_y;
                }

                compteur_frame++;
                if (compteur_frame >= 25) {
                    perso.current_frame = (perso.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Phase 2 : déplacement horizontal accéléré
            int cible_x_fin = perso.x_monde + 800;
            double vitesse_anim = 1.0;
            clock_t last_accel_time = clock();
            perso.saute = 1;

            while (perso.x_monde < cible_x_fin) {
                if ((clock() - last_accel_time) >= 100) {
                    vitesse_anim += 0.8 ;
                    last_accel_time = clock();
                }

                compteur_frame++;
                if (compteur_frame >= 13) {
                    perso.current_frame = (perso.current_frame + 1) % 4;
                    compteur_frame = 0;
                }

                perso.x_monde += (int)vitesse_anim;

                blit(fond, page, scroll_x, 0, 0, 0, fond->w - scroll_x, 600);
                blit(fond, page, 0, 0, fond->w - scroll_x, 0, scroll_x, 600);

                afficher_scies(page, scroll_x);
                afficher_piquants(page, scroll_x);
                afficher_stalactites(page, scroll_x);
                afficher_diagonales(page, scroll_x);
                afficher_pics(page, scroll_x);
                afficher_noirs(page, scroll_x);

                afficher_personnage(&perso, page, scroll_x);
                blit(page, screen, 0, 0, 0, 0, 800, 600);
                rest(20);
            }

            // Message de fin
            clear_to_color(page, makecol(0, 255, 0));
            textout_ex(page, font, "Niveau terminé !", 300, 200, makecol(0, 0, 0), -1);
            textout_ex(page, font, "1. Rejouer", 300, 230, makecol(0, 0, 0), -1);
            textout_ex(page, font, "2. Retour menu", 300, 260, makecol(0, 0, 0), -1);
            blit(page, screen, 0, 0, 0, 0, 800, 600);

            while (1) {
                if (key[KEY_1]) {
                    rest(200);
                    fin_partie = 1;
                    rejouer = 1;
                    break;
                }
                if (key[KEY_2]) {
                    rest(200);
                    detruire_scies();
                    detruire_piquants();
                    detruire_stalactites();
                    destroy_bitmap(page);
                    destroy_bitmap(fond);
                    return 0;
                }
            }
            break;
        }



        // --- JAUGES ---
        int xj = 570, yj = 20, hj = 20, lj = 200;
        if (bonus_vitesse_en_cours) {
            int t = clock() - debut_bonus_vitesse - pause_bonus_vitesse;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,0,0));
            yj += 25;
        }
        if (bonus_demulti_en_cours) {
            int t = clock() - debut_bonus_demulti - pause_bonus_demulti;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,0,255));
            yj += 25;
        }
        if (bonus_taille_en_cours) {
            int t = clock() - debut_bonus_taille - pause_bonus_taille;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(0,255,0));
            yj += 25;
        }
        if (malus_petit_en_cours) {
            int t = clock() - debut_malus_petit - pause_malus_petit;
            int w = lj * (duree_bonus - t) / duree_bonus;
            if (w < 0) w = 0;
            rect(page, xj, yj, xj + lj, yj + hj, makecol(255,255,255));
            rectfill(page, xj+1, yj+1, xj+w-1, yj+hj-1, makecol(255,255,0));
            yj += 25;
        }


        blit(page, screen, 0, 0, 0, 0, 800, 600);
        rest(20);
    }

    detruire_scies();
    detruire_piquants();
    detruire_stalactites();
    detruire_diagonales();
    detruire_pics();
    detruire_noirs();
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(bonus_vitesse);
    destroy_bitmap(bonus_demulti);
    destroy_bitmap(bonus_taille);
    destroy_bitmap(malus_petit);

    return 0;
}
