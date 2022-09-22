#include <spdlog/spdlog.h>
#include <vb/Elliptic.h>
#include <vb/Hypermap_lib.h>
#include <vb/Toroidal.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    Hub const H("Toroidal triangulation", argc, argv, "n=4,o=0,m=4,f,g=lat_C5");
    Toroidal G(HLib().at(H['g']));

    for (int i = 0; i < int(H['o']); ++i) {
        G.split_edges();
        G.from_hypermap();
    }

    for (int i = H['o']; i <= int(H['n']); ++i) {
        spdlog::info("Step {}", i);
        G.pack();
        if (H['f']) G.flip();

        spdlog::info("  tau = {}", G.m);
        spdlog::info("  j   = {}", Elliptic<double>(exp(I * M_PI * G.m)).j());

        if (i < int(H['n'])) {
            G.split_edges();
            G.from_hypermap();
        }
    }

    G.output_pdf(H.title, H['m']);
}
