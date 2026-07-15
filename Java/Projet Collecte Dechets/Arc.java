package Graphe;

public class Arc{
    private final Sommets depart;
    private final Sommets arrivee;
    private final double poids;

    public Arc(Sommets depart, Sommets arrivee, double poids){
        this.depart = depart;
        this.arrivee = arrivee;
        this.poids = poids;
    }
    public Sommets getDepart(){return depart;}
    public Sommets getArrivee(){return arrivee;}
    public double getPoids(){return poids;}
}
