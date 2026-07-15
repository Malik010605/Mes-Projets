package Graphe;

import java.util.*;

public class Graphe {
    private final Map<String, Sommets> sommets = new HashMap<>();
    private final Map<Sommets, List<Arc>> adjacent =  new HashMap<>();

    public Sommets ajouterSommet(String sommetajt){
        if (!sommets.containsKey(sommetajt)){
            Sommets s = new Sommets(sommetajt);
            sommets.put(sommetajt, s);
            adjacent.put(s, new ArrayList<>());
        }
        return sommets.get(sommetajt);
    }

    public void ajouterArc(String sommetDepart, String sommetArrivee, double poids){
        Sommets depart =  ajouterSommet(sommetDepart);
        Sommets arrivee = ajouterSommet(sommetArrivee);
        Arc arc = new Arc(depart, arrivee, poids);
        adjacent.get(depart).add(arc);
    }

    public Sommets getSommet(String sommet){
        return sommets.get(sommet);
    }

    public Collection<Sommets> getSommets(){
        return sommets.values();
    }

    public List<Arc> getAdjacent(Sommets sommet){
        List<Arc> arcs = adjacent.get(sommet);
        return arcs;
    }
}
