#pragma once /// \file

#include <chrono>
#include <functional>
#include <vector>

namespace vb {
	typedef std::chrono::duration<double> Duration;
	typedef std::chrono::time_point<std::chrono::steady_clock,Duration> TimePoint;

	class Task : public std::function<void()> {
	public:
		Task (TimePoint t, Duration d, std::function<void()> f) : std::function<void()> (f), next(t), period(d), active(true) {}
		TimePoint next;
		Duration period;
		bool active;
	};

	class Auto : public std::chrono::steady_clock {
	public:
		Auto (double t) : start(now()), next(1), slice(10), n_call(0), task {add_task(t, [this]{this->update();})} {}
		virtual ~Auto () { remove_task(task); }
		virtual void update() =0;

		void step () { ++n_call; --next; if (!next) run(); }

		static int add_task (double period, std::function<void()> task) { 
			tasks.emplace_back(now(),Duration(period),task);
			return tasks.size()-1;
		}

		static void remove_task (int i) { tasks[i].active = false; }

		static std::vector <Task> tasks;
		TimePoint start;

	private:
		void run () {
			for (Task & t : tasks) if (t.active && (now() >= t.next)) { t(); t.next = now() + t.period; }
			double time = Duration(now()-start).count();
			if (time < .1) slice += slice/10; else slice = std::min (n_call / (time*100), slice + slice/10.0);
			next = slice;
		}

		unsigned long long next, slice, n_call;
		int task;
	};
}