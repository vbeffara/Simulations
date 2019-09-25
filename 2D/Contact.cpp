#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Discrete-time contact process on Z^2");
    auto p = clp.param("c", .4, "Contagion rate");
    auto n = clp.param("n", size_t(400), "Simulation size");
    clp.finalize();

    Image img(clp.title, {n, n});

    for (size_t x = 0; x < n; ++x)
        for (size_t y = 0; y < n / 2; ++y) img.put({x, y}, WHITE);
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
