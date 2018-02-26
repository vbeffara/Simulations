#pragma once
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>

namespace vb {
    namespace rv = ranges::view;
#ifdef __cpp_coroutines
    namespace re = ranges::experimental;
#endif
} // namespace vb
