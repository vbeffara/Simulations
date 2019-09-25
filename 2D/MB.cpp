#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    Hub    H("Brownian motion", argc, argv, "n=500");
    size_t n = H['n'];

    Image img(H.title, {2 * n, 2 * n});

    for (auto z : coo_range(img.size)) img.put(z, WHITE);
    ucoo z{n, n};
    while (img.fits(z)) {
        img.put(z, BLACK);
        z += dz[prng() % 4];
    }
    img.output(H.title);
}
