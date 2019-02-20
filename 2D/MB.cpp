#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub H("Brownian motion", argc, argv, "n=500");
    int n = H['n'];

    Image img(H.title, {2 * n, 2 * n});

    for (auto z : coo_range(img.size)) img.put(z, WHITE);
    coo z{n, n};
    while (img.contains(z)) {
        img.put(z, BLACK);
        z += dz[prng() % 4];
    }
    img.output(H.title);
}
