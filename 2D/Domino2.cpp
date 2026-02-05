#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

vector<vector<vector<double>>> W, hues;
double                         minw, maxw, contrast = .2;

class Domino {
public:
    explicit Domino(coo z_ = {0, 0}, unsigned d_ = 0, bool active_ = false) : z(z_), d(d_), active(active_) {
        if (d >= 2) {
            z += dz[d];
            d -= 2;
        }
    }

    [[nodiscard]] auto weight() const -> double { return W[pmod(z.x, W.size())][pmod(z.y, W[0].size())][d]; }

    explicit operator Color() const {
        if (!active) return BLACK;
        double const hh = hues[pmod(z.x, W.size())][pmod(z.y, W[0].size())][d];
        return HSV(hh, 1, contrast + (1 - contrast) * (weight() - minw) / (maxw - minw));
    }

    coo      z;
    unsigned d;
    bool     active;
};

class Tiling : public Bitmap<Domino> {
public:
    explicit Tiling(const string &title, size_t n) : Bitmap<Domino>(title, {2 * n, 2 * n}) {
        for (size_t ii = 0; ii < n; ++ii)
            for (size_t j = n - 1 - ii; j < n + ii; j += 2) {
                putd(Domino{{int(ii), int(j)}, 1});
                putd(Domino{{int(2 * n - 1 - ii), int(j)}, 1});
            }
    };

    void putd(Domino d) {
        d.active = true;
        putp(d.z, d);
        putp(d.z + dz[d.d], d);
    }

    void flip(coo z) {
        Domino d1 = atp(z);
        if (!d1.active) return;
        z         = d1.z;
        auto   d  = d1.d;
        coo const zz = z + dz[(d + 1) % 4];
        Domino d2 = atp(zz);
        if ((!d2.active) || (d2.z != zz) || (d2.d != d)) return;
        double const w1 = d1.weight() * d2.weight();
        d1        = Domino{z, (d + 1) % 4};
        d2        = Domino{z + dz[d], (d + 1) % 4};
        double const w2 = d1.weight() * d2.weight();
        if ((w2 >= w1) || prng.bernoulli(w2 / w1)) {
            putd(d1);
            putd(d2);
        }
    }

    void run() {
        while (visible()) {
            flip(prng.uniform_coo(coo(size)));
            step();
        }
    };

    static vector<vector<vector<double>>> W;
};

void two_by_two(double a) {
    W          = vector<vector<vector<double>>>(2, vector<vector<double>>(2, vector<double>(2, 1)));
    W[0][0][0] = a;
    W[0][0][1] = a;
    W[0][1][0] = a;
    W[1][0][1] = a;
    minw       = min(a, 1.0);
    maxw       = max(a, 1.0);
    if (maxw == minw) minw -= 1;
}

void three_by_two(double a) {
    W          = vector<vector<vector<double>>>(6, vector<vector<double>>(2, vector<double>(2, 1)));
    W[0][0][1] = a;
    W[3][1][1] = a;
    minw       = min(a, 1.0);
    maxw       = max(a, 1.0);
    if (maxw == minw) minw -= 1;
}

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Domino tiling (v2)");
    auto n = clp.param("n", 100UL, "grid size");
    auto a = clp.param("a", 0.5, "weight parameter");
    clp.finalize();

    two_by_two(a);
    hues = W;
    Console C;
    C.manage(contrast, 0.0, 1.0, "contrast");
    for (auto &x : hues)
        for (auto &y : x)
            for (auto &z : y) {
                z = prng.uniform_real();
                C.manage(z, 0.0, 1.0, nullptr);
            }
    Tiling D(clp.title, n);
    C.show();
    D.show();
    D.run();
    D.pause();
}
