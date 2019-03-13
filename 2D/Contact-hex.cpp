#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub H("Discrete-time contact process on the hexagonal lattice", argc, argv, "p=.44,n=400");

    double p = H['p'];
    size_t n = H['n'];

    Image img(H.title, {n, n});

    for (int x = 0; x < n; ++x)
        for (int y = 0; y < n / 2; ++y) img.put({x, y}, WHITE);
    img.show();

    for (int i = 0; i < 3 * n; i++) {
        int total = 0;
        for (int x = 1; x < n - 1; x++) {
            for (int y = 1; y < n - 1; y++) {
                if ((x + y + i) % 3 == 0) {
                    bool nb = false;
                    nb |= (img.at(coo{x - 1, y - 1}) == WHITE);
                    nb |= (img.at(coo{x, y + 1}) == WHITE);
                    nb |= (img.at(coo{x + 1, y}) == WHITE);
                    if (nb && prng.bernoulli(p)) {
                        img.put({x, y}, WHITE);
                        total++;
                    } else {
                        img.put({x, y}, BLACK);
                    }
                }
            }
        }
        if (total == 0) break;
    }
}
