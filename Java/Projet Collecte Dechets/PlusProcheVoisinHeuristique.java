package algorithmes;

import Graphe.Graphe;
import Graphe.Sommets;
import Graphe.Itineraire;
import algorithmes.Tournee;
import java.util.*;

public class PlusProcheVoisinHeuristique {

    public static Tournee calculerTournee(Graphe graphe, Sommets depot, List<Sommets>clients, boolean retourDepot){
        Set<Sommets> clientsRestants = new HashSet<>(clients);
        clientsRestants.remove(depot);

        List<Sommets>ordre  = new ArrayList<>();
        ordre.add(depot);

        double distanceTotale = 0.0;
        Sommets courant = depot;

        while(!clientsRestants.isEmpty()){
            Sommets meilleurClient = null;
            Itineraire meilleurItinieraire = null;
            double meilleureDistance = 1000000000.0;

            for(Sommets cible : clientsRestants){
                Itineraire itineraire = Djikstra.calculer(graphe, courant, cible);
                if(itineraire == null){
                    continue;
                }
                double DistanceTot = itineraire.getDistanceTotale();
                if(DistanceTot < meilleureDistance){
                    meilleureDistance = DistanceTot;
                    meilleurClient = cible;
                    meilleurItinieraire = itineraire;
                }
            }
            if (meilleurClient == null){
                break; //plus aucun client atteignable
            }

            //On ajoute le chemin courant -> meilleurClient à la tournée générale
            List<Sommets>cheminPartiel = meilleurItinieraire.getListeSommets();


            //i=1 sinon courant ajouté 2 fois
            for(int i =1; i < cheminPartiel.size(); i++){
                ordre.add(cheminPartiel.get(i));
            }

            distanceTotale += meilleureDistance;
            courant = meilleurClient;
            clientsRestants.remove(meilleurClient);
        }

        //retour depot si demande
        if (retourDepot && !courant.equals(depot)) {
            Itineraire retour = Djikstra.calculer(graphe, courant, depot);
            if (retour != null){
                List<Sommets> cheminRetour = retour.getListeSommets();
                for(int i =1; i < cheminRetour.size(); i++){
                    ordre.add(cheminRetour.get(i));
                }
                distanceTotale += retour.getDistanceTotale();
            }
        }
        return new Tournee(ordre, distanceTotale);
    }
}
