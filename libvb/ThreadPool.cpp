#include <vb/ThreadPool.h>
#include <atomic>
#include <future>
#include <mutex>
#include <thread>

namespace vb {
    Project Parallel(std::vector<Job> js) { return {std::move(js)}; }

    void execute_plain(Job t) {
        for (auto f : t().jobs) execute_plain(f);
    }

    void execute_async(Job t) {
        std::vector<std::future<void>> ts;
        for (auto f : t().jobs) ts.emplace_back(std::async([f]() { execute_async(f); }));
    }

    void execute_parallel(Job t) {
        Project    P{{std::move(t)}};
        std::mutex m;

        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                std::optional<Job> t;
                while (P.njobs > 0) {
                    if (!t) {
                        std::lock_guard<std::mutex> l(m);
                        if (P.jobs.empty()) continue;
                        t = std::move(P.jobs.back());
                        P.jobs.pop_back();
                    }
                    auto ts = (*t)();
                    {
                        std::lock_guard<std::mutex> l(m);
                        P.njobs += ts.jobs.size() - 1;
                    }
                    if (ts.jobs.empty()) {
                        t = {};
                    } else {
                        for (int i = 0; i < ts.jobs.size() - 1; ++i) {
                            std::lock_guard<std::mutex> l(m);
                            P.jobs.push_back(std::move(ts.jobs[i]));
                        }
                        t = std::move(ts.jobs.back());
                    }
                }
            });
        for (auto & t : runners) t.join();
    }
} // namespace vb