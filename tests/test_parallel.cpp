#include <vb/Stream.h>
#include <vb/util.h>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#endif
#include <numeric>

using namespace vb; using namespace std;

int fib (int n) { return n<2 ? n : fib(n-1) + fib(n-2); }

double cost (double x) { for (int i=0; i<10; ++i) x = cos(x); return x; }

double cum (int n) {
    vector<double> X(n); for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum2 (int n) { double s=0; for (int i=0; i<n; ++i) s += cost(i); return s - long(s); }

double cum3 (int n) {
    vector<double> X(n); for (int i=0; i<n; ++i) X[i]=i;
    std::transform (X.begin(), X.end(), X.begin(), cost);
    double s = std::accumulate (X.begin(), X.end(), 0.0);
    return s - long(s);
}

double cum4 (int n) {
    auto costs = Stream<double> ([n](Sink<double> & yield){
        for (int i=0; i<n; ++i) yield(cost(i));
    });
    double s=0; for (auto x : costs) s+=x; return s - long(s);
}

double cum5 (int n) {
    using coro_t = boost::coroutines2::coroutine<double>;
    coro_t::pull_type costs ([n](coro_t::push_type & yield){
        for (int i=0; i<n; ++i) yield(cost(i));
    });
    double s=0; for (auto x : costs) s+=x; return s - long(s);
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
    cilk_for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_cilk2 (int n) {
    cilk::reducer_opadd <double> s (0);
    cilk_for (int i=0; i<n; ++i) *s += cost(i);
    return s.get_value() - long(s.get_value());
}
#endif

#ifdef OPENMP
int fib_omp (int n) {
    if (n < 30) return fib(n);
    int x, y;
    #pragma omp parallel sections
    {
        x = fib_omp (n-1);
        #pragma omp section
        y = fib_omp (n-2);
    }
    return x + y;
}

double cum_omp (int n) {
    vector<double> X(n);
    #pragma omp parallel for
    for (int i=0; i<n; ++i) X[i] = cost(i);
    double s=0; for (auto x:X) s+=x; return s - long(s);
}

double cum_omp2 (int n) {
    double s=0;
    #pragma omp parallel for reduction(+:s)
    for (int i=0; i<n; ++i) s += cost(i);
    return s - long(s);
}
#endif

int main (int argc, char ** argv) {
    H.init ("Test of various parallel frameworks",argc,argv,"n=42,l=10000000");

    timing ("Fibonacci  | Single (recursive)", [&]() { return fib(H['n']); });
#ifdef CILK
    timing ("Fibonacci  | CILK", [&]() { return fib_cilk(H['n']); });
#endif
#ifdef OPENMP
    timing ("Fibonacci  | OpenMP (parallel sections)", [&]() { return fib_omp(H['n']); });
#endif

    timing ("Map+reduce | Single 1 (fill then sum)", [&]() { return cum(H['l']); });
    timing ("Map+reduce | Single 2 (direct sum)", [&]() { return cum2(H['l']); });
    timing ("Map+reduce | Single 3 (STL algorithms)", [&]() { return cum3(H['l']); });
    timing ("Map+reduce | Single 4 (Boost coroutine)", [&]() { return cum4(H['l']); });
    timing ("Map+reduce | Single 5 (Boost coroutine2)", [&]() { return cum4(H['l']); });
#ifdef CILK
    timing ("Map+reduce | CILK", [&]() { return cum_cilk(H['l']); });
    timing ("Map+reduce | CILK 2", [&]() { return cum_cilk2(H['l']); });
#endif
#ifdef OPENMP
    timing ("Map+reduce | OpenMP 1 (fill then sum)", [&]() { return cum_omp(H['l']); });
    timing ("Map+reduce | OpenMP 2 (direct reduction)", [&]() { return cum_omp2(H['l']); });
#endif
}
