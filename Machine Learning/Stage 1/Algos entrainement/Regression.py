#Étape 1 : Importer les bibliothèques nécessaires

import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression

#Étape 2 : Créer des données d'exemple

#X : Notes de qualité (notre variable indépendante)

#y : Prix des produits (notre variable dépendante à prédire)

X = np.array([[5], [6], [7], [8], [9], [10]]) # Notes sur 10
y = np.array([12, 15, 18, 22, 24, 28])      # Prix en euros

#Étape 3 : Créer et entraîner le modèle de régression

#On crée une instance du modèle

model = LinearRegression()

#On entraîne le modèle avec nos données (X et y)

#La méthode .fit() trouve la meilleure ligne qui décrit la relation entre la note et le prix

model.fit(X, y)

print(" Modèle entraîné avec succès !")

#Étape 4 : Faire une prédiction sur une nouvelle donnée

#Quelle serait le prix pour un produit ayant une note de 8.5 ?

note_a_predire = np.array([[8.5]])
prix_estime = model.predict(note_a_predire)

print(f" Pour une note de {note_a_predire[0][0]}, le prix estimé est de : {prix_estime[0]:.2f} €")

#Étape 5 (Bonus) : Visualiser la ligne de régression

plt.figure(figsize=(8, 6))
plt.title('Régression Linéaire Simple : Prix vs. Note')
plt.xlabel('Note de Qualité')
plt.ylabel('Prix (€)')

#Afficher les points de données originaux

plt.scatter(X, y, color='blue', label='Données réelles')

#Afficher la ligne de régression apprise par le modèle

plt.plot(X, model.predict(X), color='red', label='Ligne de régression')

#Afficher notre prédiction

plt.scatter(note_a_predire, prix_estime, color='green', marker='X', s=100, label='Prédiction')

plt.legend()
plt.grid(True)
plt.show()