import numpy as np
import pandas as pd
import time
from pathlib import Path

# ------------------------------------------------------------
# ВАЖНО: сюда вставьте ваши реальные функции PCA
# ------------------------------------------------------------
def run_pca_cpu(csv_path):
    """Запусти здесь свою CPU версию PCA."""
    start = time.time()
    # TODO: ваша CPU реализация PCA
    time.sleep(0.1)  # временный заглушка
    return time.time() - start

def run_pca_opencl(csv_path):
    """Запусти здесь свою OpenCL PCA реализацию."""
    start = time.time()
    # TODO: ваша OpenCL реализация PCA
    time.sleep(0.01)  # временная заглушка
    return time.time() - start

# ------------------------------------------------------------
# Тестовые параметры
# ------------------------------------------------------------
ROW_VARIANTS = [10_000, 50_000, 100_000, 200_000]
FEATURE_VARIANTS = [10, 50, 100, 200]

OUTPUT_DIR = Path("datasets")
OUTPUT_DIR.mkdir(exist_ok=True)

results = []


# ------------------------------------------------------------
# Вспомогательная функция
# ------------------------------------------------------------
def generate_dataset(n_rows, n_features):
    file_path = OUTPUT_DIR / f"ds_{n_rows}x{n_features}.csv"

    # Генерация
    np.random.seed(42)
    data = np.random.randn(n_rows, n_features)
    df = pd.DataFrame(data)

    df.to_csv(file_path, index=False)
    return file_path


# ------------------------------------------------------------
# 1. Тест: масштабируемость по строкам
# ------------------------------------------------------------
for rows in ROW_VARIANTS:
    file_path = generate_dataset(rows, 50)  # 50 признаков фиксировано

    print(f"\n=== RUN: rows={rows}, features=50 ===")

    cpu_time = run_pca_cpu(file_path)
    gpu_time = run_pca_opencl(file_path)
    speedup = cpu_time / gpu_time if gpu_time > 0 else float('inf')

    results.append({
        "rows": rows,
        "features": 50,
        "cpu_time": cpu_time,
        "gpu_time": gpu_time,
        "speedup": speedup
    })


# ------------------------------------------------------------
# 2. Тест: масштабируемость по признакам
# ------------------------------------------------------------
for feats in FEATURE_VARIANTS:
    file_path = generate_dataset(50_000, feats)  # 50к строк фиксировано

    print(f"\n=== RUN: rows=50k, features={feats} ===")

    cpu_time = run_pca_cpu(file_path)
    gpu_time = run_pca_opencl(file_path)
    speedup = cpu_time / gpu_time if gpu_time > 0 else float('inf')

    results.append({
        "rows": 50_000,
        "features": feats,
        "cpu_time": cpu_time,
        "gpu_time": gpu_time,
        "speedup": speedup
    })


# ------------------------------------------------------------
# 3. Таблица результатов
# ------------------------------------------------------------
df_results = pd.DataFrame(results)
df_results = df_results.sort_values(["rows", "features"])
print("\n==================== RESULTS ====================")
print(df_results.to_string(index=False))

df_results.to_csv("pca_benchmarks.csv", index=False)
print("\nSaved to pca_benchmarks.csv")