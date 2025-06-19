#ifndef JEU_H
#define JEU_H
#include "joueur.h"
#include "personnage.h"


int lancer_jeu(Joueur *joueur);
int jouer_niveau(Joueur *joueur);
int niveau1(Joueur *joueur);
int niveau2(Joueur *joueur);
int niveau3(Joueur *joueur);
int niveau4(Joueur *joueur);

int reprendre_niveau1(Joueur *joueur);
int reprendre_niveau2(Joueur *joueur);
int reprendre_niveau3(Joueur *joueur);
int reprendre_niveau4(Joueur *joueur);

void niveau_intro(int num_niveau);  // animation de démarrage
void animer_intro(Personnage *perso, BITMAP *fond, BITMAP *page, int num_niveau);








#endif //JEU_H
