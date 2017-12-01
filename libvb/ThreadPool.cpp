#include <vb/ThreadPool.h>

namespace vb {
    void ThreadPool::enqueue(task t) {
        std::lock_guard<std::mutex> l(tasks_m);
        tasks.push_back(std::move(t));
    }

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
            for (const auto & tt : t()) enqueue(tt);
            --running;
        }
    }

    ThreadPool::ThreadPool() {
        int nt = std::thread::hardware_concurrency();
        for (int i = 0; i < (nt > 0 ? nt : 1); ++i) runners.emplace_back([=] { runner(); });
    }

    ThreadPool::ThreadPool(task t) : ThreadPool() { enqueue(std::move(t)); }

    ThreadPool::~ThreadPool() {
        stop = true;
        for (auto & t : runners) t.join();
    }
} // namespace vb