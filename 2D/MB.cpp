#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Brownian motion");
    auto n = clp.param("n", 500, "half grid size");
    clp.finalize();

    Image img(clp.title, {2 * size_t(n), 2 * size_t(n)});

    for (auto z : coo_range(img.size)) img.put(z, WHITE);
    ucoo zz{size_t(n), size_t(n)};
    while (img.fits(zz)) {
        img.put(zz, BLACK);
        zz += dz[prng() % 4];
    }
    img.output(clp.title);
}
