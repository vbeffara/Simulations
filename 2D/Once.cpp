#include <vb/CoarseImage.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    Hub    H("Once-reinforced random walk", argc, argv, "n=1000,a=2");
    size_t n = H['n'];
    double a = H['a'];
    a        = 1 / (1 + a);
    auto L   = size_t(pow(double(n), .33));

    CoarseImage img(H.title, {n, n}, L);
    img.show();

    for (ucoo z{n / 2, n / 2};;) {
        img.put(z, true);
        auto nz = z + dz[prng.uniform_int(4U)];
        if (!img.fits(nz)) break;
        if (img.at(nz) || prng.bernoulli(a)) z = nz;
    }
}
