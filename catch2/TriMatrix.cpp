#include "catch2/catch.hpp"
#include <vb/data/TriMatrix.h>

using namespace vb;

TEST_CASE("vb::TriMatrix") {
    TriMatrix<int> M;
    M.put({35, 42}, 3);
    M.put({1234, 5678}, 5);
    M.put({91823749, -2793474}, 23);

    CHECK(M.at({35, 42}) == 3);
    CHECK(M.at({1234, 5678}) == 5);
    CHECK(M.at({91823749, -2793474}) == 23);
    CHECK(M.at({3, 4}) == 0);
    CHECK(M.at({981327, 2371827}) == 0);
}
