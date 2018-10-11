#include <vb/util.h>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <vector>

using Work = std::function<void()>;
using BLST = boost::lockfree::stack<Work>;

struct Will : public Work {
    using Work::Work;
    ~Will() { (*this)(); }
};

void run_par(const std::function<void(BLST &, std::shared_ptr<Will>)> &f) {
    BLST                     S;
    bool                     done = false;
    std::vector<std::thread> ts(std::max(std::thread::hardware_concurrency(), 1u));
    for (auto &t : ts)
        t = std::thread([&] {
            while (!done)
                if (Work op; S.pop(op)) op();
        });
    f(S, std::make_shared<Will>([&done] { done = true; }));
    for (auto &t : ts) t.join();
}

constexpr int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

void fib(BLST &S, std::shared_ptr<Will> parent, int n, const std::shared_ptr<int> &t) {
    if (n < 25) {
        *t = fib(n);
        return;
    }
    auto t1 = std::make_shared<int>(0), t2 = std::make_shared<int>(0);
    auto post = std::make_shared<Will>([p = std::move(parent), t, t1, t2] { *t = *t1 + *t2; });
    S.push([=, &S] { fib(S, post, n - 1, t1); });
    S.push([=, &S] { fib(S, post, n - 2, t2); });
}

int main(int argc, char **argv) {
    vb::H.init("Trying a new task system", argc, argv, "n=41");
    int n = vb::H['n'];

    vb::timing("Thread pool", [=] {
        auto t = std::make_shared<int>(0);
        run_par([n, t](auto &S, auto p) { fib(S, p, n, t); });
        return *t;
    });
}
