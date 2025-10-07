import tkinter as tk
from tkinter import ttk
import random
import math


class AntColonyTSP:
    def __init__(self, graph, ants, alpha, beta, rho, iterations):
        self.graph = graph
        self.num_nodes = len(graph)
        self.ants = ants
        self.alpha = alpha
        self.beta = beta
        self.rho = rho
        self.iterations = iterations
        self.pheromone = [[1 for _ in range(self.num_nodes)] for _ in range(self.num_nodes)]

    def run(self, update_callback=None):
        best_path = None
        best_length = float('inf')

        for it in range(self.iterations):
            all_paths = []
            for _ in range(self.ants):
                path = self.construct_solution()
                length = self.path_length(path)
                all_paths.append((path, length))
                if length < best_length:
                    best_path, best_length = path, length

            self.update_pheromone(all_paths)

            if update_callback:
                update_callback(best_path, best_length, it)

        return best_path, best_length

    def construct_solution(self):
        path = [random.randint(0, self.num_nodes - 1)]
        while len(path) < self.num_nodes:
            current = path[-1]
            probs = []
            for j in range(self.num_nodes):
                if j not in path:
                    tau = self.pheromone[current][j] ** self.alpha
                    eta = (1 / self.graph[current][j]) ** self.beta
                    probs.append((j, tau * eta))
            total = sum(p for _, p in probs)
            r = random.random() * total
            s = 0
            for j, p in probs:
                s += p
                if s >= r:
                    path.append(j)
                    break
        return path

    def path_length(self, path):
        length = 0
        for i in range(len(path) - 1):
            length += self.graph[path[i]][path[i + 1]]
        length += self.graph[path[-1]][path[0]]
        return length

    def update_pheromone(self, all_paths):
        # испарение
        for i in range(self.num_nodes):
            for j in range(self.num_nodes):
                self.pheromone[i][j] *= (1 - self.rho)

        # обновление
        for path, length in all_paths:
            for i in range(len(path) - 1):
                a, b = path[i], path[i + 1]
                self.pheromone[a][b] += 1 / length
                self.pheromone[b][a] += 1 / length
            self.pheromone[path[-1]][path[0]] += 1 / length
            self.pheromone[path[0]][path[-1]] += 1 / length


class TSPApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Муравьиный алгоритм - Задача коммивояжера")

        # UI: параметры
        frm = ttk.Frame(root)
        frm.pack(side=tk.LEFT, fill=tk.Y)

        self.params = {}
        for text, key, val in [
            ("Муравьи", "ants", 30),
            ("α (влияние феромона)", "alpha", 1),
            ("β (влияние расстояния)", "beta", 5),
            ("ρ (испарение)", "rho", 0.5),
            ("Итерации", "iterations", 50),
        ]:
            ttk.Label(frm, text=text).pack()
            ent = ttk.Entry(frm)
            ent.insert(0, str(val))
            ent.pack()
            self.params[key] = ent

        ttk.Button(frm, text="Запустить", command=self.run_algorithm).pack(pady=10)

        # Canvas для визуализации
        self.canvas = tk.Canvas(root, width=800, height=600, bg="white")
        self.canvas.pack(side=tk.RIGHT, expand=True, fill=tk.BOTH)

        self.nodes = []
        self.graph = []
        self.generate_graph(20)

    def generate_graph(self, n):
        self.nodes = [(random.randint(50, 750), random.randint(50, 550)) for _ in range(n)]
        self.graph = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(n):
                if i != j:
                    x1, y1 = self.nodes[i]
                    x2, y2 = self.nodes[j]
                    self.graph[i][j] = math.dist((x1, y1), (x2, y2))

    def draw_graph(self, path=None):
        self.canvas.delete("all")
        # рёбра
        for i in range(len(self.nodes)):
            for j in range(i + 1, len(self.nodes)):
                x1, y1 = self.nodes[i]
                x2, y2 = self.nodes[j]
                self.canvas.create_line(x1, y1, x2, y2, fill="lightgray")
        # лучший путь
        if path:
            for i in range(len(path) - 1):
                x1, y1 = self.nodes[path[i]]
                x2, y2 = self.nodes[path[i + 1]]
                self.canvas.create_line(x1, y1, x2, y2, fill="red", width=2)
            x1, y1 = self.nodes[path[-1]]
            x2, y2 = self.nodes[path[0]]
            self.canvas.create_line(x1, y1, x2, y2, fill="red", width=2)
        # вершины
        for i, (x, y) in enumerate(self.nodes):
            self.canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="blue")
            self.canvas.create_text(x, y - 10, text=str(i))

    def run_algorithm(self):
        ants = int(self.params["ants"].get())
        alpha = float(self.params["alpha"].get())
        beta = float(self.params["beta"].get())
        rho = float(self.params["rho"].get())
        iterations = int(self.params["iterations"].get())

        colony = AntColonyTSP(self.graph, ants, alpha, beta, rho, iterations)

        def update_callback(path, length, it):
            self.draw_graph(path)
            self.root.update()
            self.root.title(f"Итерация {it+1}/{iterations}, длина: {length:.2f}")

        best_path, best_length = colony.run(update_callback)
        self.draw_graph(best_path)
        self.root.title(f"Готово! Лучшая длина: {best_length:.2f}")


if __name__ == "__main__":
    root = tk.Tk()
    app = TSPApp(root)
    root.mainloop()
