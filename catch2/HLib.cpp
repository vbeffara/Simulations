#include "catch2/catch_test_macros.hpp"
#include <cstring>
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Hypermap library") {
    std::vector<char *> argv{strdup("test_hlib"), nullptr};
    Hub const           H("Testing hypermap lib", 1, argv.data());
    CHECK(HLib().at("tripod_l").sigma.size() == 4);
    CHECK(!H_artem(12).is_simple(6));
    CHECK(H_genus0(15).is_triangulation());
    CHECK(H_genus1(18).is_graph());
}
