#include <boost/chrono.hpp>
#include <vb/util/misc.h>

namespace vb {
    auto time() -> double {
        static auto                           basetime = boost::chrono::process_real_cpu_clock::now();
        boost::chrono::duration<double> const dur      = boost::chrono::process_real_cpu_clock::now() - basetime;
        return dur.count();
    }
} // namespace vb
