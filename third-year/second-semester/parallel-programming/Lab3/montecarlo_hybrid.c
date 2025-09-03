// montecarlo_hybrid.c
// Hybrid MPI + OpenMP Monte-Carlo maximization
// Usage:
//   mpirun -np <nprocs> ./montecarlo_hybrid <total_samples> [seed]
#define _GNU_SOURCE
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    double val;
    double x;
    double y;
} triple;

double f(double x, double y) {
    double x2 = x*x, y2 = y*y;
    double term = x*x*x*x + y*y*y*y - 16.0*x2 - 16.0*y2 + 5.0*x*y;
    double expo = exp(-(x2 + y2) / 10.0);
    double t = tanh((x + y) / 5.0);
    return -term * expo * t;
}

/* custom reduction: choose triple with larger val */
void max_triple_fn(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype) {
    triple *in = (triple*)invec;
    triple *inout = (triple*)inoutvec;
    for (int i = 0; i < *len; ++i) {
        if (in[i].val > inout[i].val) inout[i] = in[i];
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc < 2) {
        if (rank == 0) fprintf(stderr, "Usage: %s <total_samples> [seed]\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long long total_samples = atoll(argv[1]);
    long long base_samples_per_rank = total_samples / nprocs;
    long long remainder = total_samples % nprocs;
    long long samples_for_me = base_samples_per_rank + (rank < remainder ? 1 : 0);

    unsigned int seed = (argc >= 3) ? (unsigned int)atoi(argv[2]) : (unsigned int)time(NULL);

    /* Create MPI datatype for triple and reduction op */
    MPI_Datatype triple_type;
    MPI_Type_contiguous(3, MPI_DOUBLE, &triple_type);
    MPI_Type_commit(&triple_type);

    MPI_Op max_triple_op;
    MPI_Op_create(&max_triple_fn, 1, &max_triple_op);

    double t_start = MPI_Wtime();

    triple local_best;
    local_best.val = -INFINITY;
    local_best.x = 0.0;
    local_best.y = 0.0;

    int omp_threads = omp_get_max_threads();

    /* per-thread local bests */
    double *thread_best_val = (double*)malloc(sizeof(double) * omp_threads);
    double *thread_best_x = (double*)malloc(sizeof(double) * omp_threads);
    double *thread_best_y = (double*)malloc(sizeof(double) * omp_threads);
    for (int i = 0; i < omp_threads; ++i) {
        thread_best_val[i] = -INFINITY;
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nth = omp_get_num_threads();

        /* thread-local PRNG state */
        struct drand48_data randbuf;
        long int thread_seed = (long int)seed ^ (long int)(rank * 10007) ^ (long int)(tid * 101);
        srand48_r(thread_seed, &randbuf);

        /* distribute samples between threads fairly */
        long long my_samples = samples_for_me / nth;
        long long extra = samples_for_me % nth;
        if (tid < extra) my_samples++;

        double local_val = -INFINITY;
        double local_x = 0.0, local_y = 0.0;
        double r1, r2;
        for (long long i = 0; i < my_samples; ++i) {
            drand48_r(&randbuf, &r1); // [0,1)
            drand48_r(&randbuf, &r2);
            double x = -10.0 + 20.0 * r1;
            double y = -10.0 + 20.0 * r2;
            double val = f(x, y);
            if (val > local_val) {
                local_val = val;
                local_x = x;
                local_y = y;
            }
        }
        thread_best_val[tid] = local_val;
        thread_best_x[tid] = local_x;
        thread_best_y[tid] = local_y;
    } // end omp parallel

    /* reduce thread-local bests to rank-local best */
    for (int i = 0; i < omp_threads; ++i) {
        if (thread_best_val[i] > local_best.val) {
            local_best.val = thread_best_val[i];
            local_best.x = thread_best_x[i];
            local_best.y = thread_best_y[i];
        }
    }

    /* global reduction across ranks using custom op */
    triple global_best;
    global_best.val = -INFINITY; global_best.x = 0.0; global_best.y = 0.0;
    MPI_Reduce(&local_best, &global_best, 1, triple_type, max_triple_op, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();
    double elapsed = t_end - t_start;

    /* root prints result */
    if (rank == 0) {
        printf("Total samples: %lld\n", total_samples);
        printf("Global best value: %.12e at x=%.9f y=%.9f\n", global_best.val, global_best.x, global_best.y);
        printf("Elapsed (wall) time: %.6f s\n", elapsed);
    }

    free(thread_best_val);
    free(thread_best_x);
    free(thread_best_y);
    MPI_Op_free(&max_triple_op);
    MPI_Type_free(&triple_type);
    MPI_Finalize();
    return 0;
}
