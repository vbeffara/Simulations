#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub H("Discrete-time contact process on Z^2", argc, argv, "p=.4,n=400");

    double p = H['p'];
    size_t n = H['n'];

    Image img(H.title, {n, n});

    for (size_t x = 0; x < n; ++x)
        for (size_t y = 0; y < n / 2; ++y) img.put({int(x), int(y)}, WHITE);
    img.show();

    for (size_t i = 0; i < 3 * n; i++) {
        int total = 0;
        for (size_t x = 1; x < n - 1; x++) {
            for (size_t y = 1; y < n - 1; y++) {
                if ((x + y + i) % 2 == 0) {
                    bool nb = false;
                    nb |= (img.at({x - 1, y}) == WHITE);
                    nb |= (img.at({x + 1, y}) == WHITE);
                    nb |= (img.at({x, y - 1}) == WHITE);
                    nb |= (img.at({x, y + 1}) == WHITE);
                    if (nb && prng.bernoulli(p)) {
                        img.put({int(x), int(y)}, WHITE);
                        total++;
                    } else {
                        img.put({int(x), int(y)}, BLACK);
                    }
                }
            }
        }
        if (total == 0) break;
    }
}
