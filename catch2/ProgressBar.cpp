#include "catch2/catch.hpp"
#include <vb/ProgressBar.h>

using namespace vb;

TEST_CASE("vb::ProgressBar") {
    auto l = Auto::tasks.size();
    {
        ProgressBar P(1e7);
        CHECK(Auto::tasks.size() == l + 1);
        CHECK(Auto::tasks.back().active);
        for (size_t i = 0; i < 10'000'000; ++i) P.set(i);
    }
    CHECK(!Auto::tasks.back().active);
}
