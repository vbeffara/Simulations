#pragma once
#include <vb/math/LinearAlgebra.h>

namespace vb {
    template <typename T> Vector<T> solve(const Matrix<T> &A, const Vector<T> &y) { return A.fullPivLu().solve(y); }
} // namespace vb
