#include <vb/util.h>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <vector>

using namespace std;
using task = std::function<void()>;

struct will : public task {
    using task::task;
    ~will() { (*this)(); }
};

boost::lockfree::stack<task> queue;

int fibo(int n) { return n < 2 ? n : fibo(n - 1) + fibo(n - 2); }

void fib1(shared_ptr<will> parent, int n, const shared_ptr<int> & t) {
    if (n < 25) {
        *t = fibo(n);
        return;
    }
    auto t1 = make_shared<int>(0), t2 = make_shared<int>(0);
    auto post = make_shared<will>([parent, t, t1, t2] { *t = *t1 + *t2; });
    queue.push([=] { fib1(post, n - 1, t1); });
    queue.push([=] { fib1(post, n - 2, t2); });
}

void runner(bool & done) {
    while (!done)
        if (task op; queue.pop(op)) op();
}

int main(int argc, char ** argv) {
    vb::H.init("Trying a new task system", argc, argv, "n=41");
    int n = vb::H['n'];

    vb::timing("Just the tasks", [=] {
        bool done = false;
        auto t    = make_shared<int>(0);
        {
            auto sentinel = make_shared<will>([&done] { done = true; });
            queue.push([=] { fib1(sentinel, n, t); });
        }
        runner(done);
        return *t;
    });

    vb::timing("Thread pool", [=] {
        bool done = false;
        auto t    = make_shared<int>(0);
        {
            auto sentinel = make_shared<will>([&done] { done = true; });
            queue.push([=] { fib1(sentinel, n, t); });
        }
        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i) runners.emplace_back([&done] { runner(done); });
        for (auto & th : runners) th.join();
        return *t;
    });
}
