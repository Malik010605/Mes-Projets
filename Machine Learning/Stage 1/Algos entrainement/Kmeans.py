"""
K-Means clustering demo with synthetic data, scaling, metrics, and plots.
"""

import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score

# --------------------------------------------------
# 1) Generate synthetic 2D data with 4 clusters
# --------------------------------------------------
X, y_true = make_blobs(
    n_samples=400,  # >=300 points
    centers=4,
    n_features=2,
    cluster_std=1.2,
    random_state=42,
)

# --------------------------------------------------
# 2) Standardize features
#    Scaling puts all features on comparable ranges so distance-based
#    clustering (K-Means) isn’t dominated by larger-scale dimensions.
# --------------------------------------------------
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# --------------------------------------------------
# 3) Fit K-Means on the full dataset
# --------------------------------------------------
k = 4
kmeans = KMeans(
    n_clusters=k,
    n_init="auto",  # modern default search over centroid seeds
    random_state=42,
)
kmeans.fit(X_scaled)
labels = kmeans.labels_
centroids = kmeans.cluster_centers_

# --------------------------------------------------
# 4) Metrics
#    Inertia = sum of squared distances of points to their assigned centroids
# --------------------------------------------------
inertia = kmeans.inertia_
sil_score = silhouette_score(X_scaled, labels)

print(f"Inertia (K-Means cost J): {inertia:.2f}")
print(f"Silhouette score: {sil_score:.3f}")

# --------------------------------------------------
# 5) Elbow method: try k = 1..10 and record inertia
# --------------------------------------------------
ks = range(1, 11)
inertias = []
for kk in ks:
    km = KMeans(n_clusters=kk, n_init="auto", random_state=42)
    km.fit(X_scaled)
    inertias.append(km.inertia_)

# --------------------------------------------------
# 6) Plots
# --------------------------------------------------
fig, axes = plt.subplots(1, 3, figsize=(15, 4))

# (a) Raw dataset (no labels)
axes[0].scatter(X[:, 0], X[:, 1], s=20, color="gray", alpha=0.7)
axes[0].set_title("Raw Data (Unlabeled)")
axes[0].set_xlabel("Feature 1")
axes[0].set_ylabel("Feature 2")

# (b) Clustered result (colored by predicted cluster)
scatter = axes[1].scatter(
    X_scaled[:, 0], X_scaled[:, 1], c=labels, s=20, cmap="tab10", alpha=0.8
)
axes[1].scatter(
    centroids[:, 0],
    centroids[:, 1],
    marker="X",
    s=200,
    c="black",
    edgecolors="white",
    linewidths=1.5,
    label="Centroids",
)
axes[1].set_title("K-Means Clusters (scaled space)")
axes[1].set_xlabel("Feature 1 (scaled)")
axes[1].set_ylabel("Feature 2 (scaled)")
axes[1].legend(loc="best")

# (c) Elbow plot (inertia vs k)
axes[2].plot(ks, inertias, marker="o")
axes[2].set_xticks(list(ks))
axes[2].set_title("Elbow Method (Inertia vs k)")
axes[2].set_xlabel("k (number of clusters)")
axes[2].set_ylabel("Inertia (sum of squared distances)")

plt.tight_layout()
plt.show()