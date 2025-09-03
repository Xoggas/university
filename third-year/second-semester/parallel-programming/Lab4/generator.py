import numpy as np
import pandas as pd

np.random.seed(42)
n_samples = 100_000
n_features = 50

data_huge = np.random.randn(n_samples, n_features)
df_huge = pd.DataFrame(data_huge)

file_huge = "sample_huge.csv"
df_huge.to_csv(file_huge, index=False)