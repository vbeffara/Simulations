#include "catch2/catch_test_macros.hpp"
#include <vb/Figure.h>
#include <vb/util/PRNG.h>

using namespace vb;

TEST_CASE("vb::Figure") {
    Figure F("Testing Figure");
    for (int i = 0; i < 10; ++i)
        F.add(std::make_unique<Segment>(cpx(prng.uniform_real(-5, 5), prng.uniform_real(-5, 5)),
                                        cpx(prng.uniform_real(-5, 5), prng.uniform_real(-5, 5)), Pen(Indexed(i, .6, .9))));

    for (int i = 1; i <= 5; ++i) F.add(std::make_unique<Circle>(cpx(0, 0), i));
    F.add(std::make_unique<Dot>(cpx(0, 0)));
    F.add(std::make_unique<Polygon>(std::vector<cpx>{{0, 1}, {1, 1}, {1, 0}}));
    F.add(std::make_unique<Path>(std::vector<cpx>{{0.1, 1}, {1.1, 1}, {1.1, 0}}));
    F.show();
    F.hide();
}
