#pragma once /// @file
#include <complex>
#include <fmt/ostream.h>

namespace vb {
    using cpx = std::complex<double>;

    constexpr cpx I(0, 1);

    template <typename T> struct cpx_t { using type = std::complex<T>; };
} // namespace vb

template <> struct fmt::formatter<vb::cpx> : fmt::ostream_formatter {}; // TODO: do this better
