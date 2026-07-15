# Stage 1 – Pipeline de prédiction de prix immobiliers

## Présentation

Ce projet a été réalisé dans le cadre de mon premier stage en Data Science et Machine Learning.

L'objectif était de développer un pipeline complet permettant de prédire le **prix au mètre carré de biens immobiliers** à partir des données publiques DVF (Demandes de Valeurs Foncières), tout en expérimentant différentes approches de modélisation, de prétraitement et de visualisation.

Le projet couvre l'ensemble du cycle de vie d'un modèle de Machine Learning : collecte des données, nettoyage, feature engineering, entraînement, optimisation et déploiement d'une interface interactive.

---

## Objectifs du projet

- Construire un pipeline complet de traitement des données
- Nettoyer et enrichir les données DVF
- Comparer plusieurs modèles de Machine Learning
- Optimiser les performances des modèles
- Intégrer une composante géographique dans les prédictions
- Développer une interface interactive de visualisation

---

## Technologies utilisées

- Python
- Pandas
- NumPy
- Scikit-learn
- XGBoost
- Streamlit
- Plotly
- PyDeck
- Optuna
- Jupyter Notebook

---

## Contenu du projet

### Entraînement des modèles

Le dépôt contient plusieurs implémentations d'algorithmes utilisées pour comprendre, comparer ou entraîner différents modèles :

- Régression
- Descente de gradient
- K-Nearest Neighbors (KNN)
- K-Means
- Analyse en composantes principales (PCA)

---

### Préparation des données

Plusieurs scripts sont dédiés au traitement des données :

- nettoyage des fichiers DVF ;
- fusion de plusieurs jeux de données ;
- géocodage ;
- création de nouvelles variables ;
- calcul d'indicateurs géographiques.

---

### Exploration des données

Des notebooks Jupyter permettent de réaliser :

- l'analyse exploratoire des données ;
- des visualisations statistiques ;
- des expérimentations sur différents modèles ;
- des comparaisons de performances.

---

### Modèle de prédiction

Le pipeline principal repose sur plusieurs étapes :

- préparation des données ;
- création des variables explicatives ;
- séparation temporelle des jeux d'entraînement, validation et test ;
- apprentissage des modèles ;
- optimisation des hyperparamètres avec Optuna ;
- évaluation des performances.

Une approche combinant une tendance temporelle et un modèle XGBoost est également étudiée afin d'améliorer la précision des prédictions.

---

### Application Streamlit

Une application interactive permet de :

- lancer les prédictions ;
- visualiser les performances du modèle ;
- explorer les résultats ;
- afficher différentes statistiques et graphiques.

---

## Compétences développées

- Data Engineering
- Data Cleaning
- Feature Engineering
- Analyse exploratoire de données
- Machine Learning supervisé
- Clustering
- Réduction de dimension
- Optimisation d'hyperparamètres
- Visualisation de données
- Développement d'applications Streamlit

---

## Améliorations possibles

- Intégration de nouvelles sources de données
- Déploiement du modèle via une API
- Mise à jour automatique des données
- Explicabilité des prédictions (SHAP)
- Entraînement distribué sur des jeux de données plus importants
- Déploiement cloud

---

Ce projet m'a permis d'acquérir une expérience concrète dans la conception d'un pipeline complet de **Machine Learning**, allant de la préparation de données réelles jusqu'au développement d'une application interactive de visualisation et de prédiction.
