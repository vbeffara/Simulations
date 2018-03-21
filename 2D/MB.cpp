#include <vb/Image.h>
#include <vb/Ranges.h>

using namespace vb;

int main(int argc, char ** argv) {
    H.init("Brownian motion", argc, argv, "n=500");
    int n = H['n'];

    Image img(2 * n, 2 * n);

    for (auto z : coos(img)) img.put(z, WHITE);
    coo z(n, n);
    while (img.contains(z)) {
        img.put(z, BLACK);
        z += dz[prng() % 4];
    }
    img.output();
}
