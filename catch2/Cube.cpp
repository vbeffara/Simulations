#include "catch2/catch_test_macros.hpp"
#include <vb/Cube.h>

using namespace vb;

TEST_CASE("vb::Cube") {
    Cube C("Testing Cube", {100, 100, 100});
    C.put(prng.uniform_coo3(C.size), 1);
    int s = 0;
    for (auto v : C) s += v;
    CHECK(s == 1);
    CHECK(Color(C.Bitmap<Adder>::at({0, 0})).a == 255);
    C.output_pov("Testing Cube");
}
