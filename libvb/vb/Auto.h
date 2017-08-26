#pragma once /// \file
#include <vb/Hub.h>
#include <functional>
#include <vector>

namespace vb {
	class Task : public std::function<void()> {
	public:
		Task (TimePoint t, Duration d, std::function<void()> f);
		TimePoint next;
		Duration period;
		bool active;
	};

	class Auto : public boost::chrono::process_real_cpu_clock {
	public:
		Auto (double t);
		virtual ~Auto ();

		Auto (const Auto &) = delete;
		Auto (Auto &&) = delete;
		Auto & operator= (const Auto &) = delete;
		Auto & operator= (Auto &&) = delete;

		virtual void update() =0;

		void step () { ++n_call; --next; if (next==0) run(); }

		static int add_task (double period, std::function<void()> task);
		static void remove_task (int i);

		static std::vector <Task> tasks;
		TimePoint start;

	private:
		void run ();

		int next, slice;
		long long n_call;
		int task;

	protected:
		bool die=false, hold=false;
	};
}
