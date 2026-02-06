#include <vb/CoarseImage.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Once-reinforced random walk");
    auto n = clp.param("n", 1000, "grid size");
    auto a = clp.param("a", 2.0, "reinforcement strength");
    clp.finalize();

    double aa = 1 / (1 + a);
    auto   L  = size_t(pow(double(n), .33));

    CoarseImage img(clp.title, {size_t(n), size_t(n)}, L);
    img.show();

    for (ucoo z{size_t(n) / 2, size_t(n) / 2};;) {
        img.put(z, true);
        auto nz = z + dz[prng.uniform_int(4U)];
        if (!img.fits(nz)) break;
        if (img.at(nz) || prng.bernoulli(aa)) z = nz;
    }
}
