#include <future>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#endif
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <vb/Hub.h>

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
    if (n < 20) return fib(n);
    int x, y;
    #pragma omp parallel
    {
        #pragma omp task shared(x)
        x = fib_omp (n-1);
        y = fib_omp (n-2);
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
#endif

template <typename T> void test (const string & s, T f (int), int n) {
    std::chrono::steady_clock C;
    auto i = C.now();
    auto result = f(n);
    chrono::duration<double> dur = C.now() - i;
    cout << s << " | " << n << " " << result << " " << dur.count() << endl;
}

int main (int argc, char ** argv) {
    H.init ("Test of various parallel frameworks",argc,argv,"n=45,l=500000,m=127");
    int n = H['n'], m = H['m'];

    if (m & 1)  test ("Fib | Single", fib, n);
#ifdef CILK
    if (m & 2)  test ("Fib | CILK  ", fib_cilk, n);
#endif
#ifdef OPENMP
    if (m & 4)  test ("Fib | OpenMP", fib_omp, n);
#endif

    if (m & 8)  test ("Map | Single", cum, H['l']);
#ifdef CILK
    if (m & 16) test ("Map | CILK  ", cum_cilk, H['l']);
    if (m & 32) test ("Map | CILK 2", cum_cilk2, H['l']);
#endif
#ifdef OPENMP
    if (m & 64) test ("Map | OpenMP", cum_omp, H['l']);
#endif
}
