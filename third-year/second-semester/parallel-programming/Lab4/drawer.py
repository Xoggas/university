import pandas as pd
import matplotlib.pyplot as plt

# Данные
data = [
    [10000,50,0.10058951377868652,0.010161399841308594,9.899178789300798],
    [50000,10,0.1002049446105957,0.01050710678100586,9.536873156342184],
    [50000,50,0.10043931007385254,0.010407447814941406,9.650714743883443],
    [50000,50,0.10092043876647949,0.010500907897949219,9.610639360639361],
    [50000,100,0.1005403995513916,0.010191679000854492,9.864949587105528],
    [50000,200,0.10015439987182617,0.010523319244384766,9.517377316597942],
    [100000,50,0.10049033164978027,0.010599136352539062,9.480992441965089],
    [200000,50,0.10050082206726074,0.010125160217285156,9.92585005180371]
]

df = pd.DataFrame(data, columns=["rows", "features", "cpu_time", "gpu_time", "speedup"])

# ------------------------------
# Speedup vs Rows (фиксировано features=50)
# ------------------------------
df_rows = df[df["features"]==50].sort_values("rows")
plt.figure(figsize=(7,5))
plt.plot(df_rows["rows"], df_rows["speedup"], marker="o")
plt.xlabel("Rows (N)")
plt.ylabel("Speedup (CPU/GPU)")
plt.title("Speedup vs Rows (features=50)")
plt.grid(True)
plt.show()

# ------------------------------
# Speedup vs Features (фиксировано rows=50000)
# ------------------------------
df_feats = df[df["rows"]==50000].sort_values("features")
plt.figure(figsize=(7,5))
plt.plot(df_feats["features"], df_feats["speedup"], marker="o", color="orange")
plt.xlabel("Features (D)")
plt.ylabel("Speedup (CPU/GPU)")
plt.title("Speedup vs Features (rows=50k)")
plt.grid(True)
plt.show()
