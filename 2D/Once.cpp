#include <vb/CoarseImage.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Once-reinforced random walk", argc, argv, "n=1000,a=2");
    size_t n = H['n'];
    double a = H['a'];
    a        = 1 / (1 + a);
    auto L   = size_t(pow(n, .33));

    CoarseImage img(H.title, {n, n}, L);
    img.show();

    for (coo z{int(n) / 2, int(n) / 2};;) {
        img.put(z, true);
        coo nz = z + dz[prng() % 4];
        if (!img.contains(nz)) break;
        if (img.at(nz) || prng.bernoulli(a)) z = nz;
    }
}
