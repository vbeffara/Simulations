#pragma once
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>
#include <vb/Array.h>

namespace vb {
    namespace rv = ranges::view;
#ifdef __cpp_coroutines
    namespace re = ranges::experimental;
#endif

    template <typename T> auto coos(const Array<T> & A) {
        return rv::cartesian_product(rv::ints(int64_t(0), A.ww), rv::ints(int64_t(0), A.hh)) |
               rv::transform([](auto xy) { return std::make_from_tuple<coo>(xy); });
    }
} // namespace vb
