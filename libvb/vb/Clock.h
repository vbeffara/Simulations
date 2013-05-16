/// @file
/// Definitions for the vb::Clock class

#pragma once
#include <vb/common.h>

namespace vb {
	class Task {
	public:
		Task (long p, void t (void*), void *d) : period(p), last(-1), active(true), task(t), data(d) {}

		void run () { task(data); }

		long period;	///< The period (in hundredths of a second) at which we should run.
		long last;  	///< The date (in hundredths of a second) of the last run of the task.
		bool active;	///< Whether the task should be run at all.
	private:
		void (*task) (void*);	///< The function to be run to perform the task.
		void * data;         	///< Context passed to vb::Task::task at each run.
	};

	class Clock {
	public:
		Clock ();

		long clock () const;  ///< Compute the elapsed time since creation, in clock ticks.
		double time () const; ///< Compute the elapsed time since creation, in seconds.
		long count () const;  ///< Compute the elapsed time since creation, in hundredths of a second.

		void step () { ++n_call; --next; if (!next) run(); }

		void run ();

		int add (long period, void task (void*), void *data = 0);

		void remove (int i);

	private:
		long next;
		unsigned long long slice;
		unsigned long long n_call;
		std::vector <Task> T;
	};

	extern Clock global_clock;
}
