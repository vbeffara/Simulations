#include <vb/Auto.h>

namespace vb {
    Task::Task(TimePoint t, Duration d, std::function<void()> f) : std::function<void()>(std::move(f)), next(t), period(d), active(true) {}

    Auto::Auto(double t) : start(now()), task{add_task(t, [this] { this->update(); })} {}

    Auto::~Auto() { remove_task(task); }

    int Auto::add_task(double period, std::function<void()> task) {
        tasks.emplace_back(now(), Duration(period), task);
        return tasks.size() - 1;
    }

    void Auto::remove_task(int i) { tasks[i].active = false; }

    void Auto::run() {
        if (die && (!hold)) exit(0);
        for (Task & t : tasks)
            if (t.active && (now() >= t.next)) {
                t();
                t.next = now() + t.period;
            }
        double time = Duration(now() - start).count();
        slice *= 1.01;
        if (time > .1) slice = std::min(.1 * n_call / time, slice);
        next = int(slice);
    }
} // namespace vb
