package algorithmes;

import Graphe.Sommets;
import java.util.List;

public class Tournee {
    private final List<Sommets> ordreVisite;
    private final double distanceTotale;

    public Tournee(List<Sommets>ordreVisite, double distanceTotale) {
        this.ordreVisite = ordreVisite;
        this.distanceTotale = distanceTotale;
    }

    public List<Sommets> getOrdreVisite() {
        return ordreVisite;
    }

    public double getDistanceTotale() {
        return distanceTotale;
    }
    @Override
    public String toString(){
        return "Tournee " + ordreVisite + " distance " +  distanceTotale;
    }
}
