#include "catch2/catch_test_macros.hpp"
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Constellation1") {
    auto                         M = HLib().at("lat_SV");
    Constellation1<double> const C(M);
    Constellation1<real_t>       Cq(C);
    Cq.findn();
    CHECK(fmt::format("{}", *(guess_LLL(Cq.E.j(), 80))) == "z^2 + -914416 z + 590816592");
}
