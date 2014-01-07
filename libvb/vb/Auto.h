#pragma once /// \file

#include <vb/Clock.h>

namespace vb {
	void Auto_update (void *);

	class Auto {
	public:
		Auto (double t) : task { global_clock.add(100*t,Auto_update,this) } { }
		virtual ~Auto () { global_clock.remove (task); }

		void step () { global_clock.step(); }
		virtual void update() =0;

	private:
		int task;
	};
}