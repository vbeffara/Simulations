#include <vb/mp.h>

namespace vb {
    template <> complex_t to_cpx<real_t>(const real_t & x, const real_t & y) { return complex_t(x, y); };

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
} // namespace vb
