import numpy as np
import matplotlib.pyplot as plt
from sklearn.neighbors import KNeighborsClassifier

# =============================
# Génération des données
# =============================

np.random.seed(42)

# Classe A
X_A = np.random.normal(loc=[2, 2], scale=0.6, size=(25, 2))

# Classe B
X_B = np.random.normal(loc=[6, 6], scale=0.6, size=(25, 2))

# Données d'entraînement
XZ_train = np.vstack((X_A, X_B))

# Étiquettes
y_train = np.array(['A'] * len(X_A) + ['B'] * len(X_B))

# =============================
# Entraînement du KNN
# =============================

knn = KNeighborsClassifier(n_neighbors=5)
knn.fit(XZ_train, y_train)

# =============================
# Point test
# =============================

T_test = np.array([[4, 4]])
Y_test = knn.predict(T_test)

print("Classe prédite :", Y_test[0])

# =============================
# Visualisation
# =============================

plt.figure(figsize=(7, 7))

plt.scatter(X_A[:, 0], X_A[:, 1], s=60, label='Classe A')
plt.scatter(X_B[:, 0], X_B[:, 1], s=60, label='Classe B')

plt.scatter(T_test[:, 0], T_test[:, 1],
            s=150, marker='X', label='Point test')

# Affichage des k plus proches voisins
distances, indices = knn.kneighbors(T_test)
for idx in indices[0]:
    plt.plot([T_test[0, 0], XZ_train[idx, 0]],
             [T_test[0, 1], XZ_train[idx, 1]],
             linestyle='--', alpha=0.4)

plt.xlabel("Variable 1")
plt.ylabel("Variable 2")
plt.title("KNN réaliste (k = 5) avec plusieurs points")
plt.legend()
plt.grid(True)
plt.show()
