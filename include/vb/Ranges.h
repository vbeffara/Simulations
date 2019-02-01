#pragma once
#include <vb/data/Array.h>

namespace vb {
    template <typename T>[[deprecated]] auto coos(const Array<T> &A) { return coo_range(A.size); }
} // namespace vb
