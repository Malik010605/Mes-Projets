package algorithmes;
import Graphe.*;
import java.util.*;


public class EulerienOriente {

    // Calcule pour chaque sommet son nombre d'arcs sortants.
    private static Map<Sommets, Integer> calculerDegresSortants(Graphe g) {
        Map<Sommets, Integer> out = new HashMap<>();
        for (Sommets s : g.getSommets()) {
            List<Arc> arcs = g.getAdjacent(s);
            int d = (arcs == null) ? 0 : arcs.size();
            out.put(s, d);
        }
        return out;
    }

    // Calcule pour chaque sommet son nombre d'arcs entrants.
    private static Map<Sommets, Integer> calculerDegresEntrants(Graphe g) {
        Map<Sommets, Integer> in = new HashMap<>();
        for (Sommets s : g.getSommets()) {
            in.put(s, 0);
        }

        for (Sommets s : g.getSommets()) {
            List<Arc> arcs = g.getAdjacent(s);
            if (arcs == null) continue;
            for (Arc arc : arcs) {
                Sommets v = arc.getArrivee();
                in.put(v, in.get(v) + 1);
            }
        }
        return in;
    }


    // Vérifie si le graphe orienté possède un cycle eulérien (in-degree == out-degree pour chaque sommet).
    public static boolean existeCycleEulerien(Graphe g) {
        Map<Sommets, Integer> in = calculerDegresEntrants(g);
        Map<Sommets, Integer> out = calculerDegresSortants(g);

        for (Sommets s : g.getSommets()) {
            int di = in.getOrDefault(s, 0);
            int doo = out.getOrDefault(s, 0);
            if (di != doo) {
                return false;
            }
        }
        return true;
    }


    // Vérifie si un chemin eulérien (orienté) existe : 0 ou 2 sommets déséquilibrés.
    public static boolean existeCheminEulerien(Graphe g) {
        Map<Sommets, Integer> in = calculerDegresEntrants(g);
        Map<Sommets, Integer> out = calculerDegresSortants(g);

        int plus1 = 0;
        int moins1 = 0;

        for (Sommets s : g.getSommets()) {
            int di = in.getOrDefault(s, 0);
            int doo = out.getOrDefault(s, 0);
            int diff = doo - di; // out - in

            if (diff == 1) {
                plus1++;
            } else if (diff == -1) {
                moins1++;
            } else if (diff != 0) {
                return false;
            }
        }

        if (plus1 == 0 && moins1 == 0) {
            return true; // c’est en fait un cycle eulérien
        }
        return (plus1 == 1 && moins1 == 1);
    }


    // Vérifie uniquement le cas d’un chemin eulérien non cyclique (exactement +1 et -1).
    public static boolean estCheminEulerienSansCycle(Graphe g) {
        Map<Sommets, Integer> in = calculerDegresEntrants(g);
        Map<Sommets, Integer> out = calculerDegresSortants(g);

        int plus1 = 0;
        int moins1 = 0;

        for (Sommets s : g.getSommets()) {
            int di = in.getOrDefault(s, 0);
            int doo = out.getOrDefault(s, 0);
            int diff = doo - di;

            if (diff == 1) {
                plus1++;
            } else if (diff == -1) {
                moins1++;
            } else if (diff != 0) {
                return false;
            }
        }
        return (plus1 == 1 && moins1 == 1);
    }


    // Construit la liste d’adjacence orientée du graphe (arêtes dans le sens des arcs).
    private static Map<Sommets, List<Sommets>> construireAdjacence(Graphe g) {
        Map<Sommets, List<Sommets>> adj = new HashMap<>();
        for (Sommets s : g.getSommets()) {
            adj.put(s, new ArrayList<>());
        }
        for (Sommets s : g.getSommets()) {
            List<Arc> arcs = g.getAdjacent(s);
            if (arcs == null) continue;
            for (Arc arc : arcs) {
                Sommets v = arc.getArrivee();
                adj.get(s).add(v);
            }
        }
        return adj;
    }


    // Choisit un sommet de départ valide pour un chemin eulérien orienté.
    private static Sommets choisirSommetDepartPourChemin(Graphe g) {
        Map<Sommets, Integer> in = calculerDegresEntrants(g);
        Map<Sommets, Integer> out = calculerDegresSortants(g);

        Sommets candidatPlus1 = null;
        Sommets candidatAvecArcs = null;

        for (Sommets s : g.getSommets()) {
            int di = in.getOrDefault(s, 0);
            int doo = out.getOrDefault(s, 0);
            if (doo - di == 1) {
                candidatPlus1 = s;
            }
            List<Arc> arcs = g.getAdjacent(s);
            if (arcs != null && !arcs.isEmpty()) {
                if (candidatAvecArcs == null) {
                    candidatAvecArcs = s;
                }
            }
        }

        if (candidatPlus1 != null) {
            return candidatPlus1;
        }
        return candidatAvecArcs;
    }


    // Applique l’algorithme de Hierholzer pour construire un parcours eulérien orienté.
    private static List<Sommets> hierholzer(Map<Sommets, List<Sommets>> adj, Sommets depart) {
        Deque<Sommets> pile = new ArrayDeque<>();
        List<Sommets> circuit = new ArrayList<>();

        if (depart == null) {
            return circuit;
        }

        pile.push(depart);

        while (!pile.isEmpty()) {
            Sommets v = pile.peek();
            List<Sommets> voisins = adj.get(v);

            if (voisins == null || voisins.isEmpty()) {
                circuit.add(v);
                pile.pop();
            } else {
                Sommets u = voisins.remove(0); // on consomme une arête
                pile.push(u);
            }
        }

        Collections.reverse(circuit);
        return circuit;
    }


    // Trouve et renvoie un cycle eulérien orienté si le graphe satisfait les conditions.
    public static List<Sommets> trouverCycleEulerien(Graphe g, Sommets depart) {
        if (!existeCycleEulerien(g)) {
            System.out.println("Aucun cycle eulérien orienté n'existe (in-degree != out-degree).");
            return null;
        }

        if (depart == null) {
            for (Sommets s : g.getSommets()) {
                List<Arc> arcs = g.getAdjacent(s);
                if (arcs != null && !arcs.isEmpty()) {
                    depart = s;
                    break;
                }
            }
        }
        if (depart == null) {
            return new ArrayList<>();
        }

        Map<Sommets, List<Sommets>> adj = construireAdjacence(g);
        return hierholzer(adj, depart);
    }


    // Trouve et renvoie un chemin eulérien orienté non cyclique (exactement deux sommets déséquilibrés).
    public static List<Sommets> trouverCheminEulerien(Graphe g) {
        if (!estCheminEulerienSansCycle(g)) {
            System.out.println("Le graphe ne possède pas exactement un chemin eulérien non cyclique.");
            return null;
        }

        Sommets depart = choisirSommetDepartPourChemin(g);
        if (depart == null) {
            return new ArrayList<>();
        }

        Map<Sommets, List<Sommets>> adj = construireAdjacence(g);
        return hierholzer(adj, depart);
    }
}
