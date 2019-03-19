#include "catch2/catch.hpp"
#include <vb/Constellation0.h>

using namespace vb;

TEST_CASE("vb::Constellation0") {
    char *                 argv[] = {(char *)"test_constellation0"};
    Hub                    H("Testing Constellation0", 1, argv, "s=3,m=228,d=2,g=0,v,o,b,q");
    auto                   M = HLib().at("m_dodecahedron");
    Constellation0<double> C(M);
    Constellation0<real_t> Cq(C);
    Cq.findn();
    Cq.belyi();
    Polynomial<complex_t> Q{1};
    for (auto zd : Cq.f)
        for (unsigned j = 0; j < zd.d; ++j) Q.add_root(zd.z);
    for (unsigned i = 0; i < Q.size(); ++i) {
        auto &x  = Q[i];
        auto  xx = complex_t(round(real(x)), round(imag(x)));
        if (abs(x - xx) < 1e-90) x = xx;
    }
    CHECK(fmt::format("{}", Q) == "z^55 + -55 z^50 + 1205 z^45 + -13090 z^40 + 69585 z^35 + -134761 z^30 + -69585 z^25 + -13090 z^20 + "
                                  "-1205 z^15 + -55 z^10 + -1 z^5");
}
