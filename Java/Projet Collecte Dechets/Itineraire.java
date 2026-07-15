package Graphe;

import java.util.*;

public class Itineraire{

    private final List<Sommets> listeSommets;
    private final double distanceTotale;

    public Itineraire(List<Sommets> listeSommets, double distanceTotale) {
        this.listeSommets = listeSommets;
        this.distanceTotale = distanceTotale;
    }

    public List<Sommets> getListeSommets() {
        return listeSommets;
    }

    public double getDistanceTotale() {
        return distanceTotale;
    }
    @Override
    public String toString() {
        return "chemin " + listeSommets + " distance = " + distanceTotale;
    }
}