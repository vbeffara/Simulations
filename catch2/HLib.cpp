#include "catch2/catch.hpp"
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Hypermap library") {
    char *argv[] = {(char *)"test_hlib"};
    Hub   H("Testing hypermap lib", 1, argv);
    CHECK(HLib().at("tripod_l").sigma.size() == 4);
    CHECK(!H_artem(12).is_simple(6));
    CHECK(H_genus0(15).is_triangulation());
    CHECK(H_genus1(18).is_graph());
}
