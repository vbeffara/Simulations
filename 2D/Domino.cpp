#include <gsl/gsl>
#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Half {
public:
    uint8_t d, type;
    explicit Half(uint8_t _d = 0, uint8_t _t = 0) : d(_d), type(_t) {}
    explicit operator Color() {
        static const Color     C[] = {BLACK, BLACK, BLACK, BLACK, RED,    GREEN, YELLOW, BLUE, YELLOW, BLUE,
                                  RED,   GREEN, RED,   GREEN, YELLOW, BLUE,  YELLOW, BLUE, RED,    GREEN};
        static const gsl::span CC{C};
        return CC[d + 4 * type];
    }
};

class Tiling : public Bitmap<Half> {
public:
    void putd(ucoo c, uint8_t d) {
        at(c).d                    = d;
        at(ucoo(coo(c) + dz[d])).d = (d + 2) % 4;
        step();
    }
    void freeze(ucoo c) {
        at(c).type                          = 0;
        at(ucoo(coo(c) + dz[at(c).d])).type = 0;
    }

    explicit Tiling(const Hub &H) : Bitmap<Half>(H.title, {H['n'], H['n']}), r(H['r']), rr{r, r * r, 1, r} {
        for (size_t x = 0; x < w(); ++x)
            for (size_t y = 0; y < h(); ++y) { at({x, y}) = Half(2 * (x % 2), 1 + ((x + y) % 2) + 2 * (x % 2)); }
        if (H['o'] == "aztec") {
            for (size_t i = 0; i < h() / 2; ++i) {
                for (size_t j = 0; j < w() / 2 - i - 1; ++j) {
                    at({i, j}).type                     = 0;
                    at({w() - 1 - i, j}).type           = 0;
                    at({i, h() - 1 - j}).type           = 0;
                    at({w() - 1 - i, h() - 1 - j}).type = 0;
                }
                for (size_t j = 0; j < w(); ++j) {
                    putd({i, j}, 1 + 2 * ((i + j + h() / 2 + 1) % 2));
                    putd({i + w() / 2, j}, 1 + 2 * ((i + j + w() / 2) % 2));
                }
            }
        } else if (H['o'] == "hill") {
            for (size_t y = 0; y < h() / 2; ++y)
                for (size_t x = y; x < w() - y; x += 2) {
                    putd({x, y}, 0);
                    putd({x, h() - 1 - y}, 0);
                }
            for (size_t x = 0; x < w() / 2; ++x)
                for (size_t y = x + 1; y < h() - 1 - x; y += 2) {
                    putd({x, y}, 1);
                    putd({w() - 1 - x, y}, 1);
                }
        } else if (H['o'] == "hole") {
            for (size_t y = 0; y < h() / 2; ++y)
                for (size_t x = y; x < w() - y - 1; x += 2) {
                    putd({x, y}, 0);
                    putd({x + 1, h() - 1 - y}, 0);
                }
            for (size_t x = 0; x < w() / 2; ++x)
                for (size_t y = x + 1; y < h() - 1 - x; y += 2) {
                    putd({x, y}, 1);
                    putd({w() - 1 - x, y - 1}, 1);
                }
            ucoo mid{w() / 2, h() / 2};
            at(mid).type = 0;
            putd(ucoo(coo(mid) + coo{-1, -1}), 1);
            putd(ucoo(coo(mid) + coo{-1, 1}), 0);
            putd(ucoo(coo(mid) + coo{1, 1}), 3);
            putd(ucoo(coo(mid) + coo{1, -1}), 2);
            at(mid).type = 0;
            putd(ucoo(coo(mid) + coo{-2, -2}), 1);
            putd(ucoo(coo(mid) + coo{-2, 0}), 1);
            putd(ucoo(coo(mid) + coo{-2, 2}), 0);
            putd(ucoo(coo(mid) + coo{0, 2}), 0);
            at(mid).type = 0;
            putd(ucoo(coo(mid) + coo{2, 2}), 3);
            putd(ucoo(coo(mid) + coo{2, 0}), 3);
            putd(ucoo(coo(mid) + coo{2, -2}), 2);
            putd(ucoo(coo(mid) + coo{0, -2}), 2);
        } else {
            for (size_t x = 0; x < w(); x += 2)
                for (size_t y = 0; y < h(); ++y) putd({x, y}, 0);
            size_t b = H['b'];
            if (b > 0) {
                for (auto x = w() / 2 - b; x < w() / 2 + b; ++x) putd({x, h() / 2}, (2 + at({x, h() / 2}).d) % 4);
                at({w() / 2 - b, h() / 2}).type = 0;
                at({w() / 2 + b, h() / 2}).type = 0;
            }
        }
    }

    int flip(coo c) {
        if (at(ucoo(c)).type == 0) return 0;
        uint8_t d  = at(ucoo(c)).d;
        coo     oc = c + dz[d] + dz[(d + 1) % 4];
        if (!contains(oc)) return 0;
        if (at(ucoo(oc)).type == 0) return 0;
        if (at(ucoo(oc)).d != ((d + 2) % 4)) return 0;
        vector<double> rr{r, r * r, 1, r};
        if (prng.bernoulli(1 - rr[(d + at(ucoo(c)).type) % 4])) return 0;
        putd(ucoo(c), (d + 1) % 4);
        putd(ucoo(oc), (d + 3) % 4);
        return 1;
    }

    double         r;
    vector<double> rr;
};

int main(int argc, char **argv) {
    Hub     H("Domino tiling", argc, argv, "n=200,o=aztec|hill|hole|flat,b=0,f=0,r=1");
    Tiling  T(H);
    Console C;
    C.manage(T.r, 0.0, 1.0, "r");
    C.show();
    T.show();
    T.pause();
    auto f = int(T.w() * T.h() * double(H['f']));
    for (int64_t t = 1;; ++t) {
        T.flip(coo(prng.uniform_coo(T.size)));
        if (t == f) T.freeze({T.w() / 2, T.h() / 2});
    }
}
