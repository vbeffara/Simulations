#include "catch2/catch_test_macros.hpp"
#include <vb/util/coo.h>

using vb::coo;

TEST_CASE("vb::coo") {
    coo z1{2, 3}, z2{4, -1};
    CHECK(z1 + z2 == coo{6, 2});
    CHECK(z1 - z2 == coo{-2, 4});
    CHECK(-z1 == coo{-2, -3});
    CHECK(z1 * 3 == coo{6, 9});
    CHECK(z1 / 2 == coo{1, 1});

    z1 += z2;
    z2 -= z1;
    CHECK(norm(z2) == 13);
    CHECK(sup(z2) == 3);
}
