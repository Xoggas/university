import tkinter as tk
from tkinter import ttk, messagebox
import numpy as np
from PIL import Image, ImageDraw, ImageOps
import tkinter.filedialog

from PIL.Image import Resampling

CANVAS_DISPLAY_SIZE = 300
CANVAS_SIZE = 30
INPUT_NEURONS = CANVAS_SIZE * CANVAS_SIZE
HIDDEN_NEURONS = 6
OUTPUT_LABELS = [
    "Intersection (∩)",
    "Union (∪)",
    "Difference (A \\ B)",
    "Symmetric diff (Δ)",
    "Empty set (∅)",
    "Universal set (U)",
    "Inclusion (⊆)",
    "Strict inclusion (⊂)"
]
OUTPUT_NEURONS = len(OUTPUT_LABELS)


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def sigmoid_deriv(s):
    return s * (1 - s)


class SimpleMLP:
    """
    Поддерживает архитектуры:
     - 0 hidden: input -> output (weights_input_output)
     - 1 hidden: input -> hidden -> output
     - 2 hidden: input -> hidden1 -> hidden2 -> output
    All activations: sigmoid.
    Loss: MSE.
    """

    def __init__(self, n_in, n_hidden_layers, hidden_size, n_out, seed=None):
        if seed is not None:
            np.random.seed(seed)
        self.n_in = n_in
        self.n_hidden_layers = n_hidden_layers
        self.hidden_size = hidden_size
        self.n_out = n_out

        self._init_weights()

    def _init_weights(self):

        if self.n_hidden_layers == 0:
            self.W_io = np.random.randn(self.n_in, self.n_out) * 0.1
            self.b_o = np.zeros((1, self.n_out))
        elif self.n_hidden_layers == 1:
            self.W_ih = np.random.randn(self.n_in, self.hidden_size) * 0.1
            self.b_h = np.zeros((1, self.hidden_size))
            self.W_ho = np.random.randn(self.hidden_size, self.n_out) * 0.1
            self.b_o = np.zeros((1, self.n_out))
        elif self.n_hidden_layers == 2:
            self.W_ih = np.random.randn(self.n_in, self.hidden_size) * 0.1
            self.b_h1 = np.zeros((1, self.hidden_size))
            self.W_hh = np.random.randn(self.hidden_size, self.hidden_size) * 0.1
            self.b_h2 = np.zeros((1, self.hidden_size))
            self.W_ho = np.random.randn(self.hidden_size, self.n_out) * 0.1
            self.b_o = np.zeros((1, self.n_out))
        else:
            raise ValueError("Only 0,1,2 hidden layers supported")

    def forward(self, X):

        if self.n_hidden_layers == 0:
            z_o = X @ self.W_io + self.b_o
            a_o = sigmoid(z_o)
            return {"out": a_o}
        elif self.n_hidden_layers == 1:
            z_h = X @ self.W_ih + self.b_h
            a_h = sigmoid(z_h)
            z_o = a_h @ self.W_ho + self.b_o
            a_o = sigmoid(z_o)
            return {"h": a_h, "out": a_o}
        else:
            z_h1 = X @ self.W_ih + self.b_h1
            a_h1 = sigmoid(z_h1)
            z_h2 = a_h1 @ self.W_hh + self.b_h2
            a_h2 = sigmoid(z_h2)
            z_o = a_h2 @ self.W_ho + self.b_o
            a_o = sigmoid(z_o)
            return {"h1": a_h1, "h2": a_h2, "out": a_o}

    def train(self, X, Y, epochs=10, alpha=0.1, progress_callback=None, error_callback=None):
        """
        X: (n_samples, n_in), Y: (n_samples, n_out) - targets (0/1)
        progress_callback(epoch, total_epochs, avg_error) - optional
        error_callback(avg_error) - optional (for final)
        """
        n = X.shape[0]
        for epoch in range(1, epochs + 1):

            cache = self.forward(X)
            out = cache["out"]

            err = out - Y
            mse = np.mean(err ** 2)

            d_out = err * sigmoid_deriv(out)
            if self.n_hidden_layers == 0:

                dW_io = X.T @ d_out / n
                db_o = np.mean(d_out, axis=0, keepdims=True)

                self.W_io -= alpha * dW_io
                self.b_o -= alpha * db_o
            elif self.n_hidden_layers == 1:
                a_h = cache["h"]
                dW_ho = a_h.T @ d_out / n
                db_o = np.mean(d_out, axis=0, keepdims=True)
                d_h = d_out @ self.W_ho.T * sigmoid_deriv(a_h)
                dW_ih = X.T @ d_h / n
                db_h = np.mean(d_h, axis=0, keepdims=True)

                self.W_ho -= alpha * dW_ho
                self.b_o -= alpha * db_o
                self.W_ih -= alpha * dW_ih
                self.b_h -= alpha * db_h
            else:
                a_h1 = cache["h1"]
                a_h2 = cache["h2"]
                dW_ho = a_h2.T @ d_out / n
                db_o = np.mean(d_out, axis=0, keepdims=True)
                d_h2 = d_out @ self.W_ho.T * sigmoid_deriv(a_h2)
                dW_hh = a_h1.T @ d_h2 / n
                db_h2 = np.mean(d_h2, axis=0, keepdims=True)
                d_h1 = d_h2 @ self.W_hh.T * sigmoid_deriv(a_h1)
                dW_ih = X.T @ d_h1 / n
                db_h1 = np.mean(d_h1, axis=0, keepdims=True)

                self.W_ho -= alpha * dW_ho
                self.b_o -= alpha * db_o
                self.W_hh -= alpha * dW_hh
                self.b_h2 -= alpha * db_h2
                self.W_ih -= alpha * dW_ih
                self.b_h1 -= alpha * db_h1
            print(f"Epoch: {epoch}, Avg error: {mse:.6f}")
            if progress_callback:
                progress_callback(epoch, epochs, mse)
        if error_callback:
            error_callback(mse)

    def predict(self, X):
        return self.forward(X)["out"]


class App:
    def __init__(self, root):
        self.root = root
        root.title("Set-ops recognizer (30×30 canvas)")

        self.samples_X = []
        self.samples_Y = []

        self.n_hidden_layers_var = tk.IntVar(value=0)
        self.epochs_var = tk.IntVar(value=500)
        self.alpha_var = tk.DoubleVar(value=0.1)

        self.model = SimpleMLP(INPUT_NEURONS, 0, HIDDEN_NEURONS, OUTPUT_NEURONS, seed=1)

        self._build_ui()

        self._init_canvas_image()
        self.drawing = False

        self.canvas.bind("<B1-Motion>", self.on_draw)
        self.canvas.bind("<ButtonRelease-1>", self.on_release)
        self.canvas.bind("<Button-1>", self.on_start)

        self.update_recognition()

    def load_dataset(self):
        filepath = tk.filedialog.askopenfilename(
            title="Загрузить датасет",
            filetypes=[("NPZ файлы", "*.npz"), ("Все файлы", "*.*")]
        )
        if not filepath:
            return

        try:
            data = np.load(filepath)
            X = data['X']
            Y = data['Y']

            if X.shape[1] != INPUT_NEURONS or Y.shape[1] != OUTPUT_NEURONS:
                raise ValueError("Размеры данных не совпадают с сетью")

            self.samples_X = [X[i] for i in range(X.shape[0])]
            self.samples_Y = [Y[i] for i in range(Y.shape[0])]

            self.samples_count_lab.config(text=f"Samples: {len(self.samples_X)}")
            messagebox.showinfo("Готово", f"Датасет загружен!\n{len(self.samples_X)} примеров")
        except Exception as e:
            messagebox.showerror("Ошибка", f"Не удалось загрузить датасет:\n{e}")

    def _build_ui(self):
        main = ttk.Frame(self.root, padding=8)
        main.pack(fill=tk.BOTH, expand=True)

        left = ttk.Frame(main)
        left.grid(row=0, column=0, sticky="nsew", padx=(0, 8))
        right = ttk.Frame(main)
        right.grid(row=0, column=1, sticky="nsew")

        main.columnconfigure(0, weight=0)
        main.columnconfigure(1, weight=1)
        left.rowconfigure(0, weight=1)

        self.canvas = tk.Canvas(left, width=CANVAS_DISPLAY_SIZE, height=CANVAS_DISPLAY_SIZE, bg="black")
        self.canvas.grid(row=0, column=0)
        clear_btn = ttk.Button(left, text="Clear canvas", command=self.clear_canvas)
        clear_btn.grid(row=1, column=0, pady=6, sticky="ew")

        lbl_ops = ttk.Label(right, text="Recognizable symbols (operations):", font=("Arial", 10, "bold"))
        lbl_ops.pack(anchor="w")

        self.outputs_frame = ttk.Frame(right)
        self.outputs_frame.pack(fill=tk.X, pady=4)
        self.output_labels_widgets = []
        for i, name in enumerate(OUTPUT_LABELS):
            f = ttk.Frame(self.outputs_frame)
            f.pack(fill=tk.X, pady=2)
            lab = ttk.Label(f, text=name, width=28, anchor="w")
            lab.pack(side=tk.LEFT)
            val = ttk.Label(f, text="0.000", width=7, anchor="e")
            val.pack(side=tk.RIGHT)
            self.output_labels_widgets.append(val)

        rb_frame = ttk.LabelFrame(right, text="Hidden layers")
        rb_frame.pack(fill=tk.X, pady=8)
        ttk.Radiobutton(rb_frame, text="0", variable=self.n_hidden_layers_var, value=0,
                        command=self.on_config_change).pack(side=tk.LEFT, padx=6)
        ttk.Radiobutton(rb_frame, text="1", variable=self.n_hidden_layers_var, value=1,
                        command=self.on_config_change).pack(side=tk.LEFT, padx=6)
        ttk.Radiobutton(rb_frame, text="2", variable=self.n_hidden_layers_var, value=2,
                        command=self.on_config_change).pack(side=tk.LEFT, padx=6)
        ttk.Label(rb_frame, text=f"(hidden neurons fixed: {HIDDEN_NEURONS})").pack(side=tk.LEFT, padx=8)

        params_frame = ttk.Frame(right)
        params_frame.pack(fill=tk.X, pady=4)
        ttk.Label(params_frame, text="Epochs:").grid(row=0, column=0, sticky="w")
        self.epochs_entry = ttk.Entry(params_frame, textvariable=self.epochs_var, width=8)
        self.epochs_entry.grid(row=0, column=1, sticky="w", padx=4)
        ttk.Label(params_frame, text="Alpha:").grid(row=0, column=2, sticky="w", padx=(8, 0))
        self.alpha_entry = ttk.Entry(params_frame, textvariable=self.alpha_var, width=8)
        self.alpha_entry.grid(row=0, column=3, sticky="w", padx=4)

        sample_frame = ttk.LabelFrame(right, text="Dataset / Training")
        sample_frame.pack(fill=tk.X, pady=8)
        ttk.Label(sample_frame, text="Select label:").grid(row=0, column=0, sticky="w")
        self.label_choice = tk.StringVar(value=OUTPUT_LABELS[0])
        self.labels_dropdown = ttk.Combobox(sample_frame, values=OUTPUT_LABELS, textvariable=self.label_choice,
                                            state="readonly", width=28)
        self.labels_dropdown.grid(row=0, column=1, padx=6, pady=4)
        add_sample_btn = ttk.Button(sample_frame, text="Add sample", command=self.add_sample)
        add_sample_btn.grid(row=1, column=0, pady=4, sticky="ew")

        self.samples_count_lab = ttk.Label(sample_frame, text="Samples: 0")
        self.samples_count_lab.grid(row=1, column=1, sticky="w", padx=6)

        load_dataset_btn = ttk.Button(sample_frame, text="Load dataset", command=self.load_dataset)
        load_dataset_btn.grid(row=2, column=0, columnspan=2, pady=6, sticky="ew")

        train_btn = ttk.Button(sample_frame, text="Train", command=self.train_model)
        train_btn.grid(row=3, column=0, columnspan=2, pady=6, sticky="ew")

        self.progress = ttk.Progressbar(right, length=200, mode="determinate")
        self.progress.pack(pady=6)
        self.error_label = ttk.Label(right, text="Avg error: N/A")
        self.error_label.pack()

        self.pred_label = ttk.Label(right, text="Prediction: —", font=("Arial", 10, "bold"))
        self.pred_label.pack(pady=6)

    def _init_canvas_image(self):

        self.img = Image.new("L", (CANVAS_DISPLAY_SIZE, CANVAS_DISPLAY_SIZE), color=0)
        self.draw = ImageDraw.Draw(self.img)
        self.last_pos = None

    def on_start(self, event):
        self.drawing = True
        self.last_pos = (event.x, event.y)
        self._draw_point(event.x, event.y)

    def on_draw(self, event):
        if not self.drawing:
            return
        x, y = event.x, event.y
        if self.last_pos:

            self.draw.line([self.last_pos, (x, y)], fill=255, width=8)
            self.canvas.create_line(self.last_pos[0], self.last_pos[1], x, y, width=8, capstyle=tk.ROUND, smooth=True,
                                    fill="white")
        else:
            self._draw_point(x, y)
        self.last_pos = (x, y)

        self.update_recognition()

    def on_release(self, event):
        self.drawing = False
        self.last_pos = None
        self.update_recognition()

    def _draw_point(self, x, y):
        r = 4
        self.draw.ellipse([x - r, y - r, x + r, y + r], fill=255)
        self.canvas.create_oval(x - r, y - r, x + r, y + r, fill="white", outline="white")

    def clear_canvas(self):
        self.canvas.delete("all")
        self._init_canvas_image()
        self.update_recognition()

    def get_canvas_vector(self):

        small = self.img.resize((CANVAS_SIZE, CANVAS_SIZE), resample=Resampling.BILINEAR)

        arr = np.array(small).astype(np.float32)

        arr = arr / 255.0
        vec = arr.flatten()[np.newaxis, :]
        return vec

    def add_sample(self):
        x = self.get_canvas_vector()
        label = self.label_choice.get()
        idx = OUTPUT_LABELS.index(label)
        y = np.zeros((OUTPUT_NEURONS,), dtype=np.float32)
        y[idx] = 1.0
        self.samples_X.append(x.flatten())
        self.samples_Y.append(y)
        self.samples_count_lab.config(text=f"Samples: {len(self.samples_X)}")
        messagebox.showinfo("Sample added", f"Added sample for '{label}'. Total samples: {len(self.samples_X)}")

    def on_config_change(self):

        n_hidden = self.n_hidden_layers_var.get()
        self.model = SimpleMLP(INPUT_NEURONS, n_hidden, HIDDEN_NEURONS, OUTPUT_NEURONS, seed=None)
        self.update_recognition()

    def train_model(self):
        if len(self.samples_X) == 0:
            messagebox.showwarning("No data", "No samples in dataset. Add samples first.")
            return
        try:
            epochs = int(self.epochs_entry.get())
            alpha = float(self.alpha_entry.get())
        except Exception:
            messagebox.showerror("Invalid params", "Epochs must be integer, alpha must be float.")
            return
        X = np.vstack(self.samples_X)
        Y = np.vstack(self.samples_Y)

        n_hidden = self.n_hidden_layers_var.get()
        self.model = SimpleMLP(INPUT_NEURONS, n_hidden, HIDDEN_NEURONS, OUTPUT_NEURONS, seed=None)

        self.progress["maximum"] = epochs
        self.progress["value"] = 0
        self.error_label.config(text="Avg error: ...")
        self.root.update_idletasks()

        def progress_cb(epoch, total, mse):
            self.progress["value"] = epoch
            self.error_label.config(text=f"Avg error: {mse:.6f}")

            self.update_recognition()

            self.root.update_idletasks()

        def final_err_cb(mse):
            self.error_label.config(text=f"Avg error: {mse:.6f}")
            self.update_recognition()

        self.model.train(X, Y, epochs=epochs, alpha=alpha, progress_callback=progress_cb, error_callback=final_err_cb)
        messagebox.showinfo("Training finished",
                            f"Training completed. Final MSE: {float(np.mean((self.model.predict(X) - Y) ** 2)):.6f}")

    def update_recognition(self):

        x = self.get_canvas_vector()
        out = self.model.predict(x)
        vals = out.flatten()

        for i, w in enumerate(self.output_labels_widgets):
            w.config(text=f"{vals[i]:.3f}")

        top_idx = np.argmax(vals)
        top_val = vals[top_idx]
        self.pred_label.config(text=f"Prediction: {OUTPUT_LABELS[top_idx]} ({top_val:.3f})")


def main():
    root = tk.Tk()
    app = App(root)
    root.mainloop()


if __name__ == "__main__":
    main()
