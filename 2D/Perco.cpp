#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

const Color IN{128, 128, 128};

class Perco : public Image {
public:
    Perco(const std::string &title, size_t n, double p_) : Image(title, {n, n}), mid({n / 2, n / 2}), p(p_) {
        for (size_t x = 0; x < n; ++x) {
            for (size_t y = 0; y < n; ++y) {
                if ((x % 2 == 0) && (y % 2 == 0))
                    put({x, y}, IN);
                else if ((x % 2 == 0) || (y % 2 == 0))
                    put({x, y}, prng.bernoulli(p) ? IN : BLACK);
            }
        }
        fill(mid, RED);
        show();
    }

    void run(bool c, int s) {
        int t = 0;
        while (true) {
            if ((s > 0) && ((t++ % s) == 0)) snapshot();
            auto z = prng.uniform_coo(size);
            if ((z.x + z.y) % 2 == 0) continue;
            fill(mid, IN);
            if (c)
                swap(at(z), atp(coo(z) + dz[4U + prng.uniform_int(4U)]));
            else
                at(z) = prng.bernoulli(p) ? IN : BLACK;
            fill(mid, RED);
            step();
        }
    }

    ucoo   mid;
    double p;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Percolation");
    auto n = clp.param("n", size_t(500), "Grid size");
    auto pp = clp.param("p", 0.5, "Occupation prob.");
    auto cont = clp.flag("c", "Conservative dynamics (swap)");
    auto drive = clp.flag("d", "Run dynamics (else pause)");
    auto snap = clp.param("s", int(0), "Snapshot period (0=off)");
    clp.finalize();

    Perco P(clp.title, n, pp);
    if (drive)
        P.run(cont, snap);
    else
        P.pause();
}
