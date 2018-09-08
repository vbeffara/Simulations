#pragma once /// @file
#include <vb/util.h>
#include <cmath>
#include <complex>

namespace vb {
    using cpx = std::complex<double>;

    const cpx I(0, 1);

    template <typename T> struct cpx_t { using type = std::complex<T>; };

#ifdef UNIT_TESTS
    TEST_CASE("vb::cpx") {
        cpx z(1.0, 2.3);
        CHECK(pretty(z) == "(1,2.3)");
    }
#endif
} // namespace vb
