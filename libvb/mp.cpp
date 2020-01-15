#include <vb/util/mp.h>

namespace vb {
    template <> auto sum<real_t>(const std::function<real_t(int)> &f) -> real_t {
        real_t out = 0, eps = pow(real_t(.1), 10 + real_t::default_precision());
        for (int n = 0;; ++n) {
            real_t dd = f(n);
            out += dd;
            if (abs(dd) < eps) break;
        }
        return out;
    }
    template <> auto sum<complex_t>(const std::function<complex_t(int)> &f) -> complex_t {
        complex_t out(0);
        real_t    eps = pow(real_t(.1), 10 + real_t::default_precision());
        for (int n = 0;; ++n) {
            complex_t dd = f(n);
            out += dd;
            if (real(abs(dd)) < eps) break;
        }
        return out;
    }

    void default_precision(unsigned n) noexcept {
        real_t::default_precision(n);
        complex_t::default_precision(n);
    }

    static const int mp_dummy = (default_precision(100), 0);
} // namespace vb
