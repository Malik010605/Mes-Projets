package test;

import Graphe.*;
import algorithmes.EulerienNonOriente;

import java.io.FileNotFoundException;
import java.util.List;

public class MainEulerienNonOriente {
    public static void main(String[] args) {
        try {
            // Exemple : graphe NON orienté (adapté pour T1P2ParfHO1.txt)
            Graphe g = LectureFichier_Graphe.charger("T1P2ImpHO1.txt", false);
            Sommets depart = g.getSommet("A");

            System.out.println("Analyse eulérienne NON orientée :");

            if (!EulerienNonOriente.existeCheminEulerien(g)) {
                System.out.println("Ni cycle ni chemin eulérien non orienté n'existe dans ce graphe.");
                System.out.println("Sommets impairs : " + EulerienNonOriente.sommetsImpairs(g));
                return;
            }

            if (EulerienNonOriente.existeCycleEulerien(g)) {
                System.out.println("Cycle eulérien NON orienté détecté.");
                List<Sommets> cycle = EulerienNonOriente.trouverCycleEulerien(g, depart);
                System.out.println("Cycle eulérien : " + cycle);
            } else if (EulerienNonOriente.estCheminEulerienSansCycle(g)) {
                System.out.println("Chemin eulérien NON orienté (exactement deux sommets impairs).");
                List<Sommets> impairs = EulerienNonOriente.sommetsImpairs(g);
                System.out.println("Sommets impairs : " + impairs);

                List<Sommets> chemin = EulerienNonOriente.trouverCheminEulerien(g);
                if (chemin == null || chemin.isEmpty()) {
                    System.out.println("Impossible de reconstruire le chemin eulérien.");
                } else {
                    System.out.println("Chemin eulérien : " + chemin);
                }
            }

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }
}
