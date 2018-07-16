#include <vb/PRNG.h>
#include <spdlog/fmt/ostr.h>
#include <sstream>

namespace vb {
    int PRNG::discrete(const std::vector<double> & p) {
        double U = uniform_real();
        int    i = 0;
        while (U > p[i]) {
            U -= p[i];
            ++i;
        }
        return i;
    }

    std::string PRNG::state() { return fmt::format("{}", *this); }

    void PRNG::state(const std::string & s) { std::istringstream(s) >> (*this); }

    PRNG prng;
} // namespace vb
