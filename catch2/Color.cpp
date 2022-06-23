#include "catch2/catch_test_macros.hpp"
#include <vb/Color.h>

using vb::Color, vb::Grey;

TEST_CASE("vb::Color") {
    Color c = Grey(123);
    CHECK(c == Color(123, 123, 123));
    CHECK(c != Color(122, 123, 123));
}
