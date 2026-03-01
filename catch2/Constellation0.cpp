#include "catch2/catch_test_macros.hpp"
#include <cstring>
#include <vb/Constellation0.h>
#include <vb/Hypermap_lib.h>

using namespace vb;

TEST_CASE("vb::Constellation0") {
    INFO("Setting up HLib");
    auto M = HLib().at("m_dodecahedron");

    INFO("Creating Constellation0<double>");
    Constellation0<double> const C(M);

    INFO("Creating Constellation0<real_t>");
    Constellation0<real_t> Cq(C);

    INFO("Belyi embedding");
    Cq.findn();
    Cq.belyi();
    Cq.belyi();

    INFO("Computing polynomial Q");
    Polynomial<complex_t> Q{1};
    for (const auto &zd : Cq.f) Q.add_root(zd.z, zd.d);
    for (size_t i = 0; i < Q.size(); ++i) {
        auto &x  = Q[i];
        auto  xx = complex_t(round(real(x)), round(imag(x)));
        if (abs(x - xx) < 1e-90) x = xx;
    }
    CHECK(fmt::format("{}", Q) == "z^55 + -55 z^50 + 1205 z^45 + -13090 z^40 + 69585 z^35 + -134761 z^30 + -69585 z^25 + -13090 z^20 + "
                                  "-1205 z^15 + -55 z^10 + -1 z^5");
}
