#include "catch2/catch.hpp"
#include <vb/Coloring.h>

using vb::cpx, vb::Coloring, vb::Indexed;

TEST_CASE("vb::Coloring") {
    Coloring C("Testing Coloring", cpx(0, 0), cpx(1, 1), 500, [](cpx z) { return Indexed(int(real(z) * imag(z) * 10)); });
    C.scale(1.2);
    C.shift(cpx(.2, .3));
    C.show();
    C.hide();
}
