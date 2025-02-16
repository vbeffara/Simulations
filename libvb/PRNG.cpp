#include <fmt/ostream.h>
#include <sstream>
#include <vb/util/PRNG.h>

namespace vb {
  auto PRNG::discrete(const std::vector<double> &p) -> unsigned {
    double   U  = uniform_real();
    unsigned ii = 0;
    while (U > p[ii]) {
      U -= p[ii];
      ++ii;
    }
    return ii;
  }

  auto PRNG::state() -> std::string { return fmt::format("{}", *this); }

  void PRNG::state(const std::string &s) { std::istringstream(s) >> (*this); }

  thread_local PRNG prng;
} // namespace vb
