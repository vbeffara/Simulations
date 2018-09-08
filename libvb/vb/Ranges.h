#pragma once
#include <vb/Array.h>
#include <range/v3/experimental/utility/generator.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>

namespace vb {
    namespace rv = ranges::view;
#ifdef __cpp_coroutines
    namespace re = ranges::experimental;
#endif

    template <typename T> auto coos(const Array<T> & A) {
        return rv::cartesian_product(rv::ints(int64_t(0), A.ww), rv::ints(int64_t(0), A.hh)) | rv::transform([](auto xy) -> coo {
                   const auto [x, y] = xy;
                   return {x, y};
               });
    }
} // namespace vb
