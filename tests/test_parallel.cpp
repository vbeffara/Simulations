#include <vb/Ranges.h>
#include <vb/ThreadPool.h>
#include <vb/util.h>
#include <future>
#include <numeric>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/take.hpp>

using namespace ranges;
using namespace std;
using namespace vb;

constexpr int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

void fib(Context C, int n, const std::shared_ptr<int> &t) {
    if (n < 25) {
        *t = fib(n);
        return;
    }
    auto t1 = std::make_shared<int>(0), t2 = std::make_shared<int>(0);
    C.then([=](Context C) { *t = *t1 + *t2; });
    C.push([=](Context C) { fib(C, n - 1, t1); });
    C.push([=](Context C) { fib(C, n - 2, t2); });
}

double cost(double x) {
    for (int i = 0; i < 10; ++i) x = cos(x);
    return x;
}

int main(int argc, char **argv) {
    H.init("Test of various parallel frameworks", argc, argv, "n=41,l=4000000");
    int n = H['n'], l = H['l'];

    timing("Fibonacci  | Single (recursive)", [=] { return fib(n); });

    timing("Fibonacci  | Async (recursive)", [=] {
        class fib_async {
        public:
            int operator()(int n) {
                if (n < 25) return fib(n);
                auto res1 = async([=]() { return (*this)(n - 1); });
                auto res2 = async([=]() { return (*this)(n - 2); });
                return res1.get() + res2.get();
            }
        };
        return fib_async()(n);
    });

#ifdef _OPENMP
    timing("Fibonacci  | OpenMP (parallel sections)", [=] {
        class fib {
        public:
            int operator()(int n) { return n < 2 ? n : (*this)(n - 1) + (*this)(n - 2); }
        };
        class fib_omp {
        public:
            int operator()(int n) {
                if (n < 25) return fib()(n);
                int x, y;
#pragma omp parallel sections
                {
                    x = fib_omp()(n - 1);
#pragma omp section
                    y = fib_omp()(n - 2);
                }
                return x + y;
            }
        };
        return fib_omp()(n);
    });
#endif

    timing("Fibonacci  | New style thread pool", [=] {
        auto t = std::make_shared<int>(0);
        run_par([n, t](Context C) { fib(C, n, t); });
        return *t;
    });

    timing("Map+reduce | Single (fill then sum)", [=] {
        vector<double> X(l);
        for (int i = 0; i < l; ++i) X[i] = cost(i);
        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | Single (direct sum)", [=] {
        double s = 0;
        for (int i = 0; i < l; ++i) s += cost(i);
        return s - int64_t(s);
    });

    timing("Map+reduce | Single (STL algorithms)", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing("Map+reduce | Single (Ranges)", [=] {
        auto s = ranges::accumulate(view::ints(0, l) | view::transform(cost), 0.0);
        return s - int64_t(s);
    });

    // timing("Map+reduce | Coroutine (Boost)", [=] {
    //     auto   costs = take(l, fmap(cost, ints()));
    //     double s     = 0;
    //     for (auto x : costs) s += x;
    //     return s - int64_t(s);
    // });

#ifdef __cpp_coroutines
    timing("Map+reduce | Coroutine (native with ranges)", [=] {
        auto costs = []() -> re::generator<double> {
            int i = 0;
            for (;;) { co_yield cost(i++); }
        };

        auto s = ranges::accumulate(costs() | view::take(l), 0.0);
        return s - int64_t(s);
    });
#endif

    timing("Map+reduce | new ThreadPool (loop_par, loop=1)", [=] {
        vector<double> X((int(H['l'])));
        loop_par(0, X.size(), [&X](int i) { X[i] = cost(i); }, 1);

        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | new ThreadPool (loop_par, loop=10)", [=] {
        vector<double> X((int(H['l'])));
        loop_par(0, X.size(), [&X](int i) { X[i] = cost(i); }, 10);

        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | new ThreadPool (loop_par, loop=100)", [=] {
        vector<double> X((int(H['l'])));
        loop_par(0, X.size(), [&X](int i) { X[i] = cost(i); }, 100);

        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | new ThreadPool (loop_par, loop=1000)", [=] {
        vector<double> X((int(H['l'])));
        loop_par(0, X.size(), [&X](int i) { X[i] = cost(i); }, 1000);

        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | new ThreadPool (loop_par, loop=10000)", [=] {
        vector<double> X((int(H['l'])));
        loop_par(0, X.size(), [&X](int i) { X[i] = cost(i); }, 10000);

        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | Async (std::async, split fill + sum)", [=] {
        class mr {
        public:
            vector<double> X;
            explicit mr(int l) : X(l) { run(0, l); }
            void run(int l1, int l2) {
                if (l2 - l1 <= 1000) {
                    for (int i = l1; i < l2; ++i) X[i] = cost(i);
                    return;
                }
                int  l3  = (l1 + l2) / 2;
                auto one = async([=] { run(l1, l3); }), two = async([=] { run(l3, l2); });
                one.get();
                two.get();
            }
            double sum() {
                double s = 0;
                for (auto x : X) s += x;
                return s - int64_t(s);
            }
        };
        return mr(l).sum();
    });

    timing("Map+reduce | Async (std::async, split direct sum)", [=] {
        class mr {
        public:
            double run(int l1, int l2) {
                if (l2 - l1 <= 1000) {
                    double s = 0;
                    for (int i = l1; i < l2; ++i) s += cost(i);
                    return s;
                }
                int  l3  = (l1 + l2) / 2;
                auto one = async([=] { return run(l1, l3); }), two = async([=] { return run(l3, l2); });
                return one.get() + two.get();
            }
            double sum(int l) {
                double s = run(0, l);
                return s - int64_t(s);
            }
        };
        return mr().sum(l);
    });

#ifdef _OPENMP
    timing("Map+reduce | OpenMP (fill then sum)", [=] {
        vector<double> X(l);
#pragma omp parallel for
        for (int i = 0; i < l; ++i) X[i] = cost(i); // NOLINT
        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | OpenMP (fill then sum + SIMD)", [=] {
        vector<double> X(l);
#pragma omp parallel for simd
        for (int i = 0; i < l; ++i) X[i] = cost(i); // NOLINT
        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing("Map+reduce | OpenMP (direct reduction)", [=] {
        double s = 0;
#pragma omp parallel for reduction(+ : s)
        for (int i = 0; i < l; ++i) s += cost(i); // NOLINT
        return s - int64_t(s);
    });

    timing("Map+reduce | OpenMP (direct reduction + SIMD)", [=] {
        double s = 0;
#pragma omp parallel for simd reduction(+ : s)
        for (int i = 0; i < l; ++i) s += cost(i); // NOLINT
        return s - int64_t(s);
    });
#endif
}
