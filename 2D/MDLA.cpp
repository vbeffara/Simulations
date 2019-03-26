#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

constexpr Color EMPTY = BLACK;
constexpr Color AWAY  = Color(50, 50, 50);
constexpr Color ALIVE = Color(0, 200, 0);
constexpr Color DEAD  = Color(255, 0, 0);

int main(int argc, char **argv) {
    Hub    H("Aggregation of exclusion walkers", argc, argv, "n=250,p=.5,g,a=1.0,t=0.0,s");
    size_t n = H['n']; // Half of board size
    double p = H['p']; // Initial particle density
    bool   g = H['g']; // Dynamic discovery of environment (ghosts)
    double t = H['t']; // Snapshot interval for movies
    double a = H['a']; // Contagion probability

    Image img(H.title, {2 * n, 2 * n});

    for (auto z : coo_range(img.size))
        if (g)
            img.put(z, AWAY);
        else
            img.put(z, prng.bernoulli(p) ? ALIVE : EMPTY);
    img.put({n, n}, DEAD);
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
