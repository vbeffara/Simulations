#include "catch2/catch.hpp"
#include <vb/data/Array.h>

using namespace vb;

TEST_CASE("vb::Array") {
    Array<int> A({23, 45}, 1);
    CHECK(A.fits(ucoo{6, 3}));
    CHECK(!A.fits(ucoo{23, 1}));
    CHECK(!A.fits(ucoo{5, 45}));

    A.put({1, 1}, 2);
    CHECK(A.at({3, 4}) == 1);
    CHECK(A.at({1, 1}) == 2);

    A.putp({5 * 23 + 4, 7 * 45 + 10}, 0);
    CHECK(A.atp({23 + 4, -45 + 10}) == 0);

    A.at(prng.uniform_coo(A.size)) += 10;
    int s = 0;
    for (auto z : coo_range(A.size)) s += A[z];
    CHECK(s == 23 * 45 + 10);
}
