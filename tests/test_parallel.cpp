#include <cmath>
#include <numeric>
#include <vb/Generator.h>
#include <vb/Stream.h>
#include <vb/util.h>

using namespace vb; using namespace std;

#if __has_include(<cilk/cilk.h>)
#define CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>

int fib (int n) { return n<2 ? n : fib(n-1) + fib(n-2); }

int fib_cilk (int n) {
    if (n < 20) return fib(n);
    int x = cilk_spawn fib_cilk (n-1);
    int y = fib_cilk (n-2);
    cilk_sync;
    return x + y;
}
#endif

double cost (double x) { for (int i=0; i<10; ++i) x = cos(x); return x; }

int main (int argc, char ** argv) {
    H.init ("Test of various parallel frameworks",argc,argv,"n=41,l=4000000");
    int n=H['n'], l=H['l'];

    timing ("Fibonacci  | Single (recursive)", [=]{
        class fib { public: int operator() (int n) { return n<2 ? n : (*this)(n-1) + (*this)(n-2); } };
        return fib()(n);
    });

#ifdef _OPENMP
    timing ("Fibonacci  | OpenMP (parallel sections)", [=]{
        class fib     { public: int operator() (int n) { return n<2 ? n : (*this)(n-1) + (*this)(n-2); } };
        class fib_omp { public: int operator() (int n) {
            if (n < 30) return fib()(n);
            int x,y;
            #pragma omp parallel sections
            {
                x = fib_omp() (n-1);
                #pragma omp section
                y = fib_omp() (n-2);
            }
            return x + y;
        } };
        return fib_omp()(n);
    });
#endif

#ifdef CILK
    timing ("Fibonacci  | CILK", []{ return fib_cilk(H['n']); });
#endif

    timing ("Map+reduce | Single (fill then sum)", [=]{
        vector<double> X(l); for (int i=0; i<l; ++i) X[i] = cost(i);
        double s=0; for (auto x:X) s+=x; return s - int64_t(s);
    });

    timing ("Map+reduce | Single (direct sum)", [=]{
        double s=0; for (int i=0; i<l; ++i) s += cost(i); return s - int64_t(s);
    });

    timing ("Map+reduce | Single (STL algorithms)", [=]{
        vector<double> X(l);
        std::iota (X.begin(), X.end(), 0);
        std::transform (X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate (X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing ("Map+reduce | Coroutine (Boost)", [=]{
        auto costs = take (l, fmap (cost, ints()));
        double s=0; for (auto x : costs) s+=x;
        return s - int64_t(s);
    });

#ifdef __cpp_coroutines
    timing ("Map+reduce | Coroutine (native)", [=]{
        double s=0; for (auto x : take(l,fmap(cost,new_ints()))) s += x;
        return s - int64_t(s);
    });
#endif

#ifdef _OPENMP
    timing ("Map+reduce | OpenMP (fill then sum)", [=]{
        vector<double> X(l);
        #pragma omp parallel for
        for (int i=0; i<l; ++i) X[i] = cost(i); // NOLINT
        double s=0; for (auto x:X) s+=x; return s - int64_t(s);
    });

    timing ("Map+reduce | OpenMP (fill then sum + SIMD)", [=]{
        vector<double> X(l);
        #pragma omp parallel for simd
        for (int i=0; i<l; ++i) X[i] = cost(i); // NOLINT
        double s=0; for (auto x:X) s+=x; return s - int64_t(s);
    });

    timing ("Map+reduce | OpenMP (direct reduction)", [=]{
        double s=0;
        #pragma omp parallel for reduction(+:s)
        for (int i=0; i<l; ++i) s += cost(i); // NOLINT
        return s - int64_t(s);
    });

    timing ("Map+reduce | OpenMP (direct reduction + SIMD)", [=]{
        double s=0;
        #pragma omp parallel for simd reduction(+:s)
        for (int i=0; i<l; ++i) s += cost(i); // NOLINT
        return s - int64_t(s);
    });
#endif

#ifdef CILK
    timing ("Map+reduce | CILK (fill then sum)", [=]{
        vector<double> X(l);
        cilk_for (int i=0; i<l; ++i) X[i] = cost(i);
        double s=0; for (auto x:X) s+=x; return s - int64_t(s);
    });

    timing ("Map+reduce | CILK (direct reduction)", [=]{
        cilk::reducer_opadd <double> s (0);
        cilk_for (int i=0; i<l; ++i) *s += cost(i);
        return s.get_value() - int64_t(s.get_value());
    });
#endif
}
