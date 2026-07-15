package test;

import Graphe.*;
import algorithmes.*;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;


public class MainMenuProjet {

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        boolean continuer = true;
        while (continuer) {
            System.out.println("=== MENU PRINCIPAL ===");
            System.out.println("1 - Thème 1");
            System.out.println("2 - Thème 2");
            System.out.println("0 - Quitter");
            System.out.print("Votre choix : ");
            int choixTheme = lireEntier(sc);

            switch (choixTheme) {
                case 1:
                    menuTheme1(sc);
                    break;
                case 2:
                    System.out.println("Thème 2 : pas encore développé pour l'instant.");
                    break;
                case 0:
                    continuer = false;
                    System.out.println("Fin du programme.");
                    break;
                default:
                    System.out.println("Choix invalide.");
            }
            System.out.println();
        }

        sc.close();
    }

    // Lecture robuste d'un entier
    private static int lireEntier(Scanner sc) {
        while (!sc.hasNextInt()) {
            System.out.print("Veuillez entrer un entier : ");
            sc.next();
        }
        int v = sc.nextInt();
        sc.nextLine(); // consommer le retour à la ligne
        return v;
    }



    private static void menuTheme1(Scanner sc) {
        System.out.println("\n--- Thème 1 ---");
        System.out.println("1 - Problématique 1");
        System.out.println("2 - Problématique 2");
        System.out.print("Votre choix : ");
        int choixPb = lireEntier(sc);

        switch (choixPb) {
            case 1:
                menuTheme1Probl1(sc);
                break;
            case 2:
                menuTheme1Probl2(sc);
                break;
            default:
                System.out.println("Choix invalide pour la problématique.");
        }
    }

    // ======== THEME 1 - PROBLÉMATIQUE 1 ========

    private static void menuTheme1Probl1(Scanner sc) {
        System.out.println("\n--- Thème 1 - Problématique 1 ---");

        System.out.println("Choisir l'hypothèse :");
        System.out.println("1 - Hypothèse 1 : itinéraire le plus court pour un ramassage (plus court chemin).");
        System.out.println("2 - Hypothèse 2 : itinéraire le plus court pour une tournée de plusieurs ramassages.");
        System.out.print("Votre choix : ");
        int choixHyp = lireEntier(sc);

        System.out.println("Choisir l'hypothèse HO :");
        System.out.println("1 - HO1");
        System.out.println("2 - HO2");
        System.out.println("3 - HO3");
        System.out.print("Votre choix : ");
        int choixHO = lireEntier(sc);

        switch (choixHyp) {
            case 1:
                executerTheme1Probl1Hyp1(choixHO);
                break;
            case 2:
                executerTheme1Probl1Hyp2(choixHO);
                break;
            default:
                System.out.println("Choix d'hypothèse invalide.");
        }
    }

    private static void executerTheme1Probl1Hyp1(int choixHO) {
        String fichier;
        boolean oriente;
        String depart;
        String arrivee;

        switch (choixHO) {
            case 1:
                fichier = "T1P1HO1.txt";
                oriente = false;
                depart = "E";
                arrivee = "C";
                break;
            case 2:
                fichier = "T1P1HO2.txt";
                oriente = true;
                depart = "E";
                arrivee = "I";
                break;
            case 3:
                fichier = "T1P1HO3.txt";
                oriente = true;
                depart = "A";
                arrivee = "M";
                break;
            default:
                System.out.println("Choix de HO invalide.");
                return;
        }

        try {
            Graphe g = LectureFichier_Graphe.charger(fichier, oriente);
            Sommets sDep = g.getSommet(depart);
            Sommets sArr = g.getSommet(arrivee);

            Itineraire itin = Djikstra.calculer(g, sDep, sArr);

            System.out.println("\n[Thème 1 - Pb1 - Hyp1] Fichier : " + fichier);
            System.out.println("Plus court chemin de " + depart + " à " + arrivee + " :");
            if (itin == null) {
                System.out.println("Aucun chemin trouvé.");
            } else {
                System.out.println(itin);
            }
        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }

    private static void executerTheme1Probl1Hyp2(int choixHO) {
        String fichier;
        boolean oriente;
        try {
            switch (choixHO) {
                case 1: {
                    fichier = "T1P1HO1.txt";
                    oriente = false;

                    Graphe ho1 = LectureFichier_Graphe.charger(fichier, oriente);
                    Sommets depot = ho1.getSommet("A");

                    List<Sommets> clients = new ArrayList<>();
                    clients.add(ho1.getSommet("A"));
                    clients.add(ho1.getSommet("C"));
                    clients.add(ho1.getSommet("D"));
                    clients.add(ho1.getSommet("E"));
                    clients.add(ho1.getSommet("F"));

                    Tournee tournee = PlusProcheVoisinHeuristique.calculerTournee(ho1, depot, clients, true);

                    System.out.println("\n[Thème 1 - Pb1 - Hyp2] HO1 - Fichier : " + fichier);
                    System.out.println(tournee);
                    break;
                }
                case 2: {
                    fichier = "T1P1HO2.txt";
                    oriente = true;

                    Graphe ho2 = LectureFichier_Graphe.charger(fichier, oriente);
                    Sommets depot = ho2.getSommet("E");

                    List<Sommets> clients = new ArrayList<>();
                    clients.add(ho2.getSommet("E"));
                    clients.add(ho2.getSommet("F"));
                    clients.add(ho2.getSommet("H"));
                    clients.add(ho2.getSommet("J"));
                    clients.add(ho2.getSommet("D"));
                    clients.add(ho2.getSommet("C"));
                    clients.add(ho2.getSommet("B"));
                    clients.add(ho2.getSommet("A"));
                    clients.add(ho2.getSommet("G"));
                    clients.add(ho2.getSommet("I"));

                    Tournee tournee = PlusProcheVoisinHeuristique.calculerTournee(ho2, depot, clients, true);

                    System.out.println("\n[Thème 1 - Pb1 - Hyp2] HO2 - Fichier : " + fichier);
                    System.out.println(tournee);
                    break;
                }
                case 3: {
                    fichier = "T1P1HO3.txt";
                    oriente = true;

                    Graphe ho3 = LectureFichier_Graphe.charger(fichier, oriente);
                    Sommets depot = ho3.getSommet("E");

                    List<Sommets> clients = new ArrayList<>();
                    clients.add(ho3.getSommet("C"));
                    clients.add(ho3.getSommet("F"));
                    clients.add(ho3.getSommet("J"));
                    clients.add(ho3.getSommet("M"));
                    clients.add(ho3.getSommet("K"));
                    clients.add(ho3.getSommet("E"));

                    Tournee tournee = PlusProcheVoisinHeuristique.calculerTournee(ho3, depot, clients, true);

                    System.out.println("\n[Thème 1 - Pb1 - Hyp2] HO3 - Fichier : " + fichier);
                    System.out.println(tournee);
                    break;
                }
                default:
                    System.out.println("Choix de HO invalide.");
            }
        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }



    private static void menuTheme1Probl2(Scanner sc) {
        System.out.println("\n--- Thème 1 - Problématique 2 ---");

        System.out.println("Choisir HO :");
        System.out.println("1 - HO1 (non orienté)");
        System.out.println("2 - HO2 (orienté)");
        System.out.println("3 - HO3 (mixte)");
        System.out.print("Votre choix : ");
        int choixHO = lireEntier(sc);

        System.out.println("Type de graphe :");
        System.out.println("1 - Cas parfait (Parf)");
        System.out.println("2 - Cas avec deux sommets impairs (Imp)");
        System.out.println("3 - Cas mixte (Mx)");
        System.out.print("Votre choix : ");
        int choixType = lireEntier(sc);

        String prefixType;
        switch (choixType) {
            case 1:
                prefixType = "Parf";
                break;
            case 2:
                prefixType = "Imp";
                break;
            case 3:
                prefixType = "Mx";
                break;
            default:
                System.out.println("Type invalide.");
                return;
        }

        switch (choixHO) {
            case 1:
                executerTheme1Probl2Hyp1_HO1(prefixType);
                break;
            case 2:
                executerTheme1Probl2Hyp1_HO2(prefixType);
                break;
            case 3:
                executerTheme1Probl2Hyp1_HO3(prefixType);
                break;
            default:
                System.out.println("Choix de HO invalide.");
        }
    }

    private static void executerTheme1Probl2Hyp1_HO1(String prefixType) {
        String fichier = "T1P2" + prefixType + "HO1.txt";

        try {
            Graphe g = LectureFichier_Graphe.charger(fichier, false);
            System.out.println("\n[Thème 1 - Pb2 - Hyp1] HO1 non orienté - Fichier : " + fichier);

            System.out.println("Sommets impairs : " + EulerienNonOriente.sommetsImpairs(g));

            if (!EulerienNonOriente.existeCheminEulerien(g)) {
                System.out.println("Ni cycle ni chemin eulérien non orienté.");
                return;
            }

            if (EulerienNonOriente.existeCycleEulerien(g)) {
                System.out.println("Cycle eulérien NON orienté détecté.");
                Sommets depart = g.getSommets().iterator().next();
                List<Sommets> cycle = EulerienNonOriente.trouverCycleEulerien(g, depart);
                System.out.println("Cycle eulérien : " + cycle);
            } else if (EulerienNonOriente.estCheminEulerienSansCycle(g)) {
                System.out.println("Chemin eulérien NON orienté (exactement deux sommets impairs).");
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

    private static void executerTheme1Probl2Hyp1_HO2(String prefixType) {
        String fichier = "T1P2" + prefixType + "HO2.txt";

        try {
            Graphe g = LectureFichier_Graphe.charger(fichier, true);
            System.out.println("\n[Thème 1 - Pb2 - Hyp1] HO2 orienté - Fichier : " + fichier);

            if (!EulerienOriente.existeCheminEulerien(g)) {
                System.out.println("Ni cycle ni chemin eulérien orienté.");
                return;
            }

            if (EulerienOriente.existeCycleEulerien(g)) {
                System.out.println("Cycle eulérien ORIENTÉ détecté.");
                Sommets depart = null;
                for (Sommets s : g.getSommets()) {
                    if (g.getAdjacent(s) != null && !g.getAdjacent(s).isEmpty()) {
                        depart = s;
                        break;
                    }
                }
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


    private static void executerTheme1Probl2Hyp1_HO3(String prefixType) {
        String fichier = "T1P2" + prefixType + "HO3.txt";

        try {
            Graphe g = LectureFichier_Graphe.charger(fichier, true);
            System.out.println("\n[Thème 1 - Pb2 - Hyp1] HO3 mixte - Fichier : " + fichier);

            System.out.println("En développement...");

        } catch (FileNotFoundException e) {
            System.out.println("Fichier introuvable : " + e.getMessage());
        }
    }
}
