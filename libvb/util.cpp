#include <vb/util.h>
#include <chrono>

namespace vb {
	double time () {
		static std::chrono::system_clock C;
		static auto basetime = C.now();
		std::chrono::duration<double> dur = C.now() - basetime;
		return dur.count();
	}
}
