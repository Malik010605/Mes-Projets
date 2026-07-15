package Graphe;

public class Sommets {
    private final String lettre;

    public Sommets(String lettre) {
        this.lettre = lettre;
    }
    public String getLettre(){
        return lettre;
    }
    @Override
    public String toString() {
        return lettre;
    }
}
