#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace vb {
    struct task : public std::function<std::vector<task>(void)> {
        using std::function<std::vector<task>(void)>::function;
    };

    class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();

        void enqueue(task t);
        void runner();

    private:
        std::vector<task>        tasks;
        std::vector<std::thread> runners;
        std::mutex               tasks_m;
        std::atomic<int>         running = 0;
        bool                     stop    = false;
    };
} // namespace vb