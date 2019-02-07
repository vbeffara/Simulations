#pragma once /// @file
#include <complex>

namespace vb {
    using cpx = std::complex<double>;

    inline constexpr cpx I(0, 1);

    template <typename T> struct cpx_t { using type = std::complex<T>; };
} // namespace vb
