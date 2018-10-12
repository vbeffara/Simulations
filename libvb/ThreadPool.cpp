#include <vb/ThreadPool.h>
#include <future>
#include <thread>

namespace vb {
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

    void loop_go(BLST &S, std::shared_ptr<Will> parent, int a, int b, std::function<void(int)> f, int l) {
        if (b - a <= l) {
            for (int i = a; i < b; ++i) f(i);
            return;
        }
        int c = (a + b) / 2;
        S.push([=, &S] { loop_go(S, parent, a, c, f, l); });
        S.push([=, &S] { loop_go(S, parent, c, b, f, l); });
    };

    void loop_par(int a, int b, std::function<void(int)> f, int l) {
        run_par([=](auto &S, auto p) { loop_go(S, p, a, b, f, l); });
    }
} // namespace vb
