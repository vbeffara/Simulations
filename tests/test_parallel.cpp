#include <future>
#include <numeric>
#include <pstl/algorithm>
#include <pstl/execution>
#include <pstl/memory>
#include <pstl/numeric>
#include <tbb/task_group.h>
#include <vb/util/misc.h>

using namespace std;
using namespace vb;

constexpr int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

int tbb_fib(int n) {
    if (n < 25) return fib(n);
    int             x, y;
    tbb::task_group g;
    g.run([&] { x = tbb_fib(n - 1); });
    g.run([&] { y = tbb_fib(n - 2); });
    g.wait();
    return x + y;
}

double cost(double x) {
    for (int i = 0; i < 10; ++i) x = cos(x);
    return x;
}

int main(int argc, char **argv) {
    Hub H("Test of various parallel frameworks", argc, argv, "n=41,l=4000000");
    int n = H['n'], l = H['l'];

    timing(H, "Fibonacci  | Single (recursive)", [=] { return fib(n); });

    timing(H, "Fibonacci  | Async (recursive)", [=] {
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

    timing(H, "Fibonacci  | TBB task group", [=] { return tbb_fib(n); });

    timing(H, "Map+reduce | Single (fill then sum)", [=] {
        vector<double> X(l);
        for (int i = 0; i < l; ++i) X[i] = cost(i);
        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | Single (direct sum)", [=] {
        double s = 0;
        for (int i = 0; i < l; ++i) s += cost(i);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | Single (STL algorithms)", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | Async (std::async, split fill + sum)", [=] {
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

    timing(H, "Map+reduce | Async (std::async, split direct sum)", [=] {
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

    timing(H, "Map+reduce | PSTL, execution::par", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(pstl::execution::par, X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | PSTL, execution::unseq", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(pstl::execution::unseq, X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | PSTL, execution::par_unseq", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(pstl::execution::par_unseq, X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | TBB parallel_reduce", [=] {
        struct sum_cost {
            sum_cost() = default;
            sum_cost(const sum_cost & /* unused */, tbb::split /* unused */) {}
            double my_sum = 0.0;
            void   operator()(const tbb::blocked_range<size_t> &r) {
                for (size_t i = r.begin(); i != r.end(); ++i) my_sum += cost(i);
            }
            void join(const sum_cost &y) { my_sum += y.my_sum; }
        } sc;
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, l), sc);
        return sc.my_sum - int64_t(sc.my_sum);
    });
}
