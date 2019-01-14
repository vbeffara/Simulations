#pragma once
#undef Success
#include <Eigen/Dense>

namespace vb {
    template <typename T> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    template <typename T> using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    template <typename T> Vector<T> solve(const Matrix<T> &A, const Vector<T> &y);
} // namespace vb

#ifdef UNIT_TESTS
#include <vb/mp.h> // nograph
using namespace vb;
TEST_CASE("Linear Algebra wrapper") {
    Matrix<real_t> m(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) m(i, j) = pow(2 * i + 1, j);
    Vector<real_t> v(3);
    for (int i = 0; i < 3; ++i) v(i) = 3 * i - 2;
    Vector<real_t> x(3);
    x << -3.5, 1.5, 0;
    CHECK((solve(m, v) - x).squaredNorm() < 1e-50);
}
#endif
