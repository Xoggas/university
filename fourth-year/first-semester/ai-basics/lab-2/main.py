import tkinter as tk
from tkinter import ttk, messagebox
from typing import List, Tuple
import random
import copy

# ---------------------------
# ART1 implementation
# ---------------------------

class ART1:
    def __init__(self, d: int, max_clusters: int, rho: float = 0.6, alpha: float = 0.0001, beta: float = 1.0, soft_update: bool=False):
        """
        d: размер вектора (длина)
        max_clusters: NN
        rho: vigilance (0..1)
        alpha: choice parameter (малое >0)
        beta: скорость обучения (0..1). beta=1 -> быстрое обучение
        soft_update: если True, использовать сглаженное обновление с beta
        """
        self.d = d
        self.max_clusters = max_clusters
        self.rho = rho
        self.alpha = alpha
        self.beta = beta
        self.soft_update = soft_update

        # Список прототипов кластеров (каждый - список 0/1 длины d)
        self.prototypes: List[List[int]] = []  # P_j
        # Список индексов векторов, отнесенных к каждому кластеру
        self.assignments: List[List[int]] = []

    def reset(self):
        self.prototypes = []
        self.assignments = []

    @staticmethod
    def dot_and(a: List[int], b: List[int]) -> int:
        return sum(1 for x,y in zip(a,b) if x and y)

    @staticmethod
    def ones_count(a: List[int]) -> int:
        return sum(a)

    def choice_value(self, I: List[int], P: List[int]) -> float:
        numerator = self.dot_and(I, P)
        denom = self.alpha + self.ones_count(P)
        return numerator / denom if denom != 0 else 0.0

    def vigilance_test(self, I: List[int], P: List[int]) -> bool:
        num = self.dot_and(I, P)
        denom = max(1, self.ones_count(I))  # защититься от нуля
        return (num / denom) >= self.rho

    def update_prototype(self, P_old: List[int], I: List[int]) -> List[int]:
        if self.soft_update and 0.0 < self.beta < 1.0:
            # мягкое обновление: P_new = round(beta*(I & P_old) + (1-beta)*P_old)
            new = []
            for po, ii in zip(P_old, I):
                inter = 1 if (po and ii) else 0
                val = self.beta * inter + (1 - self.beta) * po
                new.append(1 if val >= 0.5 else 0)
            return new
        else:
            # жёсткое ART1: пересечение
            return [1 if (po and ii) else 0 for po, ii in zip(P_old, I)]

    def fit(self, data: List[List[int]]):
        """
        Основной цикл ART1 (он-плайн, по одному образцу)
        data: список бинарных векторов длины d
        """
        self.reset()
        for idx, I in enumerate(data):
            # если I пустой (нет единиц) — пропуск
            if self.ones_count(I) == 0:
                # можно либо пропустить, либо создать отдельный кластер; пропустим
                continue

            # Если пока нет прототипов -> создать новый кластер
            if not self.prototypes:
                self.prototypes.append(I.copy())
                self.assignments.append([idx])
                continue

            # Вычислим значения выбора для всех кластеров, помеченные как доступные
            tried = set()
            assigned = False

            while True:
                candidates = []
                for j, P in enumerate(self.prototypes):
                    if j in tried:
                        continue
                    Tj = self.choice_value(I, P)
                    candidates.append((Tj, j))
                if not candidates:
                    break
                candidates.sort(reverse=True)  # максимум сверху
                Tj, jstar = candidates[0]
                Pj = self.prototypes[jstar]
                # Проверка vigilance
                if self.vigilance_test(I, Pj):
                    # присоединяем и обновляем прототип
                    newP = self.update_prototype(Pj, I)
                    self.prototypes[jstar] = newP
                    self.assignments[jstar].append(idx)
                    assigned = True
                    break
                else:
                    # блокируем этот кластер для текущего образца
                    tried.add(jstar)
                    continue

            if not assigned:
                # создать новый кластер если возможно
                if len(self.prototypes) < self.max_clusters:
                    self.prototypes.append(I.copy())
                    self.assignments.append([idx])
                else:
                    # если достигнут максимум — присвоить к "наиболее близкому" без vigilance (fallback)
                    # найдем кластер с max Tj и добавим туда (это вариант политики при полном кластере)
                    best_j = None
                    best_T = -1.0
                    for j, P in enumerate(self.prototypes):
                        T = self.choice_value(I, P)
                        if T > best_T:
                            best_T = T
                            best_j = j
                    if best_j is not None:
                        # добавим, но не обновим прототип (или можно обновить)
                        self.assignments[best_j].append(idx)
                        # опционально обновим: жестко пересечением
                        self.prototypes[best_j] = self.update_prototype(self.prototypes[best_j], I)

    def predict(self, data: List[List[int]]) -> List[int]:
        """Вернуть индекс кластера для каждого вектора (или -1 если не отнесён)"""
        res = [-1]*len(data)
        for j, inds in enumerate(self.assignments):
            for i in inds:
                res[i] = j
        return res

# ---------------------------
# Utilities: демонстрация и генерация данных
# ---------------------------

def generate_demo_data(d: int, n_vectors: int) -> Tuple[List[List[int]], List[List[str]]]:
    """
    Создаёт демонстрационные бинарные векторы и сопоставляет каждой позиции строковый код типа "A1","A2",...
    Стратегия: строим несколько шаблонов поставок и генерируем векторы с шумом.
    Возвращает (data, feature_names_rows) где feature_names_rows - список имён длины d (строк)
    """
    # Сформируем имена признаков: A1..A{m}, B1.., C1.. пока надо
    feature_names = []
    letters = ['A','B','C','D','E','F']
    per_letter = max(1, d // len(letters))
    i = 0
    while len(feature_names) < d:
        letter = letters[i % len(letters)]
        num = (len(feature_names) // len(letters)) + 1
        feature_names.append(f"{letter}{num}")
        i += 1
    feature_names = feature_names[:d]

    # Сделаем 3-4 шаблона (кластера) и генерируем векторы вокруг них.
    base_patterns = []
    k_patterns = min(4, d)
    for k in range(k_patterns):
        pat = [0]*d
        # для шаблона случайно включим 3..(d//2) позиций
        ones = random.randint(max(1, d//6), max(1, d//2))
        ones_indices = random.sample(range(d), ones)
        for idx in ones_indices:
            pat[idx] = 1
        base_patterns.append(pat)

    data = []
    for _ in range(n_vectors):
        # выберем случайный шаблон и добавим/уберем несколько битов (шум)
        base = random.choice(base_patterns)
        v = base.copy()
        # шум: инвертируем 0->1 с вероятностью p_add и 1->0 с вероятностью p_remove
        p_add = 0.05
        p_remove = 0.05
        for i in range(d):
            r = random.random()
            if v[i] == 0 and r < p_add:
                v[i] = 1
            elif v[i] == 1 and r < p_remove:
                v[i] = 0
        data.append(v)
    return data, feature_names

# ---------------------------
# GUI (tkinter)
# ---------------------------

class ART1App:
    def __init__(self, root):
        self.root = root
        root.title("ART1 Clustering — Демонстрация (Python)")
        self.frame = ttk.Frame(root, padding=8)
        self.frame.grid(row=0, column=0, sticky="nsew")

        # параметры по умолчанию
        self.d_var = tk.IntVar(value=11)   # размерность по умолчанию
        self.NN_var = tk.IntVar(value=10)  # максимальное число кластеров
        self.rho_var = tk.DoubleVar(value=0.6)
        self.alpha_var = tk.DoubleVar(value=0.0001)
        self.beta_var = tk.DoubleVar(value=1.0)
        self.soft_var = tk.BooleanVar(value=False)
        self.nvectors_var = tk.IntVar(value=12)

        # Верхняя панель настроек
        settings = ttk.LabelFrame(self.frame, text="Параметры ART1")
        settings.grid(row=0, column=0, sticky="ew", padx=4, pady=4)

        ttk.Label(settings, text="d (размер вектора):").grid(row=0, column=0, sticky="w")
        ttk.Entry(settings, textvariable=self.d_var, width=6).grid(row=0, column=1, sticky="w")
        ttk.Label(settings, text="NN (макс кластеров):").grid(row=0, column=2, sticky="w", padx=(10,0))
        ttk.Entry(settings, textvariable=self.NN_var, width=6).grid(row=0, column=3, sticky="w")
        ttk.Label(settings, text="n_vectors:").grid(row=0, column=4, sticky="w", padx=(10,0))
        ttk.Entry(settings, textvariable=self.nvectors_var, width=6).grid(row=0, column=5, sticky="w")

        ttk.Label(settings, text="rho (vigilance 0..1):").grid(row=1, column=0, sticky="w")
        ttk.Entry(settings, textvariable=self.rho_var, width=6).grid(row=1, column=1, sticky="w")
        ttk.Label(settings, text="alpha:").grid(row=1, column=2, sticky="w", padx=(10,0))
        ttk.Entry(settings, textvariable=self.alpha_var, width=6).grid(row=1, column=3, sticky="w")
        ttk.Label(settings, text="beta (0..1):").grid(row=1, column=4, sticky="w", padx=(10,0))
        ttk.Entry(settings, textvariable=self.beta_var, width=6).grid(row=1, column=5, sticky="w")
        ttk.Checkbutton(settings, text="Soft update", variable=self.soft_var).grid(row=2, column=0, sticky="w")

        # Кнопки действий
        actions = ttk.Frame(self.frame)
        actions.grid(row=1, column=0, sticky="ew", pady=(6,4))
        ttk.Button(actions, text="Generate demo data", command=self.generate_demo).grid(row=0, column=0, padx=4)
        ttk.Button(actions, text="Run ART1", command=self.run_art).grid(row=0, column=1, padx=4)
        ttk.Button(actions, text="Clear", command=self.clear).grid(row=0, column=2, padx=4)

        # Два дерева: входные векторы и кластеры
        display = ttk.Panedwindow(self.frame, orient=tk.HORIZONTAL)
        display.grid(row=2, column=0, sticky="nsew")
        self.left_frame = ttk.Labelframe(display, text="Входные векторы (индекс: вектор)", width=400)
        self.right_frame = ttk.Labelframe(display, text="Результат: кластеры", width=400)
        display.add(self.left_frame, weight=1)
        display.add(self.right_frame, weight=1)

        # Treeview для входных векторов
        self.tree_in = ttk.Treeview(self.left_frame, columns=("vec",), show="headings", height=20)
        self.tree_in.heading("vec", text="Вектор (1/0)")
        self.tree_in.grid(row=0, column=0, sticky="nsew")
        self.left_frame.rowconfigure(0, weight=1)
        self.left_frame.columnconfigure(0, weight=1)

        # Treeview для кластеров
        self.tree_clusters = ttk.Treeview(self.right_frame, columns=("members","prototype"), show="headings", height=20)
        self.tree_clusters.heading("members", text="Члены (индексы)")
        self.tree_clusters.heading("prototype", text="Прототип (вектор)")
        self.tree_clusters.grid(row=0, column=0, sticky="nsew")
        self.right_frame.rowconfigure(0, weight=1)
        self.right_frame.columnconfigure(0, weight=1)

        # статус
        self.status = ttk.Label(self.frame, text="Готово")
        self.status.grid(row=3, column=0, sticky="w", pady=(6,0))

        # внутренние данные
        self.data = []  # список векторов
        self.feature_names = []  # имена признаков
        self.art = None

    def generate_demo(self):
        d = int(self.d_var.get())
        n = int(self.nvectors_var.get())
        self.data, self.feature_names = generate_demo_data(d, n)
        self.populate_input_tree()
        self.status.config(text=f"Сгенерировано {len(self.data)} векторов, d={d}")

    def populate_input_tree(self):
        for i in self.tree_in.get_children():
            self.tree_in.delete(i)
        for idx, vec in enumerate(self.data):
            s = "".join(str(x) for x in vec)
            # можно показывать короткие имена (через запятую)
            display = s
            self.tree_in.insert("", "end", values=(f"{idx}: {display}",))

    def run_art(self):
        try:
            d = int(self.d_var.get())
            NN = int(self.NN_var.get())
            rho = float(self.rho_var.get())
            alpha = float(self.alpha_var.get())
            beta = float(self.beta_var.get())
            soft = bool(self.soft_var.get())
        except Exception as e:
            messagebox.showerror("Ошибка", f"Некорректные параметры: {e}")
            return

        # Проверки
        if d <= 0 or NN <= 0 or not (0 <= rho <= 1):
            messagebox.showerror("Ошибка", "Проверьте d, NN, rho")
            return

        # Если данные пусты — генерируем демонстрационные
        if not self.data:
            self.generate_demo()

        # Усечём векторы до d если надо
        data_cut = [vec[:d] + [0]*(d - len(vec)) if len(vec) < d else vec[:d] for vec in self.data]

        self.art = ART1(d=d, max_clusters=NN, rho=rho, alpha=alpha, beta=beta, soft_update=soft)
        self.art.fit(data_cut)
        self.populate_clusters()
        self.status.config(text=f"Готово. Кластеров: {len(self.art.prototypes)}")

    def populate_clusters(self):
        # очистка
        for i in self.tree_clusters.get_children():
            self.tree_clusters.delete(i)

        if not self.art:
            return

        for j, P in enumerate(self.art.prototypes):
            members = self.art.assignments[j] if j < len(self.art.assignments) else []
            s_members = ",".join(str(x) for x in members)
            s_proto = "".join(str(x) for x in P)
            self.tree_clusters.insert("", "end", values=(s_members, s_proto))

    def clear(self):
        self.data = []
        self.feature_names = []
        for i in self.tree_in.get_children():
            self.tree_in.delete(i)
        for i in self.tree_clusters.get_children():
            self.tree_clusters.delete(i)
        self.status.config(text="Очищено")

def main():
    root = tk.Tk()
    # Развернуть окно
    root.geometry("1000x600")
    app = ART1App(root)
    root.mainloop()

if __name__ == "__main__":
    main()
