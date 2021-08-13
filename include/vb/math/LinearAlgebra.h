#pragma once
#undef Success
#include <Eigen/Dense>

namespace vb {
    template <typename T> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    template <typename T> using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    template <typename T> auto solve(const Matrix<T> &A, const Vector<T> &y) -> Vector<T>;
} // namespace vb
