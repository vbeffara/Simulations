#include "catch2/catch.hpp"
#include <vb/math/LinearAlgebra.h>
#include <vb/util/mp.h>

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
