#include "catch2/catch_test_macros.hpp"
#include <vb/math/Permutation.h>

using namespace vb;

#ifdef UNIT_TESTS
TEST_CASE("vb::Permutation") {
    Permutation P1 = Transposition(4, 0, 1), P2 = Transposition(4, 0, 2), P = P1 * P2;
    Passport    PP = P.passport();
    CHECK(PP[0].first == 3);
}
#endif
