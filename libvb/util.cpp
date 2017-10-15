#include <vb/util.h>

namespace vb {
	double time () {
		static auto basetime = std::chrono::system_clock::now();
		std::chrono::duration<double> dur = std::chrono::system_clock::now() - basetime;
		return dur.count();
	}
} // namespace vb
