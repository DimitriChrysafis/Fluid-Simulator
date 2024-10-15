import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as a
from tqdm import tqdm
from scipy.ndimage import gaussian_filter as g, laplace as l
from matplotlib.colors import ListedColormap, LinearSegmentedColormap
import random

n, m = 5000, 5000
dx, dy = 0.05, 0.05
c = 1.5
dt = 0.01
nt = 10000
d = 0.9997

u = np.zeros((m, n))
u_prev = np.zeros_like(u)

s = 30

def f(c, s, v=0.1):
    y, x = c
    y_min, y_max = max(y - s, 0), min(y + s, m)
    x_min, x_max = max(x - s, 0), min(x + s, n)
    X, Y = np.meshgrid(np.arange(x_min, x_max), np.arange(y_min, y_max))
    d = np.exp(-((X - x) ** 2 + (Y - y) ** 2) / (2 * (s / 2.0) ** 2))
    u[y_min:y_max, x_min:x_max] += d * v

f((int(m / 2), int(n / 4)), s)
f((int(m / 2), int(3 * n / 4)), s)

o = np.zeros_like(u)
for _ in range(10):
    x_start = np.random.randint(0, n - 70)
    y_start = np.random.randint(0, m - 70)
    w = np.random.randint(20, 70)
    h = np.random.randint(20, 70)
    sh = np.random.choice(['r', 'e'])
    if sh == 'r':
        o[y_start:y_start + h, x_start:x_start + w] = 1
    elif sh == 'e':
        y, x = np.ogrid[-h // 2: h // 2, -w // 2: w // 2]
        mask = x ** 2 / (w / 2) ** 2 + y ** 2 / (h / 2) ** 2 <= 1
        o[y_start:y_start + h, x_start:x_start + w] = mask

u[o == 1] = 0
u_prev[o == 1] = 0

cmap_colors = [(0, 0, 0.3), (0, 0.2, 0.7), (0.1, 0.5, 1), (0.5, 0.7, 1), (1, 1, 1)]
cm = LinearSegmentedColormap.from_list('cw', cmap_colors, N=100)
new_cmap = ListedColormap(cm(np.linspace(0, 1, 256)))

fig, ax = plt.subplots(figsize=(16, 9), dpi=100)
im = ax.imshow(u, cmap=new_cmap, vmin=-0.5, vmax=0.5, interpolation='none')

def u(frame):
    global u, u_prev
    if frame % 100 == 0:
        d_center = (np.random.randint(0, m), np.random.randint(0, n))
        f(d_center, np.random.randint(20, 50))
        u += np.random.normal(0, 0.01, (m, n))

    lap = l(u, mode='nearest') / (dx * dy)
    u_new = d * (2 * u - u_prev + (c * dt) ** 2 * lap)
    u_new[o == 1] = 0

    u_prev = u.copy()
    u = u_new.copy()

    p = np.clip(u, -0.5, 0.5)
    p_colored = (p - p.min()) / (p.max() - p.min())
    p_colored = np.clip(p_colored, 0, 1)

    u_s = g(u, sigma=1)
    grad_y, grad_x = np.gradient(u_s)
    nrm = np.sqrt(grad_x**2 + grad_y**2 + 1)
    n_map = np.array([grad_x / nrm, grad_y / nrm, 1 / nrm])

    l_a = np.array([0.5, 0.5, 1.0])
    l_a /= np.linalg.norm(l_a)
    sh = np.einsum('i,ijk->jk', l_a, n_map)
    spec = np.power(np.clip(sh, 0, 1), 30)

    u_lit = new_cmap(p_colored)[:, :, :3] * (1 + sh[..., None] * 0.5)
    u_lit += spec[..., None] * np.array([1.0, 0.8, 0.6])

    f_mask = np.abs(u_s) > 0.3
    u_lit[f_mask] += 0.5
    u_lit[o == 1] = [0, 0, 0]

    d_mask = np.abs(u) > 0.2
    u_lit[d_mask] = np.array([0, 0.1, 0.2])

    p_density = 0.0005
    p = np.random.rand(*u.shape) < p_density
    u_lit[p] = [1, 1, 1]

    u_lit = np.clip(u_lit, 0, 1)
    im.set_array(u_lit)

    return [im]

ani = a.FuncAnimation(fig, u, frames=tqdm(range(nt)), blit=True, interval=10)

ani.save('super_realistic_water_simulation.mp4', writer='ffmpeg', fps=60, bitrate=4000)
plt.show()
