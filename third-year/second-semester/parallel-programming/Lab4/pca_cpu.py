
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
CPU baseline PCA (NumPy, float32). Loads CSV, computes top-M components with power iteration,
projects data, and prints results with timings.
"""
import argparse, time, numpy as np

def cpu_pca(X, M=2, iters=100):
    X = X.astype(np.float32, copy=False)
    mean = X.mean(axis=0, dtype=np.float64).astype(np.float32)
    Xc = X - mean
    C = (Xc.T @ Xc) / np.float32(X.shape[0] - 1)
    D = C.shape[0]

    comps = []
    Cw = C.copy()
    for _ in range(M):
        b = np.ones((D,), dtype=np.float32) / np.float32(np.sqrt(D))
        for _ in range(iters):
            y = Cw @ b
            nrm = np.sqrt(np.dot(y, y)).astype(np.float32)
            if nrm == 0:
                break
            b = y / nrm
        comps.append(b.copy())
        eigval = float(b @ (Cw @ b))
        Cw = Cw - eigval * np.outer(b, b).astype(np.float32)

    W = np.stack(comps, axis=0).astype(np.float32)
    Z = X @ W.T
    return W, Z

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--data", required=True)
    ap.add_argument("--components", type=int, default=2)
    ap.add_argument("--iterations", type=int, default=100)
    args = ap.parse_args()

    try:
        X = np.loadtxt(args.data, delimiter=",")
    except Exception:
        X = np.loadtxt(args.data)
    if X.ndim == 1:
        X = X.reshape(1, -1)

    t0 = time.perf_counter()
    W, Z = cpu_pca(X, args.components, args.iterations)
    t1 = time.perf_counter()

    np.set_printoptions(precision=6, suppress=True, linewidth=120)
    print(f"N={X.shape[0]}, D={X.shape[1]}, M={args.components}")
    print(f"CPU PCA time: {t1 - t0:.4f} s")
    print("\n=== PRINCIPAL COMPONENTS (CPU) ===")
    for i, comp in enumerate(W):
        print(f"Component {i}: {comp.tolist()}")

if __name__ == "__main__":
    main()
