package test;


import Graphe.*;
import algorithmes.*;
import  java.util.*;
import java.io.FileNotFoundException;

public class MainTheme2_Approche2 {
    public static void main(String[] args) {

        try {
            Graphe reseau = LectureFichier_Graphe.charger("ho2", false);
            Sommets depot = reseau.getSommet("A");

            List<Sommets> pointscollecte = new ArrayList<>();
            pointscollecte.add(reseau.getSommet("L"));
            pointscollecte.add(reseau.getSommet("G"));
            pointscollecte.add(reseau.getSommet("H"));

            Tournee tourneeCouvrantMini = Approche2Theme2.calculerTourneeArbreCouvrantMinimum(reseau, depot, pointscollecte);
            System.out.println("Thème 2 – Approche 2 (MST + Prim + Dijkstra) :");
            System.out.println(tourneeCouvrantMini);

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }
}
