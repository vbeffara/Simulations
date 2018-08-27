#pragma once /// @file
#include <vb/util.h>
#include <cln/cln.h>
#include <cmath>
#include <complex>

namespace cln {
    inline cl_R norm(const cl_N & z) { return realpart(z) * realpart(z) + imagpart(z) * imagpart(z); }
    inline cl_R abs2(const cl_N & z) { return norm(z); }
    inline cl_N conj(const cl_N & z) { return conjugate(z); }
    inline cl_R real(const cl_N & z) { return realpart(z); }
    inline cl_R imag(const cl_N & z) { return imagpart(z); }
} // namespace cln

namespace vb {
    using cpx = std::complex<double>;

    const cpx I(0, 1);
    const cpx dzc[4] = {cpx(1, 0), cpx(0, 1), cpx(-1, 0), cpx(0, -1)};

    using real_t    = cln::cl_R;
    using complex_t = cln::cl_N;

    template <typename T> class cpx_t {
    public:
        using type = std::complex<T>;
    };
    template <> class cpx_t<real_t> {
    public:
        using type = complex_t;
    };

    static cln::cl_R pow(const cln::cl_R & z, int k) { return expt(z, k); }
    static cln::cl_N pow(const cln::cl_N & z, const cln::cl_N & k) { return expt(z, k); }
    using std::pow;

    static int round(const real_t & z) { return cl_I_to_int(round1(z)); }
    using std::round;

    static bool isnormal(const cln::cl_N &) { return true; }
    using std::isnormal;

    static cln::cl_R log10(const cln::cl_R & z) { return log(z, 10); }
    using std::log10;

    template <typename T> int to_int(const T & z) { return int(z); }
    template <> int           to_int(const cln::cl_I & z);
    template <> int           to_int(const cln::cl_R & z);

    template <typename T> typename cpx_t<T>::type to_cpx(const T & x, const T & y) { return typename cpx_t<T>::type(x, y); }
    template <typename T> typename cpx_t<T>::type to_cpx(const cpx & z) { return to_cpx<T>(real(z), imag(z)); }
    template <> complex_t                         to_cpx<real_t>(const real_t & x, const real_t & y);

#ifdef UNIT_TESTS
    TEST_CASE("vb::cpx") {
        cpx z(1.0, 2.3);
        CHECK(str(z) == "(1,2.3)");
    }
#endif
} // namespace vb
