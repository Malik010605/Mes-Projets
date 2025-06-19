#ifndef DECOR_H
#define DECOR_H

#include <allegro.h>
#include "personnage.h"

#define NB_NOIRS 150
#define NB_DIAGONALES 10
#define NB_STALACTITES 5
#define NB_SCIES 5
#define NB_PIQUANTS 6
#define NB_PICS 6
#define NB_PORTAILS 4  // tu peux l’augmenter plus tard
#define NB_LAVES 200 // ou autre nombre selon le besoin
#define NOMBRE_CANONS 2
#define NB_GRIS 50

typedef struct {
    int x, y;
    int actif;
    int frame_actuelle;
    int delai_frame;
    int compteur_frame;
} Lave;

extern Lave laves[NB_LAVES];
extern BITMAP *lave_img[4];


typedef struct {
    int x, y;
    int actif;
} Portail;


// Structures
typedef struct {
    int x, y;
    int actif;
} Pic;

typedef struct {
    int x, y;
    int actif;
} Piquants;

typedef struct {
    int x, y;
    int actif;
    int direction_y;
    int type;     // 1 = statique, 2 = vertical, 3 = clignotant
    int visible;  // 0 ou 1
    int intervalle; // en ms, temps entre deux changements d'état
    int dernier_changement; // temps du dernier changement (clock)
} Noir;


typedef struct {
    int x, y;
    int actif;
    int type_img;
} Diagonale;

typedef struct {
    int x, y;
    int actif;
} Stalactite;

typedef struct {
    int x, y;
    int actif;
} Scie;

// Fond
typedef struct {
    int scroll_x;
} Monde;

typedef struct {
    int x, y;
    int actif;
} BlocGris;

// Structure pour le canon
typedef struct {
    int x, y;
    int actif;
    int last_shot_time; // Temps du dernier tir
} Canon;

// Structure pour les lasers
typedef struct {
    int x, y;
    int actif;
} Laser;

void init_decor();
void afficher_decor(BITMAP *page);
void detruire_decor();

// Scies
void init_scies();
void mettre_a_jour_scies();
void afficher_scies(BITMAP *page, int scroll_x);
void detruire_scies();
void init_scies2();
void init_scies3();
void init_scies4();

// Piquants
void init_piquants();
void mettre_a_jour_piquants();
void afficher_piquants(BITMAP *page, int scroll_x);
void detruire_piquants();
void init_piquants2();
void init_piquants3();
void init_piquants4();
void init_piquants_animation();



// Stalactites
void init_stalactites();
void mettre_a_jour_stalactites();
void afficher_stalactites(BITMAP *page, int scroll_x);
void detruire_stalactites();
void init_stalactites2();
void init_stalactites3();
void init_stalactites4();

// Noirs
void init_noirs();
void mettre_a_jour_noirs();
void mettre_a_jour_noirs3();
void mettre_a_jour_noirs4();
void afficher_noirs(BITMAP *page, int scroll_x);
void detruire_noirs();
void init_noirs2();
void init_noirs3();
void init_noirs4();
void init_noirs_intro();
void init_noirs_animation();

// Diagonales
void init_diagonales();
void mettre_a_jour_diagonales();
void afficher_diagonales(BITMAP *page, int scroll_x);
void detruire_diagonales();
void init_diagonales2();
void init_diagonales3();
void init_diagonales4();

// Pics
void init_pics();
void mettre_a_jour_pics();
void afficher_pics(BITMAP *page, int scroll_x);
void detruire_pics();
void init_pics2();
void init_pics3();
void init_pics4();

void init_portails();
void afficher_portails(BITMAP *page, int scroll_x);
void mettre_a_jour_portails();
void detruire_portails();


void init_portails2();
void afficher_portails2(BITMAP *page, int scroll_x);
void mettre_a_jour_portails2();
void detruire_portails2();

// Laves
void init_laves();
void mettre_a_jour_laves();
void afficher_laves(BITMAP *page, int scroll_x);
void detruire_laves();


// Pixel noir (fond)
int collision_pixel_noir(BITMAP* page, int x, int y, int rayon);

// Pixels gris
extern BlocGris gris[NB_GRIS];
extern BITMAP *gris_img;
void init_gris();
void afficher_gris(BITMAP *page, int scroll_x);
void detruire_gris();
int collision_bloc_gris(Personnage *perso, int scroll_x);


Canon canons[NOMBRE_CANONS];
// Fonctions pour gérer le canon
void init_canons(Canon canons[]);
void afficher_canons(BITMAP *page, Canon canons[], int scroll_x);
void mettre_a_jour_canons(Canon canons[]);
void detruire_canons(Canon canons[]);



#endif
