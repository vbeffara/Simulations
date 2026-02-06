#include <vb/Elliptic.h>
#include <vb/Hypermap_lib.h>
#include <vb/Toroidal.h>
#include <vb/util/CLP.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Toroidal triangulation");
    auto n = clp.param("n", 4, "number of subdivision steps");
    auto o = clp.param("o", 0, "initial subdivision level");
    auto m = clp.param("m", 4, "output margin");
    auto f = clp.flag("f", "enable flipping");
    auto g = clp.param("g", std::string("lat_C5"), "hypermap name");
    clp.finalize();

    Toroidal G(HLib().at(g));

    for (int i = 0; i < o; ++i) {
        G.split_edges();
        G.from_hypermap();
    }

    for (int i = o; i <= n; ++i) {
        spdlog::info("Step {}", i);
        G.pack();
        if (f) G.flip();

        spdlog::info("  tau = {}", G.m);
        spdlog::info("  j   = {}", Elliptic<double>(exp(I * M_PI * G.m)).j());

        if (i < n) {
            G.split_edges();
            G.from_hypermap();
        }
    }

    G.output_pdf(clp.title, m);
}
