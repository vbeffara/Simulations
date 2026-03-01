#include <gsl/gsl>
#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Half {
public:
    uint8_t d, type;
    explicit Half(uint8_t _d = 0, uint8_t _t = 0) : d(_d), type(_t) {}
    explicit operator Color() const {
        static const vector<Color> C{BLACK, BLACK, BLACK, BLACK, RED,    GREEN, YELLOW, BLUE, YELLOW, BLUE,
                                     RED,   GREEN, RED,   GREEN, YELLOW, BLUE,  YELLOW, BLUE, RED,    GREEN};
        return C[d + 4U * type];
    }
};

class Tiling : public Bitmap<Half> {
public:
    void putd(ucoo c, uint8_t d) {
        at(c).d         = d;
        at(c + dz[d]).d = (d + 2) % 4U;
        step();
    }
    void freeze(ucoo c) {
        at(c).type               = 0;
        at(c + dz[at(c).d]).type = 0;
    }

    explicit Tiling(const string &title, size_t n, const string &o, size_t b, double r_) : Bitmap<Half>(title, {n, n}), r(r_), rr{r, r * r, 1, r} {
        for (const auto &z : coo_range(size)) at(z) = Half(2 * (z.x % 2), uint8_t(1 + ((z.x + z.y) % 2) + 2 * (z.x % 2)));
        if (o == "aztec") {
            for (size_t ii = 0; ii < size.y / 2; ++ii) {
                for (size_t j = 0; j < size.x / 2 - ii - 1; ++j) {
                    at({ii, j}).type                           = 0;
                    at({size.x - 1 - ii, j}).type              = 0;
                    at({ii, size.y - 1 - j}).type              = 0;
                    at({size.x - 1 - ii, size.y - 1 - j}).type = 0;
                }
                for (size_t j = 0; j < size.x; ++j) {
                    putd({ii, j}, uint8_t(1 + 2 * ((ii + j + size.y / 2 + 1) % 2)));
                    putd({ii + size.x / 2, j}, uint8_t(1 + 2 * ((ii + j + size.x / 2) % 2)));
                }
            }
        } else if (o == "hill") {
            for (size_t y = 0; y < size.y / 2; ++y)
                for (size_t x = y; x < size.x - y; x += 2) {
                    putd({x, y}, 0);
                    putd({x, size.y - 1 - y}, 0);
                }
            for (size_t x = 0; x < size.x / 2; ++x)
                for (size_t y = x + 1; y < size.y - 1 - x; y += 2) {
                    putd({x, y}, 1);
                    putd({size.x - 1 - x, y}, 1);
                }
        } else if (o == "hole") {
            for (size_t y = 0; y < size.y / 2; ++y)
                for (size_t x = y; x < size.x - y - 1; x += 2) {
                    putd({x, y}, 0);
                    putd({x + 1, size.y - 1 - y}, 0);
                }
            for (size_t x = 0; x < size.x / 2; ++x)
                for (size_t y = x + 1; y < size.y - 1 - x; y += 2) {
                    putd({x, y}, 1);
                    putd({size.x - 1 - x, y - 1}, 1);
                }
            ucoo const mid = size / 2;
            at(mid).type = 0;
            putd(mid + coo{-1, -1}, 1);
            putd(mid + coo{-1, 1}, 0);
            putd(mid + coo{1, 1}, 3);
            putd(mid + coo{1, -1}, 2);
            at(mid).type = 0;
            putd(mid + coo{-2, -2}, 1);
            putd(mid + coo{-2, 0}, 1);
            putd(mid + coo{-2, 2}, 0);
            putd(mid + coo{0, 2}, 0);
            at(mid).type = 0;
            putd(mid + coo{2, 2}, 3);
            putd(mid + coo{2, 0}, 3);
            putd(mid + coo{2, -2}, 2);
            putd(mid + coo{0, -2}, 2);
        } else {
            for (size_t x = 0; x < size.x; x += 2)
                for (size_t y = 0; y < size.y; ++y) putd({x, y}, 0);
            if (b > 0) {
                for (auto x = size.x / 2 - b; x < size.x / 2 + b; ++x) putd({x, size.y / 2}, (2 + at({x, size.y / 2}).d) % 4U);
                at({size.x / 2 - b, size.y / 2}).type = 0;
                at({size.x / 2 + b, size.y / 2}).type = 0;
            }
        }
    }

    auto flip(coo c) -> int {
        if (at(ucoo(c)).type == 0) return 0;
        uint8_t const d  = at(ucoo(c)).d;
        coo const     oc = c + dz[d] + dz[(d + 1) % 4U];
        if (!fits(oc)) return 0;
        if (at(ucoo(oc)).type == 0) return 0;
        if (at(ucoo(oc)).d != ((d + 2) % 4)) return 0;
        vector<double> rs{r, r * r, 1, r};
        if (prng.bernoulli(1 - rs[(d + at(ucoo(c)).type) % 4])) return 0;
        putd(ucoo(c), (d + 1) % 4U);
        putd(ucoo(oc), (d + 3) % 4U);
        return 1;
    }

    double         r;
    vector<double> rr;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Domino tiling");
    auto n = clp.param("n", 200UL, "grid size");
    auto o = clp.param("o", "aztec"s, "initial configuration (aztec|hill|hole|flat)");
    auto b = clp.param("b", 0UL, "boundary size");
    auto f = clp.param("f", 0.0, "freeze time factor");
    auto r = clp.param("r", 1.0, "flip ratio");
    clp.finalize();

    Tiling  T(clp.title, n, o, b, r);
    Console C;
    C.manage(T.r, 0.0, 1.0, "r");
    C.show();
    T.show();
    T.pause();
    auto ff = int64_t(double(T.size.x * T.size.y) * f);
    for (int64_t t = 1;; ++t) {
        T.flip(coo(prng.uniform_coo(T.size)));
        if (t == ff) T.freeze(T.size / 2);
    }
}
