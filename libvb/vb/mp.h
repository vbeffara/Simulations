#pragma once
#include <vb/cpx.h>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpc.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <cln/cln.h>
#undef Success
#include <Eigen/Dense>

namespace cln {
    inline cl_R norm(const cl_N & z) { return realpart(z) * realpart(z) + imagpart(z) * imagpart(z); }
    inline cl_R abs2(const cl_N & z) { return norm(z); }
    inline cl_N conj(const cl_N & z) { return conjugate(z); }
    inline cl_R real(const cl_N & z) { return realpart(z); }
    inline cl_R imag(const cl_N & z) { return imagpart(z); }
} // namespace cln

namespace vb {
    using boost::multiprecision::mpc_complex;
    using boost::multiprecision::mpf_float;
    using boost::multiprecision::mpfr_float;
    using boost::multiprecision::mpz_int;

    using complex_t = mpc_complex;
    using real_t    = mpfr_float;

    template <> class cpx_t<real_t> {
    public:
        using type = complex_t;
    };

    static cln::cl_R pow(const cln::cl_R & z, int k) { return expt(z, k); }
    static cln::cl_N pow(const cln::cl_N & z, const cln::cl_N & k) { return expt(z, k); }
    using std::pow;

    static int round(const cln::cl_R & z) { return cl_I_to_int(round1(z)); }
    using std::round;

    static bool isnormal(const cln::cl_N &) { return true; }
    using std::isnormal;

    static cln::cl_R log10(const cln::cl_R & z) { return log(z, 10); }
    using std::log10;

    template <typename T> int to_int(const T & z) { return int(z); }
    template <> int           to_int(const cln::cl_I & z);
    template <> int           to_int(const cln::cl_R & z);

    template <> complex_t to_cpx<real_t>(const real_t & x, const real_t & y);
} // namespace vb

namespace Eigen {
    template <> struct NumTraits<vb::complex_t> : GenericNumTraits<vb::complex_t> {
        using Real       = vb::real_t;
        using NonInteger = vb::complex_t;
        using Literal    = vb::complex_t;
        using Nested     = vb::complex_t;

        static inline Real epsilon() { return 0; }
        static inline int  dummy_precision() { return 0; }
        static inline int  digits10() { return 0; }

        enum {
            IsInteger = 0,
            IsSigned  = 1,
            IsComplex = 1,
        };
    };
} // namespace Eigen
