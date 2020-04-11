#include <functional>
#include <future>
#include <numeric>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_group.h>
#include <vb/util/misc.h>

#ifdef EXECUTION
#include <execution>
#else
#include <pstl/algorithm>
#include <pstl/execution>
#include <pstl/numeric>
#endif

using namespace std;
using namespace vb;

constexpr auto fib(size_t n) -> size_t { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

auto tbb_fib(size_t n) -> size_t {
    if (n < 25) return fib(n);
    size_t          x, y;
    tbb::task_group g;
    g.run([&] { x = tbb_fib(n - 1); });
    g.run([&] { y = tbb_fib(n - 2); });
    g.wait();
    return x + y;
}

auto cost(double x) -> double {
    for (int i = 0; i < 10; ++i) x = cos(x);
    return x;
}

auto main(int argc, char **argv) -> int {
    Hub    H("Test of various parallel frameworks", argc, argv, "n=41,l=4000000");
    size_t n = H['n'], l = H['l'];

    timing(H, "Fibonacci  | Single (recursive)", [=] { return fib(n); });

    timing(H, "Fibonacci  | Async (recursive)", [=] {
        class fib_async {
        public:
            auto operator()(size_t n) -> size_t {
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
        for (size_t i = 0; i < l; ++i) X[i] = cost(i);
        double s = 0;
        for (auto x : X) s += x;
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | Single (direct sum)", [=] {
        double s = 0;
        for (size_t i = 0; i < l; ++i) s += cost(i);
        return s - int64_t(s);
    });

    timing(H, "Map+reduce | Single (STL algorithms: transform + accumulate)", [=] {
        vector<double> X(l);
        std::iota(X.begin(), X.end(), 0);
        std::transform(X.begin(), X.end(), X.begin(), cost);
        double s = std::accumulate(X.begin(), X.end(), 0.0);
        return s - int64_t(s);
    });

    // TODO: re-enable this when it is implemented in libstdc++
    //
    // timing(H, "Map+reduce | Single (STL algorithms: transform_reduce)", [=] {
    //     vector<double> X(l);
    //     std::iota(X.begin(), X.end(), 0);
    //     double s = std::transform_reduce(begin(X), end(X), 0.0, std::plus<double>(), cost);
    //     return s - int64_t(s);
    // });

    timing(H, "Map+reduce | Async (std::async, split fill + sum)", [=] {
        class mr {
        public:
            vector<double> X;
            explicit mr(size_t l) : X(l) { run(0, l); }
            void run(size_t l1, size_t l2) {
                if (l2 - l1 <= 1000) {
                    for (auto i = l1; i < l2; ++i) X[i] = cost(i);
                    return;
                }
                auto l3  = (l1 + l2) / 2;
                auto one = async([=] { run(l1, l3); }), two = async([=] { run(l3, l2); });
                one.get();
                two.get();
            }
            auto sum() -> double {
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
            auto run(size_t l1, size_t l2) -> double {
                if (l2 - l1 <= 1000) {
                    double s = 0;
                    for (auto i = l1; i < l2; ++i) s += cost(i);
                    return s;
                }
                auto l3  = (l1 + l2) / 2;
                auto one = async([=] { return run(l1, l3); }), two = async([=] { return run(l3, l2); });
                return one.get() + two.get();
            }
            auto sum(size_t l) -> double {
                double s = run(0, l);
                return s - int64_t(s);
            }
        };
        return mr().sum(l);
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
