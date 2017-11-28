#include <vb/ThreadPool.h>

namespace vb {
    void ThreadPool::enqueue(task t) {
        std::lock_guard l(tasks_m);
        tasks.push_back(t);
    }

    void ThreadPool::runner() {
        task t;
        while (!(stop && tasks.empty() && (running == 0))) {
            {
                std::lock_guard l(tasks_m);
                if (tasks.empty()) continue;
                t = tasks.back();
                ++running;
                tasks.pop_back();
            }
            for (auto tt : t()) enqueue(tt);
            --running;
        }
    }

    ThreadPool::ThreadPool() {
        int nt = std::thread::hardware_concurrency();
        for (int i = 0; i < (nt ? nt : 1); ++i) runners.push_back(std::thread([=] { runner(); }));
    }

    ThreadPool::~ThreadPool() {
        stop = true;
        for (auto & t : runners) t.join();
    }
} // namespace vb