#pragma once /// \file
#include <fmt/ostream.h>

namespace vb {
    // TODO: customize fmt directly
    template <typename T> std::string pretty(const T &t) { return fmt::format("{}", t); }
} // namespace vb
