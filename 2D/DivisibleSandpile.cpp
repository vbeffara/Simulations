#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

template <> auto vb::to_Color(double t) -> Color {
    if (t < -1e-9) return HSV(.5, t / (t - 1), 1);
    if (t > +1e-9) return HSV(0, t / (1 + t), 1);
    return BLACK;
}

class Sandpile : public Bitmap<double> {
public:
    Sandpile(const Hub &H, size_t n) : Bitmap(H.title, {n, n}) {
        for (auto z : coo_range(size)) put(z, prng.gaussian(H['m'], H['s']));
    }

    void swipe(const Hub &H) {
        for (auto z : coo_range(size))
            if (double excess = at(z); excess > 0) {
                for (int i = 0; i < 4; ++i) atp(coo(z) + dz[i]) += excess / 4;
                at(z) = 0;
            }
        if (H['r']) {
            double m = 0;
            for (auto z : coo_range(size))
                if (double mm = abs(at(z)); mm > m) m = mm;
            if (m > 0)
                for (auto z : coo_range(size)) at(z) = at(z) * (100 / m);
        }
        update();
    }
};

auto main(int argc, char **argv) -> int {
    Hub      H("Divisible sandpile", argc, argv, "n=500,m=.01,s=10,r");
    Sandpile S(H, H['n']);
    S.show();
    while (true) { S.swipe(H); }
}
