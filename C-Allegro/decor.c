
#include "decor.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>

#define VITESSE_SCROLL 2
#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600

#define GODMODE 1  //pour pouvoir construire le jeux sans mourrir


// Fond
BITMAP *decor = NULL;
int scroll_x = 0;

void init_decor() {
    decor = load_bitmap("decor.bmp", NULL);
    if (!decor) {
        allegro_message("Erreur chargement decor.bmp");
        exit(EXIT_FAILURE);
    }
}

void afficher_decor(BITMAP *page) {
    scroll_x += VITESSE_SCROLL;
    if (scroll_x >= decor->w - LARGEUR_ECRAN) scroll_x = 0;

    blit(decor, page, scroll_x, 0, 0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN);
}

void detruire_decor() {
    if (decor) destroy_bitmap(decor);
}

// noirs
BITMAP *noir_img = NULL;
Noir noirs[NB_NOIRS];

void init_noirs() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }

    noirs[0].x = 1285; noirs[0].y = 500; noirs[0].actif = 1;
    noirs[1].x = 1659; noirs[1].y = 500; noirs[1].actif = 1;
    noirs[2].x = 2033; noirs[2].y = 500; noirs[2].actif = 1;
    noirs[3].x = 2200; noirs[3].y = 500; noirs[3].actif = 1;
    noirs[4].x = 2200; noirs[4].y = 400; noirs[4].actif = 1;
    noirs[5].x = 2300; noirs[5].y = 500; noirs[5].actif = 1;
    noirs[6].x = 2300; noirs[6].y = 400; noirs[6].actif = 1;
    noirs[7].x = 2400; noirs[7].y = 500; noirs[7].actif = 1;
    noirs[8].x = 2400; noirs[8].y = 400; noirs[8].actif = 1;
    noirs[9].x = 2500; noirs[9].y = 500; noirs[9].actif = 1;
    noirs[10].x = 2500; noirs[10].y = 400; noirs[10].actif = 1;
    noirs[11].x = 2600; noirs[11].y = 500; noirs[11].actif = 1;
    noirs[12].x = 2600; noirs[12].y = 400; noirs[12].actif = 1;
    noirs[13].x = 2700; noirs[13].y = 500; noirs[13].actif = 1;
    noirs[14].x = 2700; noirs[14].y = 400; noirs[14].actif = 1;
    noirs[15].x = 2800; noirs[15].y = 500; noirs[15].actif = 1;
    noirs[16].x = 2800; noirs[16].y = 400; noirs[16].actif = 1;
    noirs[17].x = 2900; noirs[17].y = 500; noirs[17].actif = 1;
    noirs[18].x = 2900; noirs[18].y = 400; noirs[18].actif = 1;

    noirs[19].x = 3100; noirs[19].y = 550; noirs[19].actif = 1;
    noirs[20].x = 3100; noirs[20].y = -50; noirs[20].actif = 1;

    noirs[21].x = 3200; noirs[21].y = 550; noirs[21].actif = 1;
    noirs[22].x = 3200; noirs[22].y = -50; noirs[22].actif = 1;
    noirs[23].x = 3200; noirs[23].y = 450; noirs[23].actif = 1;
    noirs[24].x = 3200; noirs[24].y = 50; noirs[24].actif = 1;

    noirs[25].x = 3300; noirs[25].y = 550; noirs[25].actif = 1;
    noirs[26].x = 3300; noirs[26].y = -50; noirs[26].actif = 1;
    noirs[27].x = 3300; noirs[27].y = 450; noirs[27].actif = 1;
    noirs[28].x = 3300; noirs[28].y = 50; noirs[28].actif = 1;

    noirs[29].x = 3400; noirs[29].y = 550; noirs[29].actif = 1;
    noirs[30].x = 3400; noirs[30].y = -50; noirs[30].actif = 1;
    noirs[31].x = 3400; noirs[31].y = 450; noirs[31].actif = 1;
    noirs[32].x = 3400; noirs[32].y = 50; noirs[32].actif = 1;


    noirs[33].x = 3500; noirs[33].y = 550; noirs[33].actif = 1;
    noirs[34].x = 3500; noirs[34].y = -50; noirs[34].actif = 1;


    int index = 35;
    int x_depart = 4000;
    for (int bloc = 0; bloc < 10; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1;
        x_depart += 100;




    }

}

void init_noirs2() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }


    noirs[61].x = 500; noirs[61].y = 400; noirs[61].actif = 1;
    noirs[62].x = 650; noirs[62].y = 100; noirs[62].actif = 1;

    noirs[63].x = 850; noirs[63].y = 400; noirs[63].actif = 1;
    noirs[64].x = 1050; noirs[64].y = 100; noirs[64].actif = 1;
    noirs[65].x = 850; noirs[65].y = 350; noirs[65].actif = 1;
    noirs[66].x = 1050; noirs[66].y = 150; noirs[66].actif = 1;

    noirs[67].x = 1800; noirs[67].y = 400; noirs[67].actif = 1;
    noirs[68].x = 1800; noirs[68].y = 300; noirs[68].actif = 1;
    noirs[69].x = 1800; noirs[69].y = 250; noirs[69].actif = 1;

    noirs[70].x = 2000; noirs[70].y = 100; noirs[70].actif = 1;
    noirs[71].x = 2000; noirs[71].y = 200; noirs[71].actif = 1;
    noirs[72].x = 2000; noirs[72].y = 300; noirs[72].actif = 1;
    noirs[73].x = 2000; noirs[73].y = 400; noirs[73].actif = 1;
    noirs[74].x = 2100; noirs[74].y = 400; noirs[74].actif = 1;

    noirs[75].x = 3400; noirs[75].y = 500; noirs[75].actif = 1;
    noirs[76].x = 3450; noirs[76].y = 500; noirs[76].actif = 1;
    noirs[77].x = 3650; noirs[77].y = 400; noirs[77].actif = 1;

    int index = 0;
    int x_depart = 4000;
    for (int bloc = 0; bloc < 10; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1;
        x_depart += 100;




    }

}

void init_noirs3() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }

    noirs[0].x = 1285; noirs[0].y = 500; noirs[0].actif = 1;
    noirs[1].x = 1659; noirs[1].y = 500; noirs[1].actif = 1;
    noirs[2].x = 2033; noirs[2].y = 500; noirs[2].actif = 1;
    noirs[3].x = 2200; noirs[3].y = 500; noirs[3].actif = 1;
    noirs[4].x = 2200; noirs[4].y = 400; noirs[4].actif = 1;
    noirs[5].x = 2300; noirs[5].y = 500; noirs[5].actif = 1;
    noirs[6].x = 2300; noirs[6].y = 400; noirs[6].actif = 1;
    noirs[7].x = 2400; noirs[7].y = 500; noirs[7].actif = 1;
    noirs[8].x = 2400; noirs[8].y = 400; noirs[8].actif = 1;
    noirs[9].x = 2500; noirs[9].y = 500; noirs[9].actif = 1;
    noirs[10].x = 2500; noirs[10].y = 400; noirs[10].actif = 1;
    noirs[11].x = 2600; noirs[11].y = 500; noirs[11].actif = 1;
    noirs[12].x = 2600; noirs[12].y = 400; noirs[12].actif = 1;
    noirs[13].x = 2700; noirs[13].y = 500; noirs[13].actif = 1;
    noirs[14].x = 2700; noirs[14].y = 400; noirs[14].actif = 1;
    noirs[15].x = 2800; noirs[15].y = 500; noirs[15].actif = 1;
    noirs[16].x = 2800; noirs[16].y = 400; noirs[16].actif = 1;
    noirs[17].x = 2900; noirs[17].y = 500; noirs[17].actif = 1;
    noirs[18].x = 2900; noirs[18].y = 400; noirs[18].actif = 1;
    noirs[19].x = 3500; noirs[19].y = 200; noirs[19].actif = 1;
    noirs[20].x = 3500; noirs[20].y = 300; noirs[20].actif = 1;
    noirs[21].x = 3500; noirs[21].y = 400; noirs[21].actif = 1;
    noirs[22].x = 3500; noirs[22].y = 500; noirs[22].actif = 1;
    noirs[23].x = 3500; noirs[23].y = 600; noirs[23].actif = 1;
    noirs[24].x = 3500; noirs[24].y = 100; noirs[24].actif = 1;



    int index = 30;
    int x_depart = 4000;
    for (int bloc = 0; bloc < 10; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1; noirs[0].type = 1;
        x_depart += 100;
    }

}


void init_noirs4() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);
    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }
    noirs[0].x = 600; noirs[0].y = 250; noirs[0].actif = 1; noirs[0].type = 1;
    noirs[2].x = 700; noirs[2].y = 250; noirs[2].actif = 1; noirs[2].type = 1;
    noirs[1].x = 800; noirs[1].y = 250; noirs[1].actif = 1; noirs[1].type = 1;
    noirs[3].x = 900; noirs[3].y = 250; noirs[3].actif = 1; noirs[3].type = 1;
    noirs[4].x =  1000; noirs[4].y = 250; noirs[4].actif = 1; noirs[4].type = 1;
    noirs[5].x = 1100; noirs[5].y = 250; noirs[5].actif = 1; noirs[5].type = 1;
    noirs[6].x = 1200; noirs[6].y = 250; noirs[6].actif = 1; noirs[6].type = 1;
    noirs[7].x = 1300; noirs[7].y = 250; noirs[7].actif = 1; noirs[7].type = 1;
    noirs[8].x = 1400; noirs[8].y = 250; noirs[8].actif = 1; noirs[8].type = 1;
    noirs[9].x = 1500; noirs[9].y = 250; noirs[9].actif = 1; noirs[9].type = 1;
    noirs[10].x = 1600; noirs[10].y = 250; noirs[10].actif = 1; noirs[10].type = 1;
    noirs[13].x = 1700; noirs[13].y = 250; noirs[13].actif = 1; noirs[13].type = 1;
    noirs[14].x = 1750; noirs[14].y = 250; noirs[14].actif = 1; noirs[14].type = 1;

    noirs[11].x = 985; noirs[11].y = 500; noirs[11].actif = 1; noirs[11].type = 1;
    noirs[12].x = 1359; noirs[12].y = 500; noirs[12].actif = 1; noirs[12].type = 1;

    noirs[57].x = 985; noirs[57].y = 0; noirs[57].actif = 1; noirs[57].type = 1;
    noirs[58].x = 1359; noirs[58].y = 0; noirs[58].actif = 1; noirs[58].type = 1;

    noirs[15].x = 2000; noirs[15].y = 0; noirs[15].actif = 1; noirs[15].type = 1;
    noirs[16].x = 2100; noirs[16].y = 0; noirs[16].actif = 1; noirs[16].type = 1;
    noirs[17].x = 2200; noirs[17].y = 0; noirs[17].actif = 1; noirs[17].type = 1;
    noirs[18].x = 2300; noirs[18].y = 0; noirs[18].actif = 1; noirs[18].type = 1;
    noirs[19].x = 2400; noirs[19].y = 0; noirs[19].actif = 1; noirs[19].type = 1;
    noirs[20].x = 2500; noirs[20].y = 0; noirs[20].actif = 1; noirs[20].type = 1;
    noirs[21].x = 2600; noirs[21].y = 0; noirs[21].actif = 1; noirs[21].type = 1;
    noirs[22].x = 2700; noirs[22].y = 0; noirs[22].actif = 1; noirs[22].type = 1;
    noirs[38].x = 2800; noirs[38].y = 0; noirs[38].actif = 1; noirs[38].type = 1;

    noirs[23].x = 2000; noirs[23].y = 500; noirs[23].actif = 1; noirs[23].type = 1;
    noirs[24].x = 2100; noirs[24].y = 500; noirs[24].actif = 1; noirs[24].type = 1;
    noirs[25].x = 2200; noirs[25].y = 500; noirs[25].actif = 1; noirs[25].type = 1;
    noirs[26].x = 2300; noirs[26].y = 500; noirs[26].actif = 1; noirs[26].type = 1;
    noirs[27].x = 2400; noirs[27].y = 500; noirs[27].actif = 1; noirs[27].type = 1;
    noirs[28].x = 2500; noirs[28].y = 500; noirs[28].actif = 1; noirs[28].type = 1;
    noirs[29].x = 2600; noirs[29].y = 500; noirs[29].actif = 1; noirs[29].type = 1;
    noirs[30].x = 2700; noirs[30].y = 500; noirs[30].actif = 1; noirs[30].type = 1;
    noirs[39].x = 2800; noirs[39].y = 500; noirs[39].actif = 1; noirs[39].type = 1;

    noirs[31].x = 2000; noirs[31].y = 400; noirs[31].actif = 1; noirs[31].type = 2;
    noirs[32].x = 2100; noirs[32].y = 400; noirs[32].actif = 1; noirs[32].type = 2;
    noirs[33].x = 2200; noirs[33].y = 200; noirs[33].actif = 1; noirs[33].type = 2;
    noirs[34].x = 2400; noirs[34].y = 400; noirs[34].actif = 1; noirs[34].type = 2;
    noirs[35].x = 2600; noirs[35].y = 400; noirs[35].actif = 1; noirs[35].type = 2;
    noirs[36].x = 2700; noirs[36].y = 300; noirs[36].actif = 1; noirs[36].type = 2;
    noirs[37].x = 2800; noirs[37].y = 180; noirs[37].actif = 1; noirs[37].type = 2;

    noirs[40].x = 3100;
    noirs[40].y = 450;
    noirs[40].actif = 1;
    noirs[40].type = 3;
    noirs[40].visible = 0;
    noirs[40].intervalle = 2870; // clignote plus vite
    noirs[40].dernier_changement = clock();

    noirs[41].x = 3300;
    noirs[41].y = 0;
    noirs[41].actif = 1;
    noirs[41].type = 3;
    noirs[41].visible = 1;  // invisible au début
    noirs[41].intervalle = 2870;
    noirs[41].dernier_changement = clock() - (noirs[1].intervalle * CLOCKS_PER_SEC / 1000);  // décalage de demi cycle

    noirs[48].x = 3300;
    noirs[48].y = 100;
    noirs[48].actif = 1;
    noirs[48].type = 3;
    noirs[48].visible = 1;  // invisible au début
    noirs[48].intervalle = 2870;
    noirs[48].dernier_changement = clock() - (noirs[1].intervalle * CLOCKS_PER_SEC / 1000);  // décalage de demi cycle

    noirs[49].x = 3300;
    noirs[49].y = 200;
    noirs[49].actif = 1;
    noirs[49].type = 3;
    noirs[49].visible = 1;  // invisible au début
    noirs[49].intervalle = 2870;
    noirs[49].dernier_changement = clock() - (noirs[1].intervalle * CLOCKS_PER_SEC / 1000);  // décalage de demi cycle

    noirs[53].x = 3300;
    noirs[53].y = -50;
    noirs[53].actif = 1;
    noirs[53].type = 3;
    noirs[53].visible = 1;  // invisible au début
    noirs[53].intervalle = 2870;
    noirs[53].dernier_changement = clock() - (noirs[1].intervalle * CLOCKS_PER_SEC / 1000);  // décalage de demi cycle

    noirs[50].x = 3500;
    noirs[50].y = 150;
    noirs[50].actif = 1;
    noirs[50].type = 3;
    noirs[50].visible = 0;
    noirs[50].intervalle = 2870; // clignote plus vite
    noirs[50].dernier_changement = clock();

    noirs[51].x = 3500;
    noirs[51].y = 350;
    noirs[51].actif = 1;
    noirs[51].type = 3;
    noirs[51].visible = 0;
    noirs[51].intervalle = 2870; // clignote plus vite
    noirs[51].dernier_changement = clock();

    noirs[52].x = 3500;
    noirs[52].y = 250;
    noirs[52].actif = 1;
    noirs[52].type = 3;
    noirs[52].visible = 0;
    noirs[52].intervalle = 2870; // clignote plus vite
    noirs[52].dernier_changement = clock();


    noirs[42].x = 3500;
    noirs[42].y = 450;
    noirs[42].actif = 1;
    noirs[42].type = 3;
    noirs[42].visible = 0;
    noirs[42].intervalle = 2870; // clignote plus vite
    noirs[42].dernier_changement = clock();

    noirs[43].x = 3500;
    noirs[43].y = 50;
    noirs[43].actif = 1;
    noirs[43].type = 3;
    noirs[43].visible = 0;  // invisible au début
    noirs[43].intervalle = 2870;
    noirs[43].dernier_changement = clock();

    noirs[44].x = 3100;
    noirs[44].y = -50;
    noirs[44].actif = 1;
    noirs[44].type = 3;
    noirs[44].visible = 0;
    noirs[44].intervalle = 2870; // clignote plus vite
    noirs[44].dernier_changement = clock();

    noirs[45].x = 3100;
    noirs[45].y = 50;
    noirs[45].actif = 1;
    noirs[45].type = 3;
    noirs[45].visible = 0;
    noirs[45].intervalle = 2870; // clignote plus vite
    noirs[45].dernier_changement = clock();

    noirs[46].x = 3100;
    noirs[46].y = 500;
    noirs[46].actif = 1;
    noirs[46].type = 3;
    noirs[46].visible = 0;
    noirs[46].intervalle = 2870; // clignote plus vite
    noirs[46].dernier_changement = clock();

    noirs[47].x = 3100;
    noirs[47].y = 400;
    noirs[47].actif = 1;
    noirs[47].type = 3;
    noirs[47].visible = 0;
    noirs[47].intervalle = 2870; // clignote plus vite
    noirs[47].dernier_changement = clock();




    noirs[54].x = 1750; noirs[54].y = 0; noirs[54].actif = 1; noirs[54].type = 1;
    noirs[55].x = 1750; noirs[55].y = 100; noirs[55].actif = 1; noirs[55].type = 1;
    noirs[56].x = 1750; noirs[56].y = 200; noirs[56].actif = 1; noirs[56].type = 1;




    int index = 80;
    int x_depart = 4000;
    for (int bloc = 0; bloc < 10; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1; noirs[0].type = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1; noirs[0].type = 1;
        x_depart += 100;
    }

}

void init_noirs_intro() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }

    int index = 0;
    int x_depart = 0;
    for (int bloc = 0; bloc < 10; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1;
        x_depart += 100;
    }


}

void init_noirs_animation() {
    noir_img = load_bitmap("noir.bmp", NULL);
    if (!noir_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_NOIRS; i++) {
        noirs[i].actif = 0;
        noirs[i].x = 0;
        noirs[i].y = 0;
        noirs[i].direction_y = 1;
        noirs[i].visible = 1;
        noirs[i].type = 1;
        noirs[i].intervalle = 0;
        noirs[i].dernier_changement = 0;
    }
    int index = 0;
    int x_depart = 0;
    for (int bloc = 0; bloc < 2; bloc++) {
        noirs[index++].x = x_depart; noirs[index - 1].y = 550; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = -50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 450; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 50; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 350; noirs[index - 1].actif = 1;
        noirs[index++].x = x_depart; noirs[index - 1].y = 150; noirs[index - 1].actif = 1;
        x_depart += 100;
    }


}





void mettre_a_jour_noirs() {
    for (int i = 0; i < NB_NOIRS; i++) {
        if (noirs[i].actif) {
            if (noirs[i].x + noir_img->w < 0) noirs[i].actif = 0;
        }
    }
}

void mettre_a_jour_noirs3() {
    for (int i = 0; i < NB_NOIRS; i++) {
        if (noirs[i].actif) {

            if (noirs[i].type == 2) {
                // Mouvement vertical
                noirs[i].y += noirs[i].direction_y * 2;

                // Rebond haut/bas
                if (noirs[i].y >= 500) noirs[i].direction_y = -1;
                if (noirs[i].y <= 0) noirs[i].direction_y = 1;
            }

            // Désactivation si hors écran horizontalement (même pour type 1)
            if (noirs[i].x + noir_img->w < 0) noirs[i].actif = 0;
        }
    }
}



void mettre_a_jour_noirs4() {
    int maintenant = clock();

    for (int i = 0; i < NB_NOIRS; i++) {
        if (noirs[i].actif) {

            if (noirs[i].type == 2) {
                // Mouvement vertical
                noirs[i].y += noirs[i].direction_y * 2;

                // Rebond haut/bas
                if (noirs[i].y >= 400) noirs[i].direction_y = -1;
                if (noirs[i].y <= 100) noirs[i].direction_y = 1;
            }

            if (noirs[i].type == 3) {
                int temps_ecoule = (maintenant - noirs[i].dernier_changement) * 1000 / CLOCKS_PER_SEC;
                if (temps_ecoule >= noirs[i].intervalle) {
                    noirs[i].visible = !noirs[i].visible;
                    noirs[i].dernier_changement = maintenant;
                }
            }



            // Désactivation si hors écran horizontalement (même pour type 1)
            if (noirs[i].x + noir_img->w < 0) noirs[i].actif = 0;
        }
    }
}


void afficher_noirs(BITMAP *page, int scroll_x) {

    for (int i = 0; i < NB_NOIRS; i++) {
        if (!noirs[i].actif || !noirs[i].visible) continue;

        int x_affiche = noirs[i].x - scroll_x;
        int y_affiche = noirs[i].y;

        if (x_affiche >= -noir_img->w && x_affiche <= LARGEUR_ECRAN) {
            masked_blit(noir_img, page, 0, 0, x_affiche, y_affiche, noir_img->w, noir_img->h);
        }
    }
}


void detruire_noirs() {
    if (noir_img) destroy_bitmap(noir_img);
}



// diagonales
BITMAP *diagonale_img = NULL;
BITMAP *diagonale2_img = NULL;
BITMAP *diagonale3_img = NULL;
BITMAP *diagonale4_img = NULL;

Diagonale diagonales[NB_DIAGONALES];

void init_diagonales() {
    diagonale_img     = load_bitmap("diagonale.bmp", NULL);
    diagonale2_img    = load_bitmap("diagonale2.bmp", NULL);
    diagonale3_img    = load_bitmap("diagonale3.bmp", NULL);
    diagonale4_img    = load_bitmap("diagonale4.bmp", NULL);

    if (!diagonale_img || !diagonale2_img || !diagonale3_img || !diagonale4_img) {
        allegro_message("Erreur chargement des images diagonales.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_DIAGONALES; i++) {
        diagonales[i].x = 0;
        diagonales[i].y = 0;
        diagonales[i].actif = 0;
    }



    diagonales[0] = (Diagonale){3000, 550, 1, 0};
    diagonales[1] = (Diagonale){3000, -50, 1, 3};
    diagonales[2] = (Diagonale){3100, 450, 1, 0};
    diagonales[3] = (Diagonale){3100, 50, 1, 3};

    diagonales[4] = (Diagonale){3600, 550, 1, 1};
    diagonales[5] = (Diagonale){3600, -50, 1, 2};
    diagonales[6] = (Diagonale){3500, 450, 1, 1};
    diagonales[7] = (Diagonale){3500, 50, 1, 2};




}

void init_diagonales2() {
    diagonale_img     = load_bitmap("diagonale.bmp", NULL);
    diagonale2_img    = load_bitmap("diagonale2.bmp", NULL);
    diagonale3_img    = load_bitmap("diagonale3.bmp", NULL);
    diagonale4_img    = load_bitmap("diagonale4.bmp", NULL);

    for (int i = 0; i < NB_DIAGONALES; i++) {
        diagonales[i].x = 0;
        diagonales[i].y = 0;
        diagonales[i].actif = 0;
    }

    if (!diagonale_img || !diagonale2_img || !diagonale3_img || !diagonale4_img) {
        allegro_message("Erreur chargement des images diagonales.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_DIAGONALES; i++) {

        diagonales[0] = (Diagonale){30000, 550, 1, 0};



    }
}

void init_diagonales3() {
    diagonale_img     = load_bitmap("diagonale.bmp", NULL);
    diagonale2_img    = load_bitmap("diagonale2.bmp", NULL);
    diagonale3_img    = load_bitmap("diagonale3.bmp", NULL);
    diagonale4_img    = load_bitmap("diagonale4.bmp", NULL);

    for (int i = 0; i < NB_DIAGONALES; i++) {
        diagonales[i].x = 0;
        diagonales[i].y = 0;
        diagonales[i].actif = 0;
    }

    if (!diagonale_img || !diagonale2_img || !diagonale3_img || !diagonale4_img) {
        allegro_message("Erreur chargement des images diagonales.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_DIAGONALES; i++) {

    }
}


void init_diagonales4() {
    diagonale_img     = load_bitmap("diagonale.bmp", NULL);
    diagonale2_img    = load_bitmap("diagonale2.bmp", NULL);
    diagonale3_img    = load_bitmap("diagonale3.bmp", NULL);
    diagonale4_img    = load_bitmap("diagonale4.bmp", NULL);

    for (int i = 0; i < NB_DIAGONALES; i++) {
        diagonales[i].x = 0;
        diagonales[i].y = 0;
        diagonales[i].actif = 0;
    }

    if (!diagonale_img || !diagonale2_img || !diagonale3_img || !diagonale4_img) {
        allegro_message("Erreur chargement des images diagonales.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_DIAGONALES; i++) {

        diagonales[0] = (Diagonale){30000, 550, 1, 0};
    }
}





void mettre_a_jour_diagonales() {
    for (int i = 0; i < NB_DIAGONALES; i++) {
        if (diagonales[i].actif) {
            if (diagonales[i].x + diagonale_img->w < 0 || diagonales[i].y > HAUTEUR_ECRAN)
                diagonales[i].actif = 0;
        }
    }
}

void afficher_diagonales(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_DIAGONALES; i++) {
        if (!diagonales[i].actif) continue;
        int x_ecran = diagonales[i].x - scroll_x;
        if (x_ecran < -400 || x_ecran > 850) continue;

        BITMAP *img = NULL;
        switch (diagonales[i].type_img) {
            case 0: img = diagonale_img; break;
            case 1: img = diagonale2_img; break;
            case 2: img = diagonale3_img; break;
            case 3: img = diagonale4_img; break;
        }

        if (img)
            draw_sprite(page, img, x_ecran, diagonales[i].y);
    }
}




void detruire_diagonales() {
    if (diagonale_img)    destroy_bitmap(diagonale_img);
    if (diagonale2_img)   destroy_bitmap(diagonale2_img);
    if (diagonale3_img)   destroy_bitmap(diagonale3_img);
    if (diagonale4_img)   destroy_bitmap(diagonale4_img);

}


//stalactite
BITMAP *stalactite_img = NULL;
Stalactite stalactites[NB_STALACTITES];

void init_stalactites() {
    stalactite_img = load_bitmap("stalactite.bmp", NULL);
    if (!stalactite_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_STALACTITES; i++) {
        stalactites[i].x = 0;
        stalactites[i].y = 0;
        stalactites[i].actif = 0;
    }


    stalactites[0].x = 1100; stalactites[0].y = 0; stalactites[0].actif = 1;
    stalactites[1].x = 1450; stalactites[1].y = 0; stalactites[1].actif = 1;
    stalactites[2].x = 1800; stalactites[2].y = 0; stalactites[2].actif = 1;
    stalactites[3].x = 3700; stalactites[3].y = 0; stalactites[3].actif = 1;

}

void init_stalactites2() {
    stalactite_img = load_bitmap("stalactite.bmp", NULL);
    if (!stalactite_img) exit(EXIT_FAILURE);
    for (int i = 0; i < NB_STALACTITES; i++) {
        stalactites[i].x = 0;
        stalactites[i].y = 0;
        stalactites[i].actif = 0;
    }

    stalactites[0].x = 2550; stalactites[0].y = -20; stalactites[0].actif = 1;
    stalactites[1].x = 2750; stalactites[1].y = -20; stalactites[1].actif = 1;


}

void init_stalactites3() {
    stalactite_img = load_bitmap("stalactite.bmp", NULL);
    if (!stalactite_img) exit(EXIT_FAILURE);
    for (int i = 0; i < NB_STALACTITES; i++) {
        stalactites[i].x = 0;
        stalactites[i].y = 0;
        stalactites[i].actif = 0;
    }
    stalactites[0].x = 1100; stalactites[0].y = 0; stalactites[0].actif = 1;
    stalactites[1].x = 300; stalactites[1].y = 0; stalactites[1].actif = 1;

}

void init_stalactites4() {
    stalactite_img = load_bitmap("stalactite.bmp", NULL);
    if (!stalactite_img) exit(EXIT_FAILURE);
    for (int i = 0; i < NB_STALACTITES; i++) {
        stalactites[i].x = 0;
        stalactites[i].y = 0;
        stalactites[i].actif = 0;
    }

    stalactites[0].x = 660; stalactites[0].y = -125; stalactites[0].actif = 1;
    stalactites[1].x = 1450; stalactites[1].y = -125; stalactites[1].actif = 1;
    stalactites[2].x = 1050; stalactites[2].y = -125; stalactites[2].actif = 1;

}





void mettre_a_jour_stalactites() {
    for (int i = 0; i < NB_STALACTITES; i++) {
        if (stalactites[i].actif) {
            if (stalactites[i].x + stalactite_img->w < 0)
                stalactites[i].actif = 0;
        }
    }
}

void afficher_stalactites(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_STALACTITES; i++) {
        if (stalactites[i].actif) {
            int x_ecran = stalactites[i].x - scroll_x;
            if (x_ecran >= -400 && x_ecran <= 850)
                draw_sprite(page, stalactite_img, x_ecran, stalactites[i].y);
        }
    }
}


void detruire_stalactites() {
    if (stalactite_img) destroy_bitmap(stalactite_img);
}



// scies
BITMAP *scie_img = NULL;
BITMAP *scie_redim = NULL;
Scie scies[NB_SCIES];

void init_scies() {
    scie_img = load_bitmap("scie.bmp", NULL);
    if (!scie_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[i].x = 0;
        scies[i].y = 0;
        scies[i].actif = 0;
    }

    int new_w = scie_img->w / 2;
    int new_h = scie_img->h / 2;
    scie_redim = create_bitmap(new_w, new_h);
    stretch_blit(scie_img, scie_redim, 0, 0, scie_img->w, scie_img->h, 0, 0, new_w, new_h);


    scies[0].x = 2350; scies[0].y = -25; scies[0].actif = 1;
    scies[1].x = 2550; scies[1].y = -25; scies[1].actif = 1;
    scies[2].x = 2750; scies[2].y = -25; scies[2].actif = 1;

}

void init_scies2() {
    scie_img = load_bitmap("scie.bmp", NULL);
    if (!scie_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[i].x = 0;
        scies[i].y = 0;
        scies[i].actif = 0;
    }

    int new_w = scie_img->w / 2;
    int new_h = scie_img->h / 2;
    scie_redim = create_bitmap(new_w, new_h);
    stretch_blit(scie_img, scie_redim, 0, 0, scie_img->w, scie_img->h, 0, 0, new_w, new_h);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[0].x = 23500; scies[0].y = -25; scies[0].actif = 1;
    }
}

void init_scies3() {
    scie_img = load_bitmap("scie.bmp", NULL);
    if (!scie_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[i].x = 0;
        scies[i].y = 0;
        scies[i].actif = 0;
    }

    int new_w = scie_img->w / 2;
    int new_h = scie_img->h / 2;
    scie_redim = create_bitmap(new_w, new_h);
    stretch_blit(scie_img, scie_redim, 0, 0, scie_img->w, scie_img->h, 0, 0, new_w, new_h);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[0].x = 2350; scies[0].y = -25; scies[0].actif = 1;
    }
}


void init_scies4() {
    scie_img = load_bitmap("scie.bmp", NULL);
    if (!scie_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[i].x = 0;
        scies[i].y = 0;
        scies[i].actif = 0;
    }

    int new_w = scie_img->w / 2;
    int new_h = scie_img->h / 2;
    scie_redim = create_bitmap(new_w, new_h);
    stretch_blit(scie_img, scie_redim, 0, 0, scie_img->w, scie_img->h, 0, 0, new_w, new_h);

    for (int i = 0; i < NB_SCIES; i++) {
        scies[0].x = 23050; scies[0].y = -25; scies[0].actif = 1;
    }
}

void mettre_a_jour_scies() {
    for (int i = 0; i < NB_SCIES; i++) {
        if (scies[i].actif) {
            if (scies[i].x + scie_redim->w < 0)
                scies[i].actif = 0;
        }
    }
}

void afficher_scies(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_SCIES; i++) {
        if (scies[i].actif) {
            int x_ecran = scies[i].x - scroll_x;
            if (x_ecran >= -400 && x_ecran <= 850)
                masked_blit(scie_redim, page, 0, 0, x_ecran, scies[i].y, scie_redim->w, scie_redim->h);
        }
    }
}

void detruire_scies() {
    if (scie_img) destroy_bitmap(scie_img);
    if (scie_redim) destroy_bitmap(scie_redim);
}


// piquant
BITMAP* img_piquants = NULL;
BITMAP* piquants_redim = NULL;
Piquants piquants[NB_PIQUANTS];

void init_piquants() {
    img_piquants = load_bitmap("piquants.bmp", NULL);
    if (!img_piquants) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_PIQUANTS; i++) {
        piquants[i].x = 0;
        piquants[i].y = 0;
        piquants[i].actif = 0;
    }

    int new_w = img_piquants->w / 2;
    int new_h = img_piquants->h / 2;
    piquants_redim = create_bitmap(new_w, new_h);
    stretch_blit(img_piquants, piquants_redim, 0, 0, img_piquants->w, img_piquants->h, 0, 0, new_w, new_h);


    piquants[0].x = 300; piquants[0].y = 400; piquants[0].actif = 1;
    piquants[1].x = 500; piquants[1].y = 0; piquants[1].actif = 1;
    piquants[2].x = 700; piquants[2].y = -100; piquants[2].actif = 1;
    piquants[3].x = 700; piquants[3].y = 400; piquants[3].actif = 1;
    piquants[4].x = 900; piquants[4].y = 0; piquants[4].actif = 1;
    piquants[5].x = 900; piquants[5].y = 500; piquants[5].actif = 1;


}

void init_piquants2() {
    img_piquants = load_bitmap("piquants.bmp", NULL);
    if (!img_piquants) exit(EXIT_FAILURE);

    // Désactiver toutes les entrées
    for (int i = 0; i < NB_PIQUANTS; i++) {
        piquants[i].actif = 0;
        piquants[i].x = 0;
        piquants[i].y = 0;
    }

    // Redimensionner l'image
    int new_w = img_piquants->w / 2;
    int new_h = img_piquants->h / 2;
    piquants_redim = create_bitmap(new_w, new_h);
    stretch_blit(img_piquants, piquants_redim, 0, 0, img_piquants->w, img_piquants->h, 0, 0, new_w, new_h);

    // Initialiser uniquement les piquants nécessaires pour ce niveau
    piquants[0].x = 1300; piquants[0].y = 330; piquants[0].actif = 1;
    piquants[1].x = 1500; piquants[1].y = 70;  piquants[1].actif = 1;
}


void init_piquants3() {
    img_piquants = load_bitmap("piquants.bmp", NULL);
    if (!img_piquants) exit(EXIT_FAILURE);

    // Désactiver toutes les entrées avant de réinitialiser celles du niveau
    for (int i = 0; i < NB_PIQUANTS; i++) {
        piquants[i].actif = 0;
        piquants[i].x = 0;
        piquants[i].y = 0;
    }

    // Redimensionner l'image
    int new_w = img_piquants->w / 2;
    int new_h = img_piquants->h / 2;
    piquants_redim = create_bitmap(new_w, new_h);
    stretch_blit(img_piquants, piquants_redim, 0, 0, img_piquants->w, img_piquants->h, 0, 0, new_w, new_h);

    // Initialiser les piquants spécifiques à ce niveau
    piquants[0].x = 300;
    piquants[0].y = 400;
    piquants[0].actif = 1;
}



void init_piquants4() {
    img_piquants = load_bitmap("piquants.bmp", NULL);
    if (!img_piquants) exit(EXIT_FAILURE);

    // Désactiver toutes les entrées avant de réinitialiser celles du niveau
    for (int i = 0; i < NB_PIQUANTS; i++) {
        piquants[i].actif = 0;
        piquants[i].x = 0;
        piquants[i].y = 0;
    }

    // Redimensionner l'image
    int new_w = img_piquants->w / 2;
    int new_h = img_piquants->h / 2;
    piquants_redim = create_bitmap(new_w, new_h);
    stretch_blit(img_piquants, piquants_redim, 0, 0, img_piquants->w, img_piquants->h, 0, 0, new_w, new_h);

    // Initialiser les piquants spécifiques au niveau 4
    piquants[0].x = 600;
    piquants[0].y = 500;
    piquants[0].actif = 1;

    piquants[1].x = 1734;
    piquants[1].y = 500;
    piquants[1].actif = 1;

    piquants[2].x = 600;
    piquants[2].y = -100;
    piquants[2].actif = 1;
}


void init_piquants_animation() {
    img_piquants = load_bitmap("piquants1.bmp", NULL);
    if (!img_piquants) exit(EXIT_FAILURE);
    // Désactiver toutes les entrées avant de réinitialiser celles du niveau
    for (int i = 0; i < NB_PIQUANTS; i++) {
        piquants[i].actif = 0;
        piquants[i].x = 0;
        piquants[i].y = 0;
    }

    int new_w = img_piquants->w / 2;
    int new_h = img_piquants->h / 2;
    piquants_redim = create_bitmap(new_w, new_h);
    stretch_blit(img_piquants, piquants_redim, 0, 0, img_piquants->w, img_piquants->h, 0, 0, new_w, new_h);


    piquants[0].x = 300; piquants[0].y = 400; piquants[0].actif = 1;
    piquants[1].x = 500; piquants[1].y = 0; piquants[1].actif = 1;
    piquants[2].x = 700; piquants[2].y = -100; piquants[2].actif = 1;
    piquants[3].x = 700; piquants[3].y = 400; piquants[3].actif = 1;
    piquants[4].x = 900; piquants[4].y = 0; piquants[4].actif = 1;
    piquants[5].x = 900; piquants[5].y = 500; piquants[5].actif = 1;


}



void mettre_a_jour_piquants() {
    for (int i = 0; i < NB_PIQUANTS; i++) {
        if (piquants[i].actif) {
            if (piquants[i].x + piquants_redim->w < 0)
                piquants[i].actif = 0;
        }
    }
}

void afficher_piquants(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_PIQUANTS; i++) {
        if (piquants[i].actif) {
            int x_ecran = piquants[i].x - scroll_x;
            if (x_ecran >= -400 && x_ecran <= 850)
                draw_sprite(page, piquants_redim, x_ecran, piquants[i].y);
        }
    }
}



void detruire_piquants() {
    if (img_piquants) destroy_bitmap(img_piquants);
    if (piquants_redim) destroy_bitmap(piquants_redim);
}

// pics
BITMAP* pic_img = NULL;
Pic pics[NB_PICS];

void init_pics() {
    pic_img = load_bitmap("pic.bmp", NULL);
    if (!pic_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_PICS; i++) {
        pics[i].actif = 0;
        pics[i].x = 0;
        pics[i].y = 0;
    }


    pics[0].x = 1000; pics[0].y = 480; pics[0].actif = 1;
    pics[1].x = 1374; pics[1].y = 480; pics[1].actif = 1;
    pics[2].x = 1748; pics[2].y = 480; pics[2].actif = 1;

    pics[3].x = 3700; pics[3].y = 480; pics[3].actif = 1;

}
void init_pics2() {
    pic_img = load_bitmap("pic.bmp", NULL);
    if (!pic_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_PICS; i++) {
        pics[i].actif = 0;
        pics[i].x = 0;
        pics[i].y = 0;
    }


    pics[0].x = 2200; pics[0].y = 380; pics[0].actif = 1;


}

void init_pics3() {
    pic_img = load_bitmap("pic.bmp", NULL);
    if (!pic_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_PICS; i++) {
        pics[i].actif = 0;
        pics[i].x = 0;
        pics[i].y = 0;
    }


    pics[0].x = 1000; pics[0].y = 480; pics[0].actif = 1;

}

void init_pics4() {
    pic_img = load_bitmap("pic.bmp", NULL);
    if (!pic_img) exit(EXIT_FAILURE);

    for (int i = 0; i < NB_PICS; i++) {
        pics[i].actif = 0;
        pics[i].x = 0;
        pics[i].y = 0;
    }


    pics[0].x = 700; pics[0].y = 480; pics[0].actif = 1;
    pics[1].x = 1074; pics[1].y = 480; pics[1].actif = 1;
    pics[2].x = 1448; pics[2].y = 480; pics[2].actif = 1;

}


void mettre_a_jour_pics() {
    for (int i = 0; i < NB_PICS; i++) {
        if (pics[i].actif) {
            if (pics[i].x + pic_img->w < 0)
                pics[i].actif = 0;
        }
    }
}

void afficher_pics(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_PICS; i++) {
        if (pics[i].actif) {
            int x_ecran = pics[i].x - scroll_x;
            if (x_ecran >= -400 && x_ecran <= 850)
                draw_sprite(page, pic_img, x_ecran, pics[i].y);
        }
    }
}


void detruire_pics() {
    if (pic_img) destroy_bitmap(pic_img);
}


BITMAP *img_portail = NULL;
Portail portails[NB_PORTAILS];

void init_portails() {
    img_portail = load_bitmap("portail.bmp", NULL);
    if (!img_portail) {
        allegro_message("Erreur chargement portail.bmp");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_PORTAILS; i++) {
        portails[i].actif = 0;
        portails[i].x = 0;
        portails[i].y = 0;
    }

    portails[0].x = 1600;
    portails[0].y = 140;
    portails[0].actif = 1;
}

void afficher_portails(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_PORTAILS; i++) {
        if (portails[i].actif) {
            int x_ecran = portails[i].x - scroll_x;
            draw_sprite(page, img_portail, x_ecran, portails[i].y);
        }
    }
}

void mettre_a_jour_portails() {
    for (int i = 0; i < NB_PORTAILS; i++) {
        if (portails[i].actif) {
            if (portails[i].x + img_portail->w < 0) {
                portails[i].actif = 0;  // Désactive le portail s’il sort complètement de l’écran
            }
        }
    }
}


void detruire_portails() {
    if (img_portail) destroy_bitmap(img_portail);
}



// Portail 2
BITMAP *portail2_img = NULL;
Portail portails2[NB_PORTAILS];

void init_portails2() {
    portail2_img = load_bitmap("portail2.bmp", NULL);
    if (!portail2_img) {
        allegro_message("Erreur chargement portail2.bmp");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_PORTAILS; i++) {
        portails2[i].actif = 0;
        portails2[i].x = 0;
        portails2[i].y = 0;
    }


    portails2[0].x = 1780;portails2[0].y = 127;portails2[0].actif = 1;
    portails2[1].x = 2070;portails2[1].y = 280;portails2[1].actif = 1;

    portails2[2].x = 3417;portails2[2].y = 380;portails2[2].actif = 1;
    portails2[3].x = 3625;portails2[3].y = 280;portails2[3].actif = 1;

}

void afficher_portails2(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_PORTAILS; i++) {
        if (portails2[i].actif) {
            draw_sprite(page, portail2_img, portails2[i].x - scroll_x, portails2[i].y);
        }
    }
}

void mettre_a_jour_portails2() {
    for (int i = 0; i < NB_PORTAILS; i++) {
        if (portails[i].actif) {
            if (portails[i].x + img_portail->w < 0) {
                portails[i].actif = 0;  // Désactive le portail s’il sort complètement de l’écran
            }
        }
    }
}


void detruire_portails2() {
    if (portail2_img) {
        destroy_bitmap(portail2_img);
        portail2_img = NULL;
    }
}



BITMAP *lave_img[4];
Lave laves[NB_LAVES];

void init_laves() {
    char nom[20];
    for (int i = 0; i < 4; i++) {
        sprintf(nom, "lave%d.bmp", i);
        lave_img[i] = load_bitmap(nom, NULL);
        if (!lave_img[i]) {
            allegro_message("Erreur chargement %s", nom);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NB_LAVES; i++) {
        laves[i].actif = 0;
        laves[i].x = 0;
        laves[i].y = 0;
        laves[i].frame_actuelle = 0;
    }
    int x_depart = 500;

// Lignes de lave en bas
    for (int i = 0; i < 15; i++) {
        laves[i].x = x_depart + i * 100;
        laves[i].y = 500;
        laves[i].actif = 1;
        laves[i].frame_actuelle = 0;
        laves[i].delai_frame = 10;
        laves[i].compteur_frame = 0;
    }

// Lignes de lave en haut aux mêmes x
    for (int i = 0; i < 15; i++) {
        laves[i + 15].x = x_depart + i * 100;
        laves[i + 15].y = 0;
        laves[i + 15].actif = 1;
        laves[i + 15].frame_actuelle = 0;
        laves[i + 15].delai_frame = 10;
        laves[i + 15].compteur_frame = 0;
    }

    laves[30].x = 1900 ;laves[30].y = 100;laves[30].actif = 1;laves[30].frame_actuelle = 0;laves[30].delai_frame = 10;laves[30].compteur_frame = 0;

    laves[31].x = 1900 ;laves[31].y = 400;laves[31].actif = 1;laves[31].frame_actuelle = 0;laves[31].delai_frame = 10;laves[31].compteur_frame = 0;
    laves[32].x = 1900 ;laves[32].y = 300;laves[32].actif = 1;laves[32].frame_actuelle = 0;laves[32].delai_frame = 10;laves[32].compteur_frame = 0;
    laves[33].x = 1900 ;laves[33].y = 200;laves[33].actif = 1;laves[33].frame_actuelle = 0;laves[33].delai_frame = 10;laves[33].compteur_frame = 0;

    // Laves entre x = 2000 et x = 2800, en haut et bas
    int index = 34;  // continuer après laves[33]
    int x_depart2 = 2000;

    for (int i = 0; i <= 10; i++) {  // 2000 à 2800 par pas de 100 (9 positions)
        int x = x_depart2 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 500;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }

    for (int i = 0; i <= 4; i++) {
        int x = x_depart2 + i * 100;

        laves[index].x = x;
        laves[index].y = 0;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }
    int x_depart3 = 2500;

    for (int i = 0; i <= 5; i++) {
        int x = x_depart3 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 400;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }

    int x_depart4 = 3100;

    for (int i = 0; i <= 2; i++) {
        int x = x_depart4 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 500;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }
    for (int i = 0; i <= 2; i++) {
        int x = x_depart4 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 400;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }
    for (int i = 0; i <= 2; i++) {
        int x = x_depart4 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 300;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }
    for (int i = 0; i <= 2; i++) {
        int x = x_depart4 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 200;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }
    for (int i = 0; i <= 2; i++) {
        int x = x_depart4 + i * 100;

        // Lave en bas
        laves[index].x = x;
        laves[index].y = 100;
        laves[index].actif = 1;
        laves[index].frame_actuelle = 0;
        laves[index].delai_frame = 10;
        laves[index].compteur_frame = 0;
        index++;
    }

    laves[71].x = 3550 ;laves[71].y = 0;laves[71].actif = 1;laves[71].frame_actuelle = 0;laves[71].delai_frame = 10;laves[71].compteur_frame = 0;
    laves[72].x = 3550 ;laves[72].y = 100;laves[72].actif = 1;laves[72].frame_actuelle = 0;laves[72].delai_frame = 10;laves[72].compteur_frame = 0;
    laves[73].x = 3550 ;laves[73].y = 200;laves[73].actif = 1;laves[73].frame_actuelle = 0;laves[73].delai_frame = 10;laves[73].compteur_frame = 0;
    laves[74].x = 3550 ;laves[74].y = 300;laves[74].actif = 1;laves[74].frame_actuelle = 0;laves[74].delai_frame = 10;laves[74].compteur_frame = 0;
    laves[75].x = 3550 ;laves[75].y = 400;laves[75].actif = 1;laves[75].frame_actuelle = 0;laves[75].delai_frame = 10;laves[75].compteur_frame = 0;
    laves[76].x = 3550 ;laves[76].y = 500;laves[76].actif = 1;laves[76].frame_actuelle = 0;laves[76].delai_frame = 10;laves[76].compteur_frame = 0;

    laves[77].x = 3650 ;laves[77].y = 500;laves[77].actif = 1;laves[77].frame_actuelle = 0;laves[77].delai_frame = 10;laves[77].compteur_frame = 0;
    laves[78].x = 3750 ;laves[78].y = 500;laves[78].actif = 1;laves[78].frame_actuelle = 0;laves[78].delai_frame = 10;laves[78].compteur_frame = 0;
    laves[79].x = 3800 ;laves[79].y = 500;laves[79].actif = 1;laves[79].frame_actuelle = 0;laves[79].delai_frame = 10;laves[79].compteur_frame = 0;
    laves[80].x = 3900 ;laves[80].y = 500;laves[80].actif = 1;laves[80].frame_actuelle = 0;laves[80].delai_frame = 10;laves[80].compteur_frame = 0;

    laves[81].x = 3650 ;laves[81].y = 0;laves[81].actif = 1;laves[81].frame_actuelle = 0;laves[81].delai_frame = 10;laves[81].compteur_frame = 0;
    laves[82].x = 3750 ;laves[82].y = 0;laves[82].actif = 1;laves[82].frame_actuelle = 0;laves[82].delai_frame = 10;laves[82].compteur_frame = 0;
    laves[83].x = 3800 ;laves[83].y = 0;laves[83].actif = 1;laves[83].frame_actuelle = 0;laves[83].delai_frame = 10;laves[83].compteur_frame = 0;
    laves[84].x = 3900 ;laves[84].y = 0;laves[84].actif = 1;laves[84].frame_actuelle = 0;laves[84].delai_frame = 10;laves[84].compteur_frame = 0;

}


void mettre_a_jour_laves() {
    for (int i = 0; i < NB_LAVES; i++) {
        if (!laves[i].actif) continue;

        laves[i].compteur_frame++;
        if (laves[i].compteur_frame >= laves[i].delai_frame) {
            laves[i].compteur_frame = 0;
            laves[i].frame_actuelle = (laves[i].frame_actuelle + 1) % 4;
        }
    }
}

void afficher_laves(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_LAVES; i++) {
        if (!laves[i].actif) continue;

        int x_ecran = laves[i].x - scroll_x;
        draw_sprite(page, lave_img[laves[i].frame_actuelle], x_ecran, laves[i].y);
    }
}

void detruire_laves() {
    for (int i = 0; i < 4; i++) {
        if (lave_img[i]) destroy_bitmap(lave_img[i]);
    }
}






int collision_pixel_noir(BITMAP* page, int x, int y, int rayon) {
    if (GODMODE) return 0;  // désactive les collisions en mode développeur

    // Définir les couleurs mortelles
    int noir = makecol(0, 0, 0);
    int lave = makecol(255, 174, 0);

    for (int dx = -rayon; dx <= rayon; dx += rayon) {
        for (int dy = -rayon; dy <= rayon; dy += rayon) {
            int px = x + dx;
            int py = y + dy;

            if (px >= 0 && px < page->w && py >= 0 && py < page->h) {
                int couleur = getpixel(page, px, py);

                if (couleur == noir || couleur == lave) {
                    return 1;  // collision avec pixel mortel
                }
            }
        }
    }

    return 0;  // aucune collision
}

BlocGris gris[NB_GRIS];
BITMAP *gris_img;

// Fonction de détection de collision avec les blocs gris (glisse uniquement par le dessus de la colonne)
int collision_bloc_gris(Personnage *perso, int scroll_x) {
    int bloc_superieur = -1;
    int y_min = 9999;

    for (int i = 0; i < NB_GRIS; i++) {
        if (gris[i].actif) {
            // Coordonnées du bloc gris avec le défilement
            int x_gris = gris[i].x - scroll_x;
            int y_gris = gris[i].y;
            int largeur_gris = gris_img->w;
            int hauteur_gris = gris_img->h;

            // Coordonnées du personnage
            int x_perso = perso->x_monde - scroll_x;
            int y_perso = perso->y;
            int largeur_perso = perso->image[0]->w;
            int hauteur_perso = perso->image[0]->h;

            // Détection de collision par rectangle (le personnage touche le bloc gris)
            if (x_perso + largeur_perso > x_gris && x_perso < x_gris + largeur_gris &&
                y_perso + hauteur_perso > y_gris && y_perso < y_gris + hauteur_gris)
            {
                // Trouver le bloc supérieur (le plus haut de la colonne)
                if (y_gris < y_min) {
                    y_min = y_gris;
                    bloc_superieur = i;
                }
            }
        }
    }

    // Si un bloc supérieur a été détecté
    if (bloc_superieur != -1) {
        int x_gris = gris[bloc_superieur].x - scroll_x;
        int y_gris = gris[bloc_superieur].y;
        int largeur_gris = gris_img->w;
        int hauteur_gris = gris_img->h;

        // Coordonnées du personnage
        int x_perso = perso->x_monde - scroll_x;
        int y_perso = perso->y;
        int largeur_perso = perso->image[0]->w;
        int hauteur_perso = perso->image[0]->h;

        // --- Gestion de la glisse sur le bloc supérieur ---
        // 1. Si le personnage est au-dessus (glisse sur le dessus)
        if (perso->y + perso->image[0]->h <= y_gris + 5) {
            perso->vy = 0;       // Pas de gravité
            perso->saute = 0;    // Pas de saut
            perso->y = y_gris - perso->image[0]->h; // Collé parfaitement sur le bloc supérieur

            // --- Détection continue de gris en dessous ---
            int encore_du_gris = 0;
            for (int i = 0; i < NB_GRIS; i++) {
                if (gris[i].actif) {
                    int x_g = gris[i].x - scroll_x;
                    int y_g = gris[i].y;
                    if (x_perso + largeur_perso > x_g && x_perso < x_g + largeur_gris &&
                        y_gris + hauteur_gris == y_g) {
                        encore_du_gris = 1;
                        break;
                    }
                }
            }

            // Si plus de gris en dessous, il tombe
            if (!encore_du_gris) {
                perso->saute = 1;
                perso->vy = 1; // Gravité commence à agir
            }

            return 1;
        }

        // 2. Si le personnage touche la face gauche (bloqué)
        if (perso->x_monde + perso->image[0]->w - 5 <= x_gris + scroll_x) {
            perso->vx = 0;   // Bloqué horizontalement
            perso->x_monde = x_gris + scroll_x - perso->image[0]->w + 20; // Collé parfaitement
            return 1;
        }
    }

    return 0; // Aucune collision détectée
}



BlocGris gris[NB_GRIS];
BITMAP *gris_img;

// Fonction d'initialisation des blocs gris
void init_gris() {
    gris_img = load_bitmap("gris.bmp", NULL);
    if (!gris_img) {
        allegro_message("Erreur : impossible de charger gris.bmp");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NB_GRIS; i++) {
        gris[i].actif = 0;
        gris[i].x = 0;
        gris[i].y = 0;
    }

    // Initialisation des positions des deux blocs gris
    gris[0].x = 800; gris[0].y = 500;  gris[0].actif = 1;
    gris[1].x = 800; gris[1].y = 400;  gris[1].actif = 1;
    gris[2].x = 800; gris[2].y = 300;  gris[2].actif = 1;
    gris[3].x = 1500; gris[3].y = 500;  gris[3].actif = 1;
    gris[4].x = 1500; gris[4].y = 400;  gris[4].actif = 1;
    gris[5].x = 1500; gris[5].y = 300;  gris[5].actif = 1;
    gris[6].x = 1800; gris[6].y = 100;  gris[6].actif = 1;
    gris[7].x = 1800; gris[7].y = 200;  gris[7].actif = 1;
    gris[8].x = 1800; gris[8].y = 300;  gris[8].actif = 1;
    gris[9].x = 1800; gris[9].y = 400;  gris[9].actif = 1;
    gris[10].x = 1800; gris[10].y = 500;  gris[10].actif = 1;
    gris[11].x = 1800; gris[11].y = 600;  gris[11].actif = 1;
}

// Fonction d'affichage des blocs gris
void afficher_gris(BITMAP *page, int scroll_x) {
    for (int i = 0; i < NB_GRIS; i++) {
        if (gris[i].actif) {
            draw_sprite(page, gris_img, gris[i].x - scroll_x, gris[i].y);
        }
    }
}

// Fonction de destruction des blocs gris (libération de la mémoire)
void detruire_gris() {
    if (gris_img) {
        destroy_bitmap(gris_img);
        gris_img = NULL;
    }
}

// Définition de la variable globale canon_image
BITMAP *canon_image = NULL;
Laser lasers[10]; // 10 lasers maximum

// Initialisation des canons
void init_canons(Canon canons[]) {
    // Chargement de l'image du canon
    canon_image = load_bitmap("canon.bmp", NULL);
    if (!canon_image) {
        allegro_message("Erreur de chargement de l'image canon.bmp");
        exit(EXIT_FAILURE);
    }


    // Placement initial du canon
    canons[0].x = 650;
    canons[0].y = 0;
    canons[0].actif = 1;
    canons[0].last_shot_time = clock(); // Temps initial (en ticks)

    // Initialisation des lasers
    for (int i = 0; i < 10; i++) {
        lasers[i].x = 0;
        lasers[i].y = 0;
        lasers[i].actif = 0;
    }
}

// Affichage des canons et des lasers
void afficher_canons(BITMAP *page, Canon canons[], int scroll_x) {
    if (canons[0].actif) {
        // Affichage du canon
        draw_sprite(page, canon_image, canons[0].x - scroll_x, canons[0].y);

        // Affichage des lasers
        for (int i = 0; i < 10; i++) {
            if (lasers[i].actif) {
                rectfill(page,
                         lasers[i].x - scroll_x, lasers[i].y,
                         lasers[i].x - scroll_x + 4, lasers[i].y + 15,
                         makecol(255, 0, 0)); // Laser rouge
            }
        }
    }
}

// Mise à jour des canons et des lasers
void mettre_a_jour_canons(Canon canons[]) {
    // Récupérer le temps actuel
    clock_t current_time = clock();
    float delay = 1.0f * CLOCKS_PER_SEC; // 2 secondes en ticks

    // Gestion des tirs du canon
    if (canons[0].actif && (current_time - canons[0].last_shot_time) >= delay) {
        for (int i = 0; i < 10; i++) {
            if (!lasers[i].actif) {
                // Position initiale du laser (sous la sortie du canon)
                lasers[i].x = canons[0].x + (canon_image->w / 2) - 2;
                lasers[i].y = canons[0].y + canon_image->h;
                lasers[i].actif = 1;
                canons[0].last_shot_time = current_time;
                break;
            }
        }
    }

    // Mise à jour des lasers
    for (int i = 0; i < 10; i++) {
        if (lasers[i].actif) {
            lasers[i].y += 8; // Vitesse de descente

            // Si le laser sort de l'écran, il est désactivé
            if (lasers[i].y > SCREEN_H) {
                lasers[i].actif = 0;
            }
        }
    }
}

// Destruction des canons et des lasers
void detruire_canons(Canon canons[]) {
    if (canon_image) {
        destroy_bitmap(canon_image);
        canon_image = NULL;
    }
}



