import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

# ------------------------------------------------------------
# 1) Fonction de coût et gradient
#    J(x, y) = (x - 2)^2 + (y + 1)^2
# ------------------------------------------------------------
def J(x, y):
    return (x - 2)**2 + (y + 1)**2

def grad_J(x, y):
    return np.array([2 * (x - 2), 2 * (y + 1)])

# ------------------------------------------------------------
# 2) Paramètres de la descente de gradient
# ------------------------------------------------------------
alpha = 0.20
n_steps = 18
theta = np.array([-4.0, 4.0])  # point de départ

# Historique
path = [theta.copy()]
costs = [J(theta[0], theta[1])]
grad_norms = [np.linalg.norm(grad_J(theta[0], theta[1]))]
steps = []
grads = []

# ------------------------------------------------------------
# 3) Descente de gradient
# ------------------------------------------------------------
for _ in range(n_steps):
    g = grad_J(theta[0], theta[1])
    grads.append(g.copy())

    new_theta = theta - alpha * g
    steps.append(new_theta - theta)

    theta = new_theta
    path.append(theta.copy())
    costs.append(J(theta[0], theta[1]))
    grad_norms.append(np.linalg.norm(grad_J(theta[0], theta[1])))

path = np.array(path)
costs = np.array(costs)
grad_norms = np.array(grad_norms)
steps = np.array(steps)
grads = np.array(grads)

print("Point final :", path[-1])
print("Coût final :", costs[-1])

# ------------------------------------------------------------
# 4) Grille pour les visualisations
# ------------------------------------------------------------
xx = np.linspace(-6, 6, 300)
yy = np.linspace(-6, 6, 300)
X, Y = np.meshgrid(xx, yy)
Z = J(X, Y)

# ============================================================
# FIGURE 1 — Courbes de niveau + trajectoire + flèches
# ============================================================
plt.figure(figsize=(7, 6))
plt.contour(X, Y, Z, levels=25)
plt.plot(path[:, 0], path[:, 1], marker="o")
plt.scatter(2, -1, marker="X", s=150)
plt.title("Gradient Descent on Contours of J(x, y)")
plt.xlabel("x")
plt.ylabel("y")

for t in range(len(grads)):
    x_t, y_t = path[t]
    g = grads[t]
    step = steps[t]

    # Gradient (direction de montée)
    plt.arrow(x_t, y_t, 0.25 * g[0], 0.25 * g[1],
              head_width=0.1, length_includes_head=True)

    # Pas de descente (-∇J)
    plt.arrow(x_t, y_t, step[0], step[1],
              head_width=0.15, length_includes_head=True)

plt.show()

# ============================================================
# FIGURE 2 — Surface 3D + trajectoire
# ============================================================
fig = plt.figure(figsize=(7, 6))
ax = fig.add_subplot(111, projection="3d")
ax.plot_surface(X, Y, Z, alpha=0.6, rstride=6, cstride=6)
ax.plot(path[:, 0], path[:, 1], J(path[:, 0], path[:, 1]), marker="o")
ax.scatter(2, -1, J(2, -1), marker="X", s=150)
ax.set_title("3D Surface of J(x, y) with GD Path")
ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_zlabel("J")
plt.show()

# ============================================================
# FIGURE 3 — Coût J en fonction des itérations
# ============================================================
plt.figure(figsize=(7, 4))
plt.plot(range(len(costs)), costs, marker="o")
plt.title("Cost J vs Iteration")
plt.xlabel("Iteration")
plt.ylabel("J")
plt.grid(True)
plt.show()

# ============================================================
# FIGURE 4 — Norme du gradient en fonction des itérations
# ============================================================
plt.figure(figsize=(7, 4))
plt.plot(range(len(grad_norms)), grad_norms, marker="o")
plt.title("Gradient Norm ||∇J|| vs Iteration")
plt.xlabel("Iteration")
plt.ylabel("||∇J||")
plt.grid(True)
plt.show()
