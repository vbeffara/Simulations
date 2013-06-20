#pragma once /// @file
#include <vector>

namespace vb {
	class Task { public:
		Task (long p, void t (void*), void *d) : period(p), last(-1), active(true), task(t), data(d) {}

		void run () { task(data); }

		long period;	///< The period (in hundredths of a second) at which we should run.
		long last;  	///< The date (in hundredths of a second) of the last run of the task.
		bool active;	///< Whether the task should be run at all.
	private:
		void (*task) (void*);	///< The function to be run to perform the task.
		void * data;         	///< Context passed to vb::Task::task at each run.
	};

	class Clock { public:
		Clock () : next(1), slice(10), n_call(0) {}

		long  	clock	() const	{ return ::clock();                       	}
		double	time 	() const	{ return double(clock()) / CLOCKS_PER_SEC;	}
		long  	count	() const	{ return time() * 100;                    	}
		void  	step 	()      	{ ++n_call; --next; if (!next) run();     	}

		int add (long period, void task (void*), void *data = 0) { T.push_back(Task(period,task,data)); return T.size()-1; }

		void remove (int i) { if (i>=0) T[i].active = false; }

		void run () {
			for (Task & t : T) if (t.active && (count() >= t.last + t.period)) { t.run(); t.last = count(); }
			if (count() <= 100) slice += slice/10;
			else slice = std::min (n_call / count(), slice + slice/10);
			next = slice;
		}

	private:
		long next;
		unsigned long long slice, n_call;
		std::vector <Task> T;
	};

	extern Clock global_clock;
}
