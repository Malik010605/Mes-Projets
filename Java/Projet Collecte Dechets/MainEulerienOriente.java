package test;

import Graphe.*;
import algorithmes.EulerienOriente;

import java.io.FileNotFoundException;
import java.util.List;

public class MainEulerOriente {
    public static void main(String[] args) {
        try {
            // Exemple : graphe ORIENTÉ (par exemple T1P2ImpHO2.txt)
            Graphe g = LectureFichier_Graphe.charger("T1P2ImpHO2.txt", true);
            Sommets depart = g.getSommet("A");   // ou null

            System.out.println("Analyse eulérienne ORIENTÉE :");

            if (!EulerienOriente.existeCheminEulerien(g)) {
                System.out.println("Ni cycle ni chemin eulérien orienté n'existe dans ce graphe.");
                return;
            }

            if (EulerienOriente.existeCycleEulerien(g)) {
                System.out.println("Cycle eulérien ORIENTÉ détecté.");
                List<Sommets> cycle = EulerienOriente.trouverCycleEulerien(g, depart);
                System.out.println("Cycle eulérien orienté : " + cycle);
            } else if (EulerienOriente.estCheminEulerienSansCycle(g)) {
                System.out.println("Chemin eulérien ORIENTÉ détecté (mais pas de cycle).");
                List<Sommets> chemin = EulerienOriente.trouverCheminEulerien(g);
                if (chemin == null || chemin.isEmpty()) {
                    System.out.println("Impossible de reconstruire le chemin eulérien orienté.");
                } else {
                    System.out.println("Chemin eulérien orienté : " + chemin);
                }
            }

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }
}
