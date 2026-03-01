#include <vb/Figure.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "Simple random walk");
    auto adj = clp.param("a", 15, "Adjacency bitmask");
    auto o   = clp.param("o", 1, "Orthogonal mode");
    clp.finalize();

    auto                  C    = std::make_unique<vb::Circle>(0, .5);
    vb::cpx &             z    = C->z;
    auto                  P    = std::make_unique<vb::Path>(std::vector<vb::cpx>{0});
    std::vector<vb::cpx> &path = P->z;
    vb::Figure            f(clp.title);
    f.ortho = o;
    f.add(std::move(C));
    f.add(std::move(P));
    f.margin = 1;
    f.show();

    for (int a = 1; a < 200; ++a) {
        for (int b = 0; b < a; ++b) {
            unsigned d = 9;
            while (((1U << d) & unsigned(adj)) == 0) { d = vb::prng.uniform_int(8U); }
            z += vb::cpx(double(vb::dz[d].x), double(vb::dz[d].y));
            path.push_back(z);
        }
        f.update();
    }
}
