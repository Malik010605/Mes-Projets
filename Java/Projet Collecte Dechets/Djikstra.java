package algorithmes;

import java.util.*;
import Graphe.*;

public class Djikstra {

    public static Itineraire calculer(Graphe graphe, Sommets depart, Sommets arrivee) {
        final double INFINI = 1000000000.0;

        Map<Sommets, Double> distance = new HashMap<>();
        Map<Sommets, Sommets> precedent = new HashMap<>();
        List<Sommets> nonVisite = new ArrayList<>();

        for (Sommets sommet : graphe.getSommets()) {
            distance.put(sommet, INFINI);
            nonVisite.add(sommet);
        }

        distance.put(depart, 0.0);
        precedent.put(depart, null);

        while (!nonVisite.isEmpty()) {
            Sommets p = choisirSommetPlusProche(nonVisite, distance);
            if (distance.get(p) == INFINI) {
                break;
            }
            nonVisite.remove(p);

            if (p.equals(arrivee)) {
                break;
            }

            for (Arc arc : graphe.getAdjacent(p)) {
                Sommets a = arc.getArrivee();
                if (!nonVisite.contains(a)) {
                    continue;
                }

                double nouvelleDistance = distance.get(p) + arc.getPoids();
                if (nouvelleDistance < distance.get(a)) {
                    distance.put(a, nouvelleDistance);
                    precedent.put(a, p);
                }
            }
        }

        //Si pas de précédent pour arrivée, pas de chemin
        if (!depart.equals(arrivee) && !precedent.containsKey(arrivee)) {
            return null;
        }

        //Reconstruction du chemin, on remonte de arrivee jusqu'à depart
        List<Sommets> chemin = new ArrayList<>();
        Sommets courant = arrivee;

        while (courant != null) {
            chemin.add(courant);
            courant = precedent.get(courant);
        }

        Collections.reverse(chemin);

        double distanceTotale = distance.get(arrivee);
        return new Itineraire(chemin, distanceTotale);
    }

    //Choix parmi les sommets non visités, celui qui a la plus petite distance
    private static Sommets choisirSommetPlusProche(List<Sommets> nonVisite, Map<Sommets, Double> distance){
        Sommets meilleur = nonVisite.get(0);
        double meilleureDistance = distance.get(meilleur);

        for(Sommets sommet : nonVisite){
            double d = distance.get(sommet);
            if (d < meilleureDistance){
                meilleur = sommet;
                meilleureDistance = d;
            }
        }
        return meilleur;
    }
}
