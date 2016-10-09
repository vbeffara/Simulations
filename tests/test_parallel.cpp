#include <vb/util.h>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#endif

using namespace vb; using namespace std;

int fib (int n) { return n<2 ? n : fib(n-1) + fib(n-2); }

double cum (int n) {
    vector<double> X(n);
    for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        X[i] = x;
    }
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum2 (int n) {
    double s=0;
    for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        s += x;
    }
    return s - long(s);
}

#ifdef CILK
int fib_cilk (int n) {
    if (n < 20) return fib(n);
    int x = cilk_spawn fib_cilk (n-1);
    int y = fib_cilk (n-2);
    cilk_sync;
    return x + y;
}

double cum_cilk (int n) {
    vector<double> X(n);
    cilk_for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        X[i] = x;
    }
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_cilk2 (int n) {
    cilk::reducer_opadd <double> s (0);
    cilk_for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        *s += x;
    }
    return s.get_value() - long(s.get_value());
}
#endif

#ifdef OPENMP
int fib_omp (int n) {
    if (n < 25) return fib(n);
    int x, y;
    #pragma omp parallel
    #pragma omp single nowait
    {
        #pragma omp task
        { x = fib_omp (n-1); }
        #pragma omp task
        { y = fib_omp (n-2); }
    }
    return x + y;
}

double cum_omp (int n) {
    vector<double> X(n);
    #pragma omp parallel for
    for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        X[i] = x;
    }
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_omp2 (int n) {
    double s=0;
    #pragma omp parallel for reduction(+:s)
    for (int i=0; i<n; ++i) {
        double x = i;
        for (int t=0; t<1000; ++t) x = cos(x);
        s += x;
    }
    return s - long(s);
}
#endif

int main (int argc, char ** argv) {
    H.init ("Test of various parallel frameworks",argc,argv,"n=42,l=100000");

    timing ("Fibonacci  | Single", [&]() { return fib(H['n']); });
#ifdef CILK
    timing ("Fibonacci  | CILK", [&]() { return fib_cilk(H['n']); });
#endif
#ifdef OPENMP
    timing ("Fibonacci  | OpenMP", [&]() { return fib_omp(H['n']); });
#endif

    timing ("Map+reduce | Single", [&]() { return cum(H['l']); });
    timing ("Map+reduce | Single 2", [&]() { return cum2(H['l']); });
#ifdef CILK
    timing ("Map+reduce | CILK", [&]() { return cum_cilk(H['l']); });
    timing ("Map+reduce | CILK 2", [&]() { return cum_cilk2(H['l']); });
#endif
#ifdef OPENMP
    timing ("Map+reduce | OpenMP  ", [&]() { return cum_omp(H['l']); });
    timing ("Map+reduce | OpenMP 2", [&]() { return cum_omp2(H['l']); });
#endif
}
