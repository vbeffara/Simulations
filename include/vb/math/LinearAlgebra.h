#pragma once
#undef Success
#include <Eigen/Dense>

namespace vb {
    // TODO: change to composition to get access by unsigned index
    template <typename T> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    template <typename T> using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    template <typename T> Vector<T> solve(const Matrix<T> &A, const Vector<T> &y);
} // namespace vb
