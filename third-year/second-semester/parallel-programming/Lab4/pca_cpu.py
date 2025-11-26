#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, time, math, csv

def load_csv(path):
    data = []
    with open(path, "r") as f:
        reader = csv.reader(f)
        for row in reader:
            if row:
                data.append([float(x) for x in row])
    return data  # list of list

def mean_vector(X):
    N = len(X)
    D = len(X[0])
    mean = [0.0]*D
    for i in range(D):
        s = 0.0
        for n in range(N):
            s += X[n][i]
        mean[i] = s / N
    return mean

def center_data(X, mean):
    N = len(X)
    D = len(X[0])
    Xc = []
    for n in range(N):
        Xc.append([X[n][i] - mean[i] for i in range(D)])
    return Xc

def covariance_matrix(Xc):
    N = len(Xc)
    D = len(Xc[0])
    C = [[0.0]*D for _ in range(D)]
    for i in range(D):
        for j in range(D):
            s = 0.0
            for n in range(N):
                s += Xc[n][i]*Xc[n][j]
            C[i][j] = s / (N-1)
    return C

def matvec(M, v):
    D = len(v)
    y = [0.0]*D
    for i in range(D):
        acc = 0.0
        for j in range(D):
            acc += M[i][j]*v[j]
        y[i] = acc
    return y

def norm(v):
    return math.sqrt(sum(x*x for x in v))

def scalar_mult(v, s):
    return [x*s for x in v]

def vector_sub(a, b):
    return [a[i]-b[i] for i in range(len(a))]

def outer(b):
    D = len(b)
    return [[b[i]*b[j] for j in range(D)] for i in range(D)]

def subtract_matrix(C, outer_bb, lam):
    D = len(C)
    for i in range(D):
        for j in range(D):
            C[i][j] -= lam * outer_bb[i][j]

def power_iteration(C, iters=100):
    D = len(C)
    b = [1.0/math.sqrt(D)]*D
    for _ in range(iters):
        y = matvec(C, b)
        nrm = norm(y)
        if nrm == 0:
            break
        b = scalar_mult(y, 1/nrm)
    return b

def project(X, components):
    N = len(X)
    M = len(components)
    Z = []
    for n in range(N):
        row = []
        for m in range(M):
            acc = sum(X[n][i]*components[m][i] for i in range(len(X[0])))
            row.append(acc)
        Z.append(row)
    return Z

def cpu_pca(X, M=2, iters=100):
    mean = mean_vector(X)
    Xc = center_data(X, mean)
    C = covariance_matrix(Xc)
    components = []
    for _ in range(M):
        b = power_iteration(C, iters)
        components.append(b[:])
        lam = sum(b[i]*sum(C[i][j]*b[j] for j in range(len(b))) for i in range(len(b)))
        subtract_matrix(C, outer(b), lam)
    Z = project(X, components)
    return components, Z

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--data", required=True)
    ap.add_argument("--components", type=int, default=2)
    ap.add_argument("--iterations", type=int, default=100)
    args = ap.parse_args()

    X = load_csv(args.data)
    t0 = time.perf_counter()
    W, Z = cpu_pca(X, args.components, args.iterations)
    t1 = time.perf_counter()

    print(f"N={len(X)}, D={len(X[0])}, M={args.components}")
    print(f"CPU PCA time (pure Python): {t1-t0:.4f} s\n")
    for i, comp in enumerate(W):
        print(f"Component {i}: {comp}")

if __name__ == "__main__":
    main()
