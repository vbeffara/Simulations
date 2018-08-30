#pragma once /// @file
#include <vb/util.h>
#include <cmath>
#include <complex>

namespace vb {
    using cpx = std::complex<double>;

    const cpx I(0, 1);
    const cpx dzc[4] = {cpx(1, 0), cpx(0, 1), cpx(-1, 0), cpx(0, -1)};

    template <typename T> class cpx_t {
    public:
        using type = std::complex<T>;
    };

    template <typename T> typename cpx_t<T>::type to_cpx(const T & x, const T & y) { return typename cpx_t<T>::type(x, y); }
    template <typename T> typename cpx_t<T>::type to_cpx(const cpx & z) { return to_cpx<T>(real(z), imag(z)); }

#ifdef UNIT_TESTS
    TEST_CASE("vb::cpx") {
        cpx z(1.0, 2.3);
        CHECK(str(z) == "(1,2.3)");
    }
#endif
} // namespace vb
