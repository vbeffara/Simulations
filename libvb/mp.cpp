#include <vb/mp.h>
#include <spdlog/fmt/ostr.h>

namespace vb {
    template <> real_t sum<real_t>(const std::function<real_t(int)> & f) {
        real_t out = 0, eps = pow(real_t(.1), 10 + real_t::default_precision());
        for (int n = 0;; ++n) {
            real_t dd = f(n);
            out += dd;
            if (abs(dd) < eps) break;
        }
        return out;
    }
    template <> complex_t sum<complex_t>(const std::function<complex_t(int)> & f) {
        complex_t out(0);
        real_t    eps = pow(real_t(.1), 10 + real_t::default_precision());
        for (int n = 0;; ++n) {
            complex_t dd = f(n);
            out += dd;
            if (real(abs(dd)) < eps) break;
        }
        return out;
    }

    std::string pretty(const real_t & t) {
        if (mpz_int ti(round(t)); ti == t) return fmt::format("{}", ti);
        return fmt::format("{}", t);
    }

    std::string pretty(const complex_t & t) {
        if (imag(t) == 0) return pretty(real(t));
        if (real(t) == 0) return fmt::format("({} I)", pretty(imag(t)));
        return fmt::format("({} + {} I)", pretty(real(t)), pretty(imag(t)));
    }
} // namespace vb
