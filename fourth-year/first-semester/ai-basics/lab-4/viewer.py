import numpy as np
import matplotlib.pyplot as plt

# =============================================
# ЗАГРУЗКА
# =============================================
data = np.load("dataset.npz")
X_flat = data['X']  # (N, 900)
Y_onehot = data['Y']  # (N, 8)

X = X_flat.reshape(-1, 30, 30)
y = np.argmax(Y_onehot, axis=1)

N = len(X)
print(f"Всего изображений в датасете: {N} (по {N // 8} на класс)")

class_names = [
    "Intersection ∩", "Union ∪", "Difference ∖", "Symmetric Δ",
    "Empty ∅", "Universal U", "Inclusion ⊆", "Strict ⊂"
]

# =============================================
# 1. Случайные примеры (автоматически берём не больше, чем есть)
# =============================================
n_show = min(60, N)  # ← вот и всё исправление!
print(f"Показываю {n_show} случайных изображений")

plt.figure(figsize=(18, 3.3 * (n_show // 10 + 1)))
indices = np.random.choice(N, n_show, replace=False)

for i, idx in enumerate(indices):
    plt.subplot(n_show // 10 + 1, 10, i + 1)
    plt.imshow(X[idx], cmap='gray', vmin=0, vmax=1)
    plt.title(f"{class_names[y[idx]]}\n(class {y[idx]})", fontsize=11)
    plt.axis('off')

plt.suptitle(f"{n_show} случайных примеров из датасета (всего {N} изображений)",
             fontsize=20, y=0.98)
plt.tight_layout()
plt.show()

# =============================================
# 2. По 8 примеров каждого класса (если в классе меньше — покажет всё, что есть)
# =============================================
max_per_class = 1
plt.figure(figsize=(16, 16))

pos = 1
for cls in range(8):
    idxs = np.where(y == cls)[0]
    show_idxs = idxs[:max_per_class]  # не больше max_per_class
    for j, idx in enumerate(show_idxs):
        plt.subplot(8, max_per_class, pos)
        plt.imshow(X[idx], cmap='gray', vmin=0, vmax=1)
        if j == 0:
            plt.ylabel(class_names[cls], fontsize=14, rotation=0, labelpad=80, va='center')
        plt.title(cls, fontsize=12)
        plt.axis('off')
        pos += 1

    # Если в классе меньше 8 примеров — оставшиеся ячейки пропускаем
    pos += (max_per_class - len(show_idxs))

plt.suptitle(f"По до {max_per_class} примеров каждого класса (всего в датасете {N} изображений)",
             fontsize=20, y=0.93)
plt.tight_layout()
plt.show()

# =============================================
# 3. Гистограмма
# =============================================
plt.figure(figsize=(12, 6))
counts = np.bincount(y, minlength=8)
plt.bar(range(8), counts, color='teal', alpha=0.8, edgecolor='black')
plt.xticks(range(8), class_names, rotation=20, ha='right')
plt.title("Распределение по классам", fontsize=16)
plt.ylabel("Количество")
for i, c in enumerate(counts):
    plt.text(i, c + max(counts) * 0.02, str(c), ha='center', va='bottom', fontsize=12)
plt.grid(axis='y', alpha=0.3)
plt.tight_layout()
plt.show()