#pragma once /// \file

#include <chrono>
#include <functional>
#include <vector>

namespace vb {
	using Duration = std::chrono::duration<double>;
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock,Duration>;

	class Task : public std::function<void()> {
	public:
		Task (TimePoint t, Duration d, std::function<void()> f);
		TimePoint next;
		Duration period;
		bool active;
	};

	class Auto : public std::chrono::steady_clock {
	public:
		Auto (double t);
		virtual ~Auto ();
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
	};
}
