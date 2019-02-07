#include <vb/util/misc.h>

namespace vb {
    double time() {
        static auto                     basetime = boost::chrono::process_real_cpu_clock::now();
        boost::chrono::duration<double> dur      = boost::chrono::process_real_cpu_clock::now() - basetime;
        return dur.count();
    }
} // namespace vb
