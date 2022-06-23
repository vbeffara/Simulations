#include "catch2/catch_test_macros.hpp"
#include <cstring>
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Constellation1") {
    std::vector<char *>    argv{strdup("test_constellation1"), nullptr};
    Hub                    H("Testing Constellation1", 1, argv.data());
    auto                   M = HLib().at("lat_SV");
    Constellation1<double> C(M);
    Constellation1<real_t> Cq(C);
    Cq.findn();
    CHECK(fmt::format("{}", *(guess(Cq.E.j(), 80))) == "z^2 + -914416 z + 590816592");
}
