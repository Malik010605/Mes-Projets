package test;

import Graphe.Graphe;
import Graphe.Sommets;
import Graphe.LectureFichier_Graphe;
import algorithmes.PlusProcheVoisinHeuristique;
import algorithmes.Tournee;

import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;


public class MainHyp2 {

    public static void main(String[] args) {

        try {
            //HO1 : non orienté
            Graphe ho1 = LectureFichier_Graphe.charger("ho1.txt", false);
            Sommets depotHO1 = ho1.getSommet("A");
            List<Sommets> clientsHO1 = new ArrayList<>();

            clientsHO1.add(ho1.getSommet("A"));
            clientsHO1.add(ho1.getSommet("C"));
            clientsHO1.add(ho1.getSommet("D"));
            clientsHO1.add(ho1.getSommet("E"));
            clientsHO1.add(ho1.getSommet("F"));

            Tournee tourneeHO1 = PlusProcheVoisinHeuristique.calculerTournee(ho1, depotHO1, clientsHO1, true);
            System.out.println("HO1 – Hypothèse 2 : " + tourneeHO1);
            System.out.println();



            //HO2 : orienté
            Graphe ho2 = LectureFichier_Graphe.charger("ho2.txt", true);
            Sommets depotHO2 = ho2.getSommet("E");
            List<Sommets> clientsHO2 = new ArrayList<>();

            clientsHO2.add(ho2.getSommet("E"));
            clientsHO2.add(ho2.getSommet("F"));
            clientsHO2.add(ho2.getSommet("H"));
            clientsHO2.add(ho2.getSommet("J"));
            clientsHO2.add(ho2.getSommet("D"));
            clientsHO2.add(ho2.getSommet("C"));
            clientsHO2.add(ho2.getSommet("B"));
            clientsHO2.add(ho2.getSommet("A"));
            clientsHO2.add(ho2.getSommet("G"));
            clientsHO2.add(ho2.getSommet("I"));

            Tournee tourneeHO2 = PlusProcheVoisinHeuristique.calculerTournee(ho2, depotHO2, clientsHO2, true);
            System.out.println("HO2 – Hypothèse 2 : " + tourneeHO2);
            System.out.println();




            //HO3 : mixte
            Graphe ho3 = LectureFichier_Graphe.charger("ho3.txt", true);
            Sommets depotHO3 = ho3.getSommet("E");
            List<Sommets> clientsHO3 = new ArrayList<>();

            clientsHO3.add(ho3.getSommet("C"));
            clientsHO3.add(ho3.getSommet("F"));
            clientsHO3.add(ho3.getSommet("J"));
            clientsHO3.add(ho3.getSommet("M"));
            clientsHO3.add(ho3.getSommet("K"));
            clientsHO3.add(ho3.getSommet("E"));

            Tournee tourneeHO3 = PlusProcheVoisinHeuristique.calculerTournee(ho3, depotHO3, clientsHO3, true);
            System.out.println("HO3 – Hypothèse 2 : " + tourneeHO3);
            System.out.println();

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }
}

