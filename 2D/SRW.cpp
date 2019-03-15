#include <vb/Figure.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>

int main(int argc, char **argv) {
    vb::Hub  H("Simple random walk", argc, argv, "a=15,o=1");
    unsigned adj = H['a'];

    auto                  C    = std::make_unique<vb::Circle>(0, .5);
    vb::cpx &             z    = C->z;
    auto                  P    = std::make_unique<vb::Path>(std::vector<vb::cpx>{0});
    std::vector<vb::cpx> &path = P->z;
    vb::Figure            f(H.title);
    f.ortho = H['o'];
    f.add(std::move(C));
    f.add(std::move(P));
    f.margin = 1;
    f.show();

    for (int a = 1; a < 200; ++a) {
        for (int b = 0; b < a; ++b) {
            unsigned d = 9;
            while (((1u << d) & adj) == 0) { d = vb::prng.uniform_int(8u); }
            z += vb::cpx(vb::dz[d].x, vb::dz[d].y);
            path.push_back(z);
        }
        f.update();
    }
}
