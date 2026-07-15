package Graphe;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class LectureFichier_Graphe {
    public static Graphe charger(String nomFichier, boolean Oriente) throws FileNotFoundException {
        Graphe graphe = new Graphe();
        Scanner sc = new Scanner(new File(nomFichier));
        while (sc.hasNextLine()) {
            String ligne = sc.nextLine().trim();
            if (ligne.isEmpty()) {
                continue;
            }
            String[] morceaux = ligne.split(";");
            if (morceaux.length != 3) {
                System.out.println("Ligne mal formatée : " + ligne);
                continue;
            }
            String depart = morceaux[0].trim();
            String arrivee = morceaux[1].trim();
            String poidstxt = morceaux[2].trim();

            double poids = Double.parseDouble(poidstxt);


            graphe.ajouterSommet(depart);
            graphe.ajouterSommet(arrivee);
            graphe.ajouterArc(depart, arrivee, poids);

            if (!Oriente) {
                graphe.ajouterArc(arrivee, depart, poids);
            }
        }
        sc.close();
        return graphe;
    }
}
