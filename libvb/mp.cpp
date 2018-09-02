#include <vb/mp.h>

namespace vb {
    template <> complex_t to_cpx<real_t>(const real_t & x, const real_t & y) { return complex_t(x, y); };
} // namespace vb
