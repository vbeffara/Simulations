#include <vb/ThreadPool.h>
#include <thread>

namespace vb {
    void run_par(const std::function<void(Context)> &f) {
        boost::lockfree::stack<Work> S;
        bool                         done = false;
        std::vector<std::thread>     ts(std::max(std::thread::hardware_concurrency(), 1u));
        for (auto &t : ts)
            t = std::thread([&] {
                while (!done)
                    if (Work op; S.pop(op)) op();
            });
        f({S, std::make_shared<Will>([&done] { done = true; })});
        for (auto &t : ts) t.join();
    }

    void loop_go(Context C, int a, int b, std::function<void(int)> f, int l) {
        if (b - a <= l) {
            for (int i = a; i < b; ++i) f(i);
            return;
        }
        C.S.push([=] { loop_go(C, a, (a + b) / 2, f, l); });
        C.S.push([=] { loop_go(C, (a + b) / 2, b, f, l); });
    };

    void loop_par(int a, int b, std::function<void(int)> f, int l) {
        run_par([=](Context C) { loop_go(C, a, b, f, l); });
    }
} // namespace vb
