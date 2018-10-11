#include <vb/util.h>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <vector>

struct Will : public std::function<void()> {
    using std::function<void()>::function;
    ~Will() { (*this)(); }
};

struct STP : public boost::lockfree::stack<std::function<void()>> {
    std::vector<std::thread> runners;
    bool                     done = false;
    template <typename F> STP(F &&f) {
        f(*this, std::make_shared<Will>([=] { done = true; }));
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([=] {
                while (!done)
                    if (value_type op; pop(op)) op();
            });
        for (auto &th : runners) th.join();
    }
};

constexpr int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }

void fib(STP &S, std::shared_ptr<Will> parent, int n, const std::shared_ptr<int> &t) {
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

    vb::timing("Thread pool", [n] {
        auto t = std::make_shared<int>(0);
        STP([n, t](STP &S, std::shared_ptr<Will> post) { fib(S, post, n, t); });
        return *t;
    });
}
