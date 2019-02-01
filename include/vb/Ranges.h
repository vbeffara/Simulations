#pragma once
#include <range/v3/experimental/utility/generator.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <vb/data/Array.h>

namespace vb {
    namespace rv = ranges::view;
#ifdef __cpp_coroutines
    namespace re = ranges::experimental;
#endif

    template <typename T> auto coos(const Array<T> &A) {
        return rv::cartesian_product(rv::ints(int64_t(0), A.ww), rv::ints(int64_t(0), A.hh)) |
               rv::transform([](std::tuple<int64_t, int64_t> xy) -> coo {
                   const auto [x, y] = xy;
                   return {x, y};
               });
    }
} // namespace vb
