#pragma once
#include <vb/math.h>
#include <boost/multiprecision/mpc.hpp>
#include <boost/multiprecision/mpfr.hpp>
#undef Success
#include <Eigen/Dense>

namespace vb {
    using boost::multiprecision::mpz_int;
    using complex_t = boost::multiprecision::mpc_complex;
    using real_t    = boost::multiprecision::mpfr_float;

    template <> struct cpx_t<real_t> { using type = complex_t; };

    template <> real_t    sum<real_t>(const std::function<real_t(int)> &f);
    template <> complex_t sum<complex_t>(const std::function<complex_t(int)> &f);

    std::string pretty(const real_t &t);
    std::string pretty(const complex_t &t);
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
