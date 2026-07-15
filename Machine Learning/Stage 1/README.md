# PlaceToBe AI — Machine Learning & Data Engineering

Ce dépôt regroupe l’ensemble des travaux réalisés autour du projet **PlaceToBe AI**, avec un focus sur  
l’analyse de données immobilières (DVF), le machine learning, l’expérimentation algorithmique et la mise en production via Streamlit.

Il contient à la fois :
- des algorithmes d’entraînement,
- des notebooks d’exploration,
- des scripts de collecte de données,
- une application Streamlit complète de prédiction des prix immobiliers.


## Structure du dépôt

### `.idea/`
Fichiers de configuration de l’environnement de développement (IDE).
  
> Dossier technique, non utilisé par le code applicatif.



### `.streamlit/`
Configuration visuelle de l’application Streamlit.

Contient notamment :
- le thème (couleurs, typographie),
- les paramètres d’interface (layout, sidebar).

Objectif : assurer une cohérence graphique avec l’identité **PlaceToBe AI** (violet / blanc crème).



### `Algos entrainement/`
Implémentations des algorithmes de machine learning utilisés dans le projet.

On y retrouve :
- des scripts d’entraînement et de test,
- des modèles de régression (Ridge),
- du clustering géographique (KMeans),
- des modèles avancés de type **XGBoost**,
- des pipelines séparant tendance temporelle et résidus.

Ces scripts servent de base aux modèles intégrés ensuite dans l’application Streamlit.



### `Notebooks/`
Notebooks Jupyter dédiés à l’exploration et à l’expérimentation.

Ils incluent :
- analyses exploratoires des données DVF,
- tests de features,
- visualisations intermédiaires,
- essais de modèles et de paramètres.

Objectif : comprendre les données et valider les choix méthodologiques avant industrialisation.



### `Scrapper/`
Scripts de collecte et de préparation des données.

Ce dossier contient :
- des outils de récupération de données brutes,
- des scripts de pré-traitement,
- des étapes intermédiaires avant nettoyage et modélisation.

Il s’inscrit dans la chaîne complète **data → features → modèle**.



### `V0 prediction prix/`
Première version fonctionnelle de l’application de prédiction des prix immobiliers.

On y trouve :
- le pipeline complet DVF (nettoyage, features, split temporel),
- l’entraînement des modèles,
- une application **Streamlit** interactive permettant :
  - de lancer l’entraînement,
  - de visualiser les métriques (MAE train / val / test),
  - d’analyser les erreurs,
  - d’explorer les résultats sur une carte interactive.

Cette version sert de base pédagogique et technique pour les évolutions futures.



## Technologies utilisées

- Python
- Streamlit
- Pandas / NumPy
- Scikit-learn
- XGBoost
- Plotly
- PyDeck



## Objectif du projet

L’objectif global de ce repository est de :
- construire une **chaîne complète de traitement de données immobilières**,
- tester et comparer des approches de modélisation,
- produire une application interactive claire et exploitable,
- poser les bases d’un moteur de scoring géospatial pour le retail et l’immobilier.



## Statut

Projet en cours d’évolution.  
Le dépôt est organisé pour permettre :
- l’expérimentation,
- la montée en qualité des modèles,
- et la préparation à une mise en production progressive.
