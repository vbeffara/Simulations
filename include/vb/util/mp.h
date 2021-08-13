#pragma once
#include <boost/multiprecision/mpc.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <vb/math/math.h>
#undef Success
#include <Eigen/Dense>
#include <fmt/ostream.h>

namespace vb {
    using boost::multiprecision::mpz_int;
    using complex_t = boost::multiprecision::mpc_complex;
    using real_t    = boost::multiprecision::mpfr_float;

    void default_precision(unsigned n) noexcept;

    template <> struct cpx_t<real_t> { using type = complex_t; };

    template <> auto sum<real_t>(const std::function<real_t(int)> &f) -> real_t;
    template <> auto sum<complex_t>(const std::function<complex_t(int)> &f) -> complex_t;
} // namespace vb

namespace Eigen {
    template <> struct NumTraits<vb::complex_t> : GenericNumTraits<vb::complex_t> {
        using Real       = vb::real_t;
        using NonInteger = vb::complex_t;
        using Literal    = vb::complex_t;
        using Nested     = vb::complex_t;

        static inline auto epsilon() -> Real { return 0; }
        static inline auto dummy_precision() -> int { return 0; }
        static inline auto digits10() -> int { return 0; }

        enum {
            IsInteger = 0,
            IsSigned  = 1,
            IsComplex = 1,
        };
    };
} // namespace Eigen

template <> struct fmt::formatter<vb::real_t> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::real_t &x, FormatContext &ctx) {
        return format_to(ctx.out(), "{}", x.str(x.precision()));
    }
};

template <> struct fmt::formatter<vb::complex_t> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::complex_t &z, FormatContext &ctx) {
        if (imag(z) == 0) return format_to(ctx.out(), "{}", real(z));
        return format_to(ctx.out(), "({} + {} I)", real(z), imag(z));
    }
};
