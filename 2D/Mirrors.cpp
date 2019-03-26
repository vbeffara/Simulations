#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

constexpr unsigned STATE_NONE    = 0U;
constexpr unsigned STATE_PRESENT = 128U;
constexpr unsigned STATE_NE      = 1U;
constexpr unsigned STATE_FLIP    = 2U;
constexpr unsigned STATE_VISITED = 4U;

constexpr Color MIRROR_NONE();
constexpr Color MIRROR_NE(255, 0, 0);
constexpr Color MIRROR_NW(255, 255, 0);
constexpr Color MIRROR_FLIP_NE(0, 0, 255);
constexpr Color MIRROR_FLIP_NW(0, 255, 0);
constexpr Color MIRROR_NE_VISITED(128, 0, 0);
constexpr Color MIRROR_NW_VISITED(128, 128, 0);
constexpr Color MIRROR_FLIP_NE_VISITED(0, 0, 128);
constexpr Color MIRROR_FLIP_NW_VISITED(0, 128, 0);

namespace vb {
    template <> Color to_Color(uint8_t t) {
        if (t == 0) return BLACK;
        if (t == STATE_VISITED) return Grey(128);
        static const Color     colors[] = {MIRROR_NW,         MIRROR_NE,         MIRROR_FLIP_NW,         MIRROR_FLIP_NE,
                                       MIRROR_NW_VISITED, MIRROR_NE_VISITED, MIRROR_FLIP_NW_VISITED, MIRROR_FLIP_NE_VISITED};
        static const gsl::span ccolors{colors};
        return ccolors[t % 8];
    }
} // namespace vb

class Mirrors : public Bitmap<uint8_t> {
public:
    explicit Mirrors(const Hub &H) : Bitmap<uint8_t>(H.title, {H['n'], H['n']}), p(H['p']), q(H['q']), f(H['f']) {}
    void   main();
    double p, q, f;
};

void Mirrors::main() {
    show();

    while (true) {
        for (auto z : coo_range(size)) {
            auto &m = at(z);
            m       = STATE_NONE;
            if (prng.bernoulli(1 - q)) {
                m |= STATE_PRESENT;
                if (prng.bernoulli(p)) m |= STATE_NE;
                if (prng.bernoulli(f)) m |= STATE_FLIP;
            }
        }

        auto z = coo(ucoo{w() / 2, h() / 2});
        for (size_t t = 0, d = 0; (t < 8 * w() * h()) && fits(z); ++t) {
            if ((at(ucoo(z)) & STATE_PRESENT) != 0) {
                static const unsigned  flip_ne[] = {1, 0, 3, 2}, flip_nw[] = {3, 2, 1, 0};
                static const gsl::span fne{flip_ne}, fnw{flip_nw};
                if ((at(ucoo(z)) & STATE_NE) != 0)
                    d = fne[gsl::index(d)];
                else
                    d = fnw[gsl::index(d)];
                if ((at(ucoo(z)) & STATE_FLIP) != 0) at(ucoo(z)) ^= STATE_NE;
            }
            at(ucoo(z)) |= STATE_VISITED;
            z += dz[gsl::index(d)];
        }

        pause();
    }
}

int main(int argc, char **argv) {
    Hub H("Mirror model", argc, argv, "n=200,p=.5,q=0,f=0");
    Mirrors(H).main();
}
