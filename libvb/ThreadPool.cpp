#include <vb/ThreadPool.h>

namespace vb {
    void ThreadPool::runner() {
        task t;
        while (!(stop && tasks.empty() && (running == 0))) {
            {
                std::lock_guard<std::mutex> l(tasks_m);
                if (tasks.empty()) continue;
                t = tasks.back();
                ++running;
                tasks.pop_back();
            }
            const auto                  ts = t();
            std::lock_guard<std::mutex> l(tasks_m);
            for (const auto & tt : ts) tasks.push_back(tt);
            --running;
        }
    }

    ThreadPool::ThreadPool() {
        int nt = std::thread::hardware_concurrency();
        for (int i = 0; i < (nt > 0 ? nt : 1); ++i) runners.emplace_back([=] { runner(); });
    }

    ThreadPool::ThreadPool(task t) : ThreadPool() {
        std::lock_guard<std::mutex> l(tasks_m);
        tasks.push_back(std::move(t));
    }

    ThreadPool::~ThreadPool() {
        stop = true;
        for (auto & t : runners) t.join();
    }
} // namespace vb