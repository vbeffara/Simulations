#pragma once /// \file

#include <functional>
#include <vector>

namespace vb {
	class Task : public std::function<void()> {
	public:
		Task (long p, std::function<void()> f) : std::function<void()> (f), period(p), last(-1), active(true) {}

		double period, last;
		bool active;
	};

	class Auto {
	public:
		Auto (double t) : next(1), slice(10), n_call(0), start(0) {
			task = add_task(t, [this]{this->update();});
			start = time();
		}
		virtual ~Auto () { remove_task(task); }

		virtual void update() =0;

		void step () { ++n_call; --next; if (!next) run(); }
		double time () { return double(::clock()) / CLOCKS_PER_SEC - start; }

		static int add_task (long period, std::function<void()> task) { 
			tasks.push_back(Task(period,task));
			return tasks.size()-1;
		}

		static void remove_task (int i) { tasks[i].active = false; }

		static std::vector <Task> tasks;

	private:
		void run () {
			for (Task & t : tasks) if (t.active && (time() >= t.last + t.period)) { t(); t.last = time(); }
			if (time() < .1) slice += slice/10; else slice = std::min (n_call / (time()*100), slice + slice/10.0);
			next = slice;
		}

		unsigned long long next, slice, n_call;
		double start;
		int task;
	};
}