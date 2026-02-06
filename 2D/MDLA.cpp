#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

constexpr Color EMPTY = BLACK;
constexpr Color AWAY  = Color(50, 50, 50);
constexpr Color ALIVE = Color(0, 200, 0);
constexpr Color DEAD  = Color(255, 0, 0);

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Aggregation of exclusion walkers");
    auto n = clp.param("n", 250, "half of board size");
    auto p = clp.param("p", .5, "initial particle density");
    auto g = clp.flag("g", "dynamic discovery of environment (ghosts)");
    auto a = clp.param("a", 1.0, "contagion probability");
    auto t = clp.param("t", 0.0, "snapshot interval for movies");
    auto s = clp.flag("s", "silent mode");
    clp.finalize();

    Image img(clp.title, {2 * size_t(n), 2 * size_t(n)});

    for (auto z : coo_range(img.size))
        if (g)
            img.put(z, AWAY);
        else
            img.put(z, prng.bernoulli(p) ? ALIVE : EMPTY);
    img.put({size_t(n), size_t(n)}, DEAD);
    img.show();
    if (t > 0) img.snapshot_setup("MDLA", t);

    while (true) {
        auto z   = prng.uniform_coo(img.size);
        auto nnz = coo(z) + dz[prng() % 4];
        if (!img.fits(nnz)) continue;
        auto nz = ucoo(nnz);
        if (img.at(z) == img.at(nz)) continue;

        bool flag = false;
        if (img.at(z) == AWAY) {
            if (prng.bernoulli(a) || (img.at(nz) == DEAD)) {
                flag = true;
                img.put(z, (prng.bernoulli(p) ? ALIVE : EMPTY));
            } else
                continue;
        }
        if (img.at(nz) == AWAY) {
            if (prng.bernoulli(a) || (img.at(z) == DEAD)) {
                flag = true;
                img.put(nz, (prng.bernoulli(p) ? ALIVE : EMPTY));
            } else
                continue;
        }

        if (img.at(z) == img.at(nz)) continue;
        if (img.at(z) != ALIVE) std::swap(z, nz);
        if (img.at(z) != ALIVE) continue;

        if (img.at(nz) == EMPTY) {
            if (prng.bernoulli(a) || flag) {
                img.put(z, EMPTY);
                img.put(nz, ALIVE);
            }
        } else if (img.at(nz) == DEAD) {
            img.put(z, DEAD);
            if ((z.x == 0) || (z.x == 2 * n - 1) || (z.y == 0) || (z.y == 2 * n - 1)) break;
        }
    }
}
