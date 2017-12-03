#include <atomic>
#include <mutex>
#include <optional>
#include <thread>
#include <vb/ThreadPool.h>

namespace vb {
    void execute_plain(Job t) {
        const auto ts = t();
        for (auto f : ts) execute_plain(f);
    }

    void execute_parallel(Job t) {
        std::vector<Job> tasks{std::move(t)};
        std::atomic<int> ntasks{1};
        std::mutex       tasks_m;

        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                std::optional<Job> t;
                while (ntasks > 0) {
                    if (!t) {
                        std::lock_guard<std::mutex> l(tasks_m);
                        if (tasks.empty()) continue;
                        t = std::move(tasks.back());
                        tasks.pop_back();
                    }
                    auto ts = (*t)();
                    ntasks += ts.size() - 1;
                    if (ts.empty()) {
                        t = {};
                    } else {
                        for (int i = 0; i < ts.size() - 1; ++i) {
                            std::lock_guard<std::mutex> l(tasks_m);
                            tasks.push_back(std::move(ts[i]));
                        }
                        t = std::move(ts.back());
                    }
                }
            });
        for (auto & t : runners) t.join();
    }
} // namespace vb