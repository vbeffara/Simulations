#include <fmt/ostream.h>
#include <sstream>
#include <vb/util/PRNG.h>

namespace vb {
    auto PRNG::discrete(const std::vector<double> &p) -> unsigned {
        double   U = uniform_real();
        unsigned i = 0;
        while (U > p[i]) {
            U -= p[i];
            ++i;
        }
        return i;
    }

    auto PRNG::state() -> std::string { return fmt::format("{}", *this); }

    void PRNG::state(const std::string &s) { std::istringstream(s) >> (*this); }

    PRNG prng;
} // namespace vb
