#include "catch2/catch.hpp"
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Constellation1") {
    char *                 argv[] = {(char *)"test_constellation1"};
    Hub                    H("Testing Constellation1", 1, argv);
    auto                   M = HLib().at("lat_SV");
    Constellation1<double> C(M);
    Constellation1<real_t> Cq(C);
    Cq.findn();
    CHECK(fmt::format("{}", *(guess(Cq.E.j(), 80))) == "z^2 + -914416 z + 590816592");
}
