#pragma once /// \file
#include <boost/chrono.hpp>
#include <functional>
#include <vector>

namespace vb {
    using Duration  = boost::chrono::duration<double>;
    using TimePoint = boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>;

    class Task : public std::function<void()> {
    public:
        Task(TimePoint t, Duration d, std::function<void()> f);
        TimePoint next;
        Duration  period;
        bool      active;
    };

    class Auto : public boost::chrono::process_real_cpu_clock {
    public:
        explicit Auto(double t);
        virtual ~Auto();

        Auto(const Auto &) = delete;
        Auto(Auto &&)      = delete;
        Auto &operator=(const Auto &) = delete;
        Auto &operator=(Auto &&) = delete;

        virtual void update() = 0;

        void step() {
            ++n_call;
            --next;
            if (next == 0) run();
        }

        static size_t add_task(double period, const std::function<void()> &task);
        static void   remove_task(size_t i);

        static inline std::vector<Task> tasks;
        TimePoint                       start;

    private:
        void run();

        int     next   = 1;
        double  slice  = 1;
        int64_t n_call = 0;
        size_t  task;

    protected:
        bool die = false, hold = false;
    };
} // namespace vb
