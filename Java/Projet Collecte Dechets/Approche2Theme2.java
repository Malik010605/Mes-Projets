package algorithmes;

import Graphe.*;
import java.util.*;

public class Approche2Theme2 {
    public static Graphe ConstruireGraphe(Graphe reseau, List<Sommets> sommet){
        Graphe grapheComplet = new Graphe();

        int n = sommet.size();
        for(int i=0; i<n; i++){
            Sommets si = sommet.get(i);
            for(int j=i+1; j<n; j++){
                Sommets sj = sommet.get(j);

                Itineraire itinieraire = Djikstra.calculer(reseau, si, sj);
                if (itinieraire == null){
                    continue;
                }
                double distance = itinieraire.getDistanceTotale();

                String Sommet1 = si.getLettre();
                String Sommet2 = sj.getLettre();

                //grpahe non orienté donc on ajoute les 2 sens
                grapheComplet.ajouterArc(Sommet1, Sommet2, distance);
                grapheComplet.ajouterArc(Sommet2, Sommet1, distance);
            }
        }
        return grapheComplet;
    }

    private static Graphe Prim(Graphe grapheComplet, Sommets depart){
        Graphe couvrantMinimum = new Graphe();
        List <Sommets> sommets = new ArrayList(grapheComplet.getSommets());

        int n = sommets.size();
        if (n == 0){
            return couvrantMinimum;
        }

        Map<Sommets, Double> distance = new HashMap<>();
        Map<Sommets, Sommets> parent = new HashMap<>();
        Set<Sommets> dansCouvrantMinimum = new HashSet<>();

        for (Sommets s : sommets){
            distance.put(s, 1000000.0);
            parent.put(s,null);
        }

        Sommets debut = grapheComplet.getSommet(depart.getLettre());
        if (debut == null){
            debut = sommets.get(0);
        }
        distance.put(debut,0.0);

        for (int i =0 ; i<n; i++){
            //choix du sommet hors arbre couvrant minimum avec distance mini
            Sommets a = null;
            double meilleur = 10000000.0;
            for (Sommets s : sommets){
                if (!dansCouvrantMinimum.contains(s) && distance.get(s) < meilleur){
                    meilleur = distance.get(s);
                    a = s;
                }
            }

            if (a == null){
                break;
            }
            dansCouvrantMinimum.add(a);

            Sommets p = parent.get(a);
            if (p!= null){
                double v = distance.get(a);
                couvrantMinimum.ajouterArc(p.getLettre(), a.getLettre(), v);
                couvrantMinimum.ajouterArc(a.getLettre(), p.getLettre(), v);
            }

            //mise à jour des voidins
            for (Arc arc : grapheComplet.getAdjacents(a)){
                Sommets w = arc.getArrivee();
                if (!dansCouvrantMinimum.contains(w) && arc.getPoids() < distance.get(w)){
                    distance.put(w, arc.getPoids());
                    parent.put(w,a);
                }
            }
        }
        return couvrantMinimum;
    }
    private static List<Sommets> parcoursPrefixe(Graphe CouvrantMinimum, Sommets depart){
        List<Sommets> ordre = new ArrayList<>();
        Set<Sommets> visite = new HashSet<>();

        Sommets racine = CouvrantMinimum.getSommet(depart.getLettre());
        if (racine == null) return ordre;

        dfs(CouvrantMinimum, racine, visite, ordre);
        return ordre;
    }
    private static void dfs(Graphe CouvrantMinimum, Sommets a, Set<Sommets> visite, List<Sommets> ordre) {
        visite.add(a);
        ordre.add(a);

        for(Arc arc : CouvrantMinimum.getAdjacents(a)){
            Sommets w = arc.getArrivee();
            if (!visite.contains(w)){
                dfs(CouvrantMinimum, w, visite, ordre);
            }
        }
    }


    public static Tournee calculerTourneeArbreCouvrantMinimum(Graphe reseau, Sommets depot, List<Sommets> pointsCollecte){
        List<Sommets> pointscollecte = new ArrayList<>();
        pointscollecte.add(depot);
        for(Sommets s : pointsCollecte){
            if(!pointscollecte.contains(s)){
                pointscollecte.add(s);
            }
        }

        Graphe complet = ConstruireGraphe(reseau, pointscollecte);

        Graphe CouvrantMini = Prim(complet, complet.getSommet(depot.getLettre()));

        List<Sommets> ordreCouvrantMini = parcoursPrefixe(CouvrantMini, CouvrantMini.getSommet(depot.getLettre()));


        //Supprimer les doublons tout en gardant l'ordre
        List<Sommets> ordreSansDoublons = new ArrayList<>();
        Set<String> deja = new HashSet<>();
        for (Sommets s : ordreCouvrantMini) {
            String id = s.getLettre();
            if (!deja.contains(id)) {
                ordreSansDoublons.add(s);
                deja.add(id);
            }
        }

        //Reconstruction de la tournée dans le réseau routier
        List<Sommets> cheminGlobal = new ArrayList<>();
        double distanceTotale = 0.0;

        Sommets courant = depot;
        cheminGlobal.add(courant);

        for (int i = 1; i < ordreSansDoublons.size(); i++) {
            String Cible = ordreSansDoublons.get(i).getLettre();
            Sommets cibleReseau = reseau.getSommet(Cible);

            Itineraire itin = Djikstra.calculer(reseau, courant, cibleReseau);
            if (itin == null) continue;

            List<Sommets> partiel = itin.getListeSommets();
            for (int j = 1; j < partiel.size(); j++) {
                cheminGlobal.add(partiel.get(j));
            }
            distanceTotale += itin.getDistanceTotale();
            courant = cibleReseau;
        }

        // Retour au dépôt
        if (!courant.equals(depot)) {
            Itineraire retour = Djikstra.calculer(reseau, courant, depot);
            if (retour != null) {
                List<Sommets> cheminRetour = retour.getListeSommets();
                for (int j = 1; j < cheminRetour.size(); j++) {
                    cheminGlobal.add(cheminRetour.get(j));
                }
                distanceTotale += retour.getDistanceTotale();
            }
        }
        return new Tournee(cheminGlobal, distanceTotale);
    }
}
