#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class FCBM : public Image {
public:
    FCBM(const std::string &s, size_t n, double p) : Image(s, {2 * n, 2 * n}), mid({n - (n % 2), n - (n % 2)}), n(n), p(p) {
        for (size_t x = 0; x < 2 * n; ++x)
            for (size_t y = 0; y < 2 * n; ++y)
                if ((x % 2 == 0) && (y % 2 == 0)) put({x, y}, WHITE);
    }

    [[nodiscard]] auto degree(coo z) const -> int {
        int out = 0;
        for (unsigned i = 0; i < 4; ++i)
            if (atp(z + dz[i]) != BLACK) ++out;
        return out;
    }

    auto dlr(coo z, coo d) -> double {
        if ((z == coo(mid)) || (z + 2 * d == coo(mid))) return 1;
        if (atp(z) != atp(z + 2 * d)) return p;
        if ((atp(z) == WHITE) && (atp(z + 2 * d) == WHITE)) return 0;
        if (atp(z + d) == BLACK) return p;
        if ((degree(z) == 1) || (degree(z + 2 * d) == 1)) return p;

        double out = p;
        atp(z + d) = BLACK;
        fill(ucoo(z), WHITE);
        if (atp(z) != atp(z + 2 * d)) out = 1;
        fill(ucoo(z), RED);
        atp(z + d) = RED;
        return out;
    }

    void run() {
        while (true) {
            auto   z  = prng.uniform_coo(mid) * 2;
            auto   d  = dz[prng.uniform_int(4U)];
            double pp = dlr(coo(z), d);
            fill(mid, WHITE);
            atp(coo(z) + d) = prng.bernoulli(pp) ? WHITE : BLACK;
            fill(mid, RED);
            step();
        }
    }

    ucoo   mid;
    size_t n;
    double p;
};

auto main(int argc, char **argv) -> int {
    Hub  H("Fully Connected Bond Percolation", argc, argv, "n=300,p=.5");
    FCBM P(H.title, H['n'], H['p']);
    P.show();
    P.run();
}
