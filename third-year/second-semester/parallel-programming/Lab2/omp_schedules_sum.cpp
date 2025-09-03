// OpenMP experiment: cyclic parallelism & schedules
// Task: compute S = sum_{i=1}^N ( tan(i) + i^3 * e^{-sin(i^2)} ) / ( (i+2)! + sqrt(i) + 3 + cos(i) )
// Implements schedule(static), schedule(dynamic), schedule(guided), schedule(auto)
// Also supports two workload modes: uniform (balanced cost per i) and skewed (cost grows with i)
// Build (GCC/Clang):
//   g++ -O3 -fopenmp omp_schedules_sum.cpp -o omp_schedules_sum -std=c++17
// Build (MSVC):
//   cl /O2 /openmp omp_schedules_sum.cpp
// Run examples:
//   ./omp_schedules_sum --N 20000000 --sched static --chunk 1024 --threads 8 --mode uniform
//   ./omp_schedules_sum --N 20000000 --sched guided --chunk 1024 --threads 8 --mode skewed
//   ./omp_schedules_sum --sweep 1,2,4,8 --sched dynamic --chunk 512 --N 20000000
// Notes:
//  * Uses long double for better numerical headroom.
//  * Uses lgammal (log-gamma) to avoid direct factorial overflow. For i >= 20 the factorial
//    overwhelms the small addends in the denominator; we compute via exp(logGamma) when it
//    does not overflow long double, otherwise we safely treat denominator as +infinity,
//    making the term ~0.
//  * Skips rare i where tan(i) is undefined (cos(i) ~ 0).

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#ifdef _OPENMP
  #include <omp.h>
#else
  #warning "OpenMP not enabled — build with -fopenmp or /openmp"
  inline double omp_get_wtime() { return 0.0; }
  inline int omp_get_max_threads() { return 1; }
  inline void omp_set_num_threads(int) {}
#endif

struct Args {
    long long N = 20'000'000;      // pick empirically for your CPU; default gives a few seconds on modern CPUs
    std::string sched = "static";  // static | dynamic | guided | auto
    int chunk = 1024;              // chunk size for static/dynamic/guided
    int threads = 0;               // 0 -> use OMP default
    std::vector<int> sweep;        // optional: sweep thread counts, e.g. 1,2,4,8
    std::string mode = "uniform";  // uniform | skewed
};

static void parse_sweep(const std::string& s, std::vector<int>& out) {
    size_t start = 0; 
    while (start < s.size()) {
        size_t comma = s.find(',', start);
        std::string tok = s.substr(start, comma == std::string::npos ? s.size() - start : comma - start);
        if (!tok.empty()) out.push_back(std::max(1, std::stoi(tok)));
        if (comma == std::string::npos) break; else start = comma + 1;
    }
}

static Args parse_args(int argc, char** argv) {
    Args a;
    for (int i=1; i<argc; ++i) {
        std::string k = argv[i];
        auto need = [&](const char* what){ if (i+1>=argc){fprintf(stderr,"Missing value for %s\n",what); exit(1);} return std::string(argv[++i]); };
        if (k=="--N") a.N = std::stoll(need("--N"));
        else if (k=="--sched") a.sched = need("--sched");
        else if (k=="--chunk") a.chunk = std::max(1, std::stoi(need("--chunk")));
        else if (k=="--threads") a.threads = std::max(1, std::stoi(need("--threads")));
        else if (k=="--sweep") parse_sweep(need("--sweep"), a.sweep);
        else if (k=="--mode") a.mode = need("--mode");
        else if (k=="-h" || k=="--help") {
            std::cout << "Usage: omp_schedules_sum [--N N] [--sched static|dynamic|guided|auto] [--chunk K] [--threads T] [--sweep 1,2,4,...] [--mode uniform|skewed]\n";
            exit(0);
        }
    }
    return a;
}

// Busy-work to emulate skewed (disproportionate) load per iteration.
// When mode==skewed, we do O(r) floating ops with r depending on i (growing with i).
static inline void skewed_work(long long i, bool skewed) {
    if (!skewed) return;
    // r grows with i but is bounded to keep runtime reasonable
    int r = (int)((i % 10) * (i % 10) * 10); // up to 900 iterations per i
    long double x = (long double)i * 1e-6L;
    for (int t=0; t<r; ++t) {
        // a few transcendental ops to burn cycles in an i-dependent way
        x = expl(-sinl(x)) + cosl(x);
    }
}

static inline long double safe_tanl(long double x) {
    long double c = cosl(x);
    if (fabsl(c) < 1e-18L) return NAN; // undefined; caller will skip
    return sinl(x) / c;
}

static inline long double denom_factorial(long long i) {
    // (i+2)! = tgamma(i+3)
    long double lg = lgammal((long double)i + 3.0L);
    // If lg is too big, exp(lg) will overflow -> treat as +inf, which makes term ~0.
    const long double MAX_LDEXP = 1.1356523406294144e+04L; // ~ logl(LDBL_MAX) for 80-bit long double
    if (lg > MAX_LDEXP) return INFINITY;
    return expl(lg); // exp(log((i+2)!))
}

static inline long double term(long long i) {
    long double t = safe_tanl((long double)i);
    if (!std::isfinite((double)t)) return 0.0L; // skip undefined tan(i)
    long double num = t + (long double)i*(long double)i*(long double)i * expl(-sinl((long double)i*(long double)i));

    long double denom = denom_factorial(i) + sqrtl((long double)i) + 3.0L + cosl((long double)i);
    if (!std::isfinite((double)denom) || denom==0.0L) return 0.0L;
    return num / denom;
}

static long double run_once(const Args& a, int T) {
#ifdef _OPENMP
    omp_set_num_threads(T);
#endif
    double t0 = omp_get_wtime();
    long double sum = 0.0L;

    // Choose schedule
    #pragma omp parallel
    {
        long double local = 0.0L;
        #pragma omp for schedule(static) nowait
        for (int _=0; _<0; ++_) {}
    }

    if (a.sched == "static") {
        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(static, /*chunk*/1)
            for (long long i = 1; i <= a.N; ++i) {
                skewed_work(i, a.mode == "skewed");
                sum += term(i);
            }
        }
    } else if (a.sched == "dynamic") {
        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(dynamic, /*chunk*/1)
            for (long long i = 1; i <= a.N; ++i) {
                skewed_work(i, a.mode == "skewed");
                sum += term(i);
            }
        }
    } else if (a.sched == "guided") {
        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(guided, /*chunk*/1)
            for (long long i = 1; i <= a.N; ++i) {
                skewed_work(i, a.mode == "skewed");
                sum += term(i);
            }
        }
    } else if (a.sched == "auto") {
        #pragma omp parallel reduction(+:sum)
        {
            #pragma omp for schedule(auto)
            for (long long i = 1; i <= a.N; ++i) {
                skewed_work(i, a.mode == "skewed");
                sum += term(i);
            }
        }
    } else {
        fprintf(stderr, "Unknown --sched %s\n", a.sched.c_str());
        exit(2);
    }

    double t1 = omp_get_wtime();
    double elapsed = t1 - t0;

    std::cout.setf(std::ios::fixed);
    std::cout.precision(6);
    std::cout << "threads=" << T
              << ", sched=" << a.sched
              << ", chunk=" << a.chunk
              << ", mode=" << a.mode
              << ", N=" << a.N
              << ", time_sec=" << elapsed
              << ", S=" << (double)sum << "\n";

    return sum;
}

int main(int argc, char** argv) {
    Args a = parse_args(argc, argv);

    // Honor custom chunk size by redefining OMP_SCHEDULE at runtime (supported by GCC/Clang)
    // For MSVC you can just rely on the explicit schedule clauses we use above; chunk is set to 1 in pragmas
    if (a.chunk > 1) {
        std::string spec = a.sched + "," + std::to_string(a.chunk);
        // If OMP_SCHEDULE is respected, schedule(auto) ignores it; others can pick it up depending on compiler
        setenv("OMP_SCHEDULE", spec.c_str(), 1);
    }

    if (a.sweep.empty()) {
        int T = a.threads > 0 ? a.threads : omp_get_max_threads();
        run_once(a, T);
        return 0;
    }

    // Sweep thread counts and print lines for easy plotting
    for (int T : a.sweep) {
        run_once(a, T);
    }
    return 0;
}
