package algorithmes;

import Graphe.*;
import java.util.*;


public class EulerienNonOriente {

    // Génère une clé unique et ordonnée pour représenter une arête non orientée entre deux sommets.
    private static String cle(Sommets a, Sommets b) {
        String la = a.getLettre();
        String lb = b.getLettre();
        return (la.compareTo(lb) <= 0) ? la + "-" + lb : lb + "-" + la;
    }


    // Construit la liste d’adjacence non orientée du graphe à partir de ses arcs orientés.
    private static Map<Sommets, List<Sommets>> construireAdjacenceNonOriente(Graphe g) {
        Map<Sommets, List<Sommets>> adj = new HashMap<>();
        for (Sommets s : g.getSommets()) {
            adj.put(s, new ArrayList<>());
        }

        Set<String> deja = new HashSet<>();

        for (Sommets s : g.getSommets()) {
            List<Arc> arcs = g.getAdjacent(s);
            if (arcs == null) continue;

            for (Arc arc : arcs) {
                Sommets u = arc.getDepart();
                Sommets v = arc.getArrivee();
                String key = cle(u, v);

                // Une seule arête non orientée par paire u-v
                if (!deja.contains(key)) {
                    deja.add(key);
                    adj.get(u).add(v);
                    adj.get(v).add(u);
                }
            }
        }
        return adj;
    }


    // Calcule le degré non orienté de chaque sommet du graphe.
    private static Map<Sommets, Integer> calculerDegresNonOrientes(Graphe g) {
        Map<Sommets, List<Sommets>> adj = construireAdjacenceNonOriente(g);
        Map<Sommets, Integer> degres = new HashMap<>();

        for (Sommets s : g.getSommets()) {
            List<Sommets> voisins = adj.get(s);
            int d = (voisins == null) ? 0 : voisins.size();
            degres.put(s, d);
        }
        return degres;
    }


    // Retourne la liste des sommets ayant un degré impair.
    public static List<Sommets> sommetsImpairs(Graphe g) {
        Map<Sommets, Integer> degres = calculerDegresNonOrientes(g);
        List<Sommets> impairs = new ArrayList<>();

        for (Map.Entry<Sommets, Integer> e : degres.entrySet()) {
            if (e.getValue() % 2 != 0) {
                impairs.add(e.getKey());
            }
        }
        return impairs;
    }


    // Vérifie s’il existe un cycle eulérien (aucun sommet impair).
    public static boolean existeCycleEulerien(Graphe g) {
        return sommetsImpairs(g).isEmpty();
    }


    // Vérifie s’il existe un chemin eulérien (0 ou 2 sommets impairs).
    public static boolean existeCheminEulerien(Graphe g) {
        int k = sommetsImpairs(g).size();
        return (k == 0 || k == 2);
    }


    // Vérifie s’il existe un chemin eulérien non cyclique (exactement 2 sommets impairs).
    public static boolean estCheminEulerienSansCycle(Graphe g) {
        return sommetsImpairs(g).size() == 2;
    }



    // Applique l’algorithme de Hierholzer pour construire un chemin/cycle eulérien dans un graphe non orienté.
    private static List<Sommets> hierholzer(Map<Sommets, List<Sommets>> adj, Sommets depart) {
        Deque<Sommets> pile = new ArrayDeque<>();
        List<Sommets> circuit = new ArrayList<>();

        if (depart == null) return circuit;

        pile.push(depart);

        while (!pile.isEmpty()) {
            Sommets v = pile.peek();
            List<Sommets> voisins = adj.get(v);

            if (voisins == null || voisins.isEmpty()) {
                circuit.add(v);
                pile.pop();
            } else {
                Sommets u = voisins.remove(0);
                List<Sommets> adjU = adj.get(u);
                if (adjU != null) {
                    adjU.remove(v);  // suppression de l’arête dans les deux sens
                }
                pile.push(u);
            }
        }

        Collections.reverse(circuit);
        return circuit;
    }



    // Construit et renvoie un cycle eulérien si le graphe en possède un.
    public static List<Sommets> trouverCycleEulerien(Graphe g, Sommets depart) {
        List<Sommets> impairs = sommetsImpairs(g);
        if (!impairs.isEmpty()) {
            System.out.println("Aucun cycle eulérien non orienté n'existe : sommets impairs = " + impairs);
            return null;
        }

        Map<Sommets, List<Sommets>> adj = construireAdjacenceNonOriente(g);

        // Choisit automatiquement un sommet non isolé si depart est invalide.
        if (depart == null || adj.get(depart) == null || adj.get(depart).isEmpty()) {
            for (Sommets s : g.getSommets()) {
                List<Sommets> voisins = adj.get(s);
                if (voisins != null && !voisins.isEmpty()) {
                    depart = s;
                    break;
                }
            }
        }

        return hierholzer(adj, depart);
    }


    // Construit et renvoie un chemin eulérien non cyclique (exactement deux sommets impairs).
    public static List<Sommets> trouverCheminEulerien(Graphe g) {
        List<Sommets> impairs = sommetsImpairs(g);
        if (impairs.size() != 2) {
            System.out.println("Pas exactement deux sommets impairs, pas de chemin eulérien non cyclique.");
            return null;
        }

        Map<Sommets, List<Sommets>> adj = construireAdjacenceNonOriente(g);

        // Démarre obligatoirement sur l’un des sommets impairs.
        Sommets depart = impairs.get(0);
        return hierholzer(adj, depart);
    }
}
