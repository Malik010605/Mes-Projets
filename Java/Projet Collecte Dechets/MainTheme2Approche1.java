package test;

import Graphe.*;
import algorithmes.*;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

public class MainTheme2Approche1 {
    public static void main(String[] args) {

        try {
            Graphe reseau = LectureFichier_Graphe.charger("ho1.txt", false);

            Sommets depot = reseau.getSommet("A");

            List<Sommets> pointsCollecte = new ArrayList<>();
            pointsCollecte.add(reseau.getSommet("L"));
            pointsCollecte.add(reseau.getSommet("G"));
            pointsCollecte.add(reseau.getSommet("H"));

            Tournee tournee = PlusProcheVoisinHeuristique.calculerTournee(reseau, depot, pointsCollecte, true);

            System.out.println("Tournee Theme2 Approche 1: ");
            System.out.println(tournee);

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());

        }
    }
}
