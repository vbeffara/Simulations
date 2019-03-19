#include "catch2/catch.hpp"
#include <vb/math/math.h>

using namespace vb;

TEST_CASE("math library") {
    CHECK(sign(3) == 1);
    CHECK(sign(-2.0) == -1.0);
    CHECK(sign(0) == 0);

    CHECK(fact(3) == 6);
    CHECK(binom(5, 2) == 10);
    CHECK(catalan(3) == 5);

    CHECK(abs(sin(pi_<double>())) < 1e-10);

    CHECK(abs(q_<double>(cpx(1.0 + 1e-5)) - q_<double>(cpx(1.0)) - 1e-5 * q_t<double>(cpx(1.0))) < 1e-9);

    auto e = sum<cpx>([](int n) {
        auto out = 1.0 / fact(n);
        return std::isinf(out) ? 0 : out;
    });
    CHECK(abs(log(abs(e)) - 1) < 1e-6);
}
