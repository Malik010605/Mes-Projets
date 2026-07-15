import numpy as np
import matplotlib.pyplot as plt

from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA


def visualize_pca(random_state: int = 42) -> None:
    # ------------------------------------------------------------
    # 1) Données synthétiques (3 variables) avec classes pour la couleur
    # ------------------------------------------------------------
    X, y = make_blobs(
        n_samples=600,
        centers=4,
        n_features=3,
        cluster_std=[2.2, 1.6, 2.0, 1.3],
        random_state=random_state,
    )

    # Ajout volontaire de corrélations entre variables (pour rendre PCA utile visuellement)
    rng = np.random.default_rng(random_state)
    A = np.array([
        [1.0, 0.8, 0.2],
        [0.2, 1.0, 0.6],
        [0.5, 0.1, 1.0],
    ])
    X = X @ A + 0.3 * rng.normal(size=X.shape)

    # ------------------------------------------------------------
    # 2) Standardisation (souvent indispensable avant PCA)
    # ------------------------------------------------------------
    scaler = StandardScaler()
    Xs = scaler.fit_transform(X)

    # ------------------------------------------------------------
    # 3) PCA
    # ------------------------------------------------------------
    pca = PCA(n_components=3, random_state=random_state)
    Xp = pca.fit_transform(Xs)

    explained = pca.explained_variance_ratio_
    cum_explained = np.cumsum(explained)

    # ------------------------------------------------------------
    # 4) Plots : (a) 3D avant / après PCA, (b) Projection 2D PC1/PC2, (c) Variance expliquée, (d) Cercle des corrélations
    # ------------------------------------------------------------

    # (a) 3D AVANT PCA (données standardisées) vs APRES PCA
    fig = plt.figure(figsize=(13, 5))

    ax1 = fig.add_subplot(1, 2, 1, projection="3d")
    sc1 = ax1.scatter(Xs[:, 0], Xs[:, 1], Xs[:, 2], c=y, s=18)
    ax1.set_title("Avant PCA (données standardisées)")
    ax1.set_xlabel("Feature 1 (standardisée)")
    ax1.set_ylabel("Feature 2 (standardisée)")
    ax1.set_zlabel("Feature 3 (standardisée)")

    ax2 = fig.add_subplot(1, 2, 2, projection="3d")
    sc2 = ax2.scatter(Xp[:, 0], Xp[:, 1], Xp[:, 2], c=y, s=18)
    ax2.set_title("Après PCA (PC1, PC2, PC3)")
    ax2.set_xlabel(f"PC1 ({explained[0]*100:.1f}%)")
    ax2.set_ylabel(f"PC2 ({explained[1]*100:.1f}%)")
    ax2.set_zlabel(f"PC3 ({explained[2]*100:.1f}%)")

    plt.tight_layout()
    plt.show()

    # (b) Projection 2D PC1/PC2 + flèches des loadings (contributions des variables)
    plt.figure(figsize=(7.5, 6))
    plt.scatter(Xp[:, 0], Xp[:, 1], c=y, s=18)
    plt.axhline(0, linewidth=1)
    plt.axvline(0, linewidth=1)
    plt.title("Projection PCA : PC1 vs PC2")
    plt.xlabel(f"PC1 ({explained[0]*100:.1f}%)")
    plt.ylabel(f"PC2 ({explained[1]*100:.1f}%)")

    # Loadings = directions des variables dans l’espace des PC
    # Pour une PCA sur données standardisées, on peut visualiser pca.components_.T comme directions.
    loadings = pca.components_.T[:, :2]  # (p=3 variables) -> 2 composantes
    # On scale les flèches pour la lisibilité
    arrow_scale = 3.0
    for j in range(loadings.shape[0]):
        plt.arrow(
            0, 0,
            arrow_scale * loadings[j, 0],
            arrow_scale * loadings[j, 1],
            width=0.02,
            length_includes_head=True
        )
        plt.text(
            arrow_scale * loadings[j, 0] * 1.08,
            arrow_scale * loadings[j, 1] * 1.08,
            f"Feature {j+1}",
            fontsize=10
        )

    plt.show()

    # (c) Variance expliquée (barres) + cumul
    plt.figure(figsize=(7.5, 5))
    x = np.arange(1, 4)
    plt.bar(x, explained * 100)
    plt.plot(x, cum_explained * 100, marker="o")
    plt.xticks(x, [f"PC{i}" for i in x])
    plt.ylabel("Variance expliquée (%)")
    plt.title("Variance expliquée par composante (et cumul)")
    plt.ylim(0, 105)
    plt.show()

    # (d) Cercle des corrélations (PC1/PC2)
    # Approche classique : corr(variable_j, PC_k) = loading_jk * sqrt(eigenvalue_k)
    # En sklearn : eigenvalues = pca.explained_variance_
    eig = pca.explained_variance_
    corr = pca.components_.T[:, :2] * np.sqrt(eig[:2])

    plt.figure(figsize=(6.5, 6.5))
    theta = np.linspace(0, 2*np.pi, 400)
    plt.plot(np.cos(theta), np.sin(theta))  # cercle unité
    plt.axhline(0, linewidth=1)
    plt.axvline(0, linewidth=1)

    for j in range(corr.shape[0]):
        plt.arrow(
            0, 0,
            corr[j, 0], corr[j, 1],
            width=0.01,
            length_includes_head=True
        )
        plt.text(corr[j, 0] * 1.08, corr[j, 1] * 1.08, f"Feature {j+1}", fontsize=10)

    plt.gca().set_aspect("equal", "box")
    plt.xlim(-1.1, 1.1)
    plt.ylim(-1.1, 1.1)
    plt.xlabel(f"PC1 ({explained[0]*100:.1f}%)")
    plt.ylabel(f"PC2 ({explained[1]*100:.1f}%)")
    plt.title("Cercle des corrélations (PC1/PC2)")
    plt.show()

    # ------------------------------------------------------------
    # 5) Petite vérif : reconstruction (compression -> décompression)
    # ------------------------------------------------------------
    pca2 = PCA(n_components=2, random_state=random_state)
    Xp2 = pca2.fit_transform(Xs)
    Xs_recon = pca2.inverse_transform(Xp2)
    mse = np.mean((Xs - Xs_recon) ** 2)

    print("=== Résumé PCA ===")
    print(f"Explained variance ratio: {explained}")
    print(f"Cumul: {cum_explained}")
    print(f"MSE reconstruction avec 2 composantes (sur données standardisées): {mse:.6f}")


if __name__ == "__main__":
    visualize_pca()
