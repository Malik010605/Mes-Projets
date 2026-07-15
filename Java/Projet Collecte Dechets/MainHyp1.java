package test;

import Graphe.Graphe;
import Graphe.Sommets;
import Graphe.LectureFichier_Graphe;
import algorithmes.Djikstra;
import Graphe.Itineraire;

import java.io.FileNotFoundException;

public class MainHyp1 {
    public static void main(String[] args) {
        try{
            Graphe hO1 = LectureFichier_Graphe.charger("ho1.txt", false);
            testerHO("HO1 :", hO1 , "E", "C");

            Graphe hO2 = LectureFichier_Graphe.charger("ho2.txt", true);
            testerHO("HO2 :", hO2, "E", "I");

            Graphe hO3 = LectureFichier_Graphe.charger("ho3.txt", true);
            testerHO("HO3 :", hO3, "E", "M");

        } catch(FileNotFoundException e){
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }

    private static void testerHO(String ho, Graphe graphe, String Depart, String Arrivee){
        Sommets depart = graphe.getSommet(Depart);
        Sommets arrivee = graphe.getSommet(Arrivee);

        Itineraire itineraire = Djikstra.calculer(graphe, depart, arrivee);

        System.out.println(ho + "Plus court chemin de: "+ depart + " à " + arrivee + ":");

        if (itineraire == null){
            System.out.println("Aucun chemin trouvé");
            System.out.println();
            return;
        }
        System.out.println(itineraire);
        System.out.println();
    }
}