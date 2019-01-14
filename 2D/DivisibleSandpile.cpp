#include <vb/Bitmap.h>
#include <vb/Ranges.h>

using namespace vb;

template <> Color vb::to_Color(double t) {
    if (t < -1e-9) return HSV(.5, t / (t - 1), 1);
    if (t > +1e-9) return HSV(0, t / (1 + t), 1);
    return BLACK;
}

class Sandpile : public Bitmap<double> {
public:
    explicit Sandpile(int n) : Bitmap(n, n) {
        for (auto z : coos(*this)) put(z, prng.gaussian(H['m'], H['s']));
    }

    void swipe() {
        for (auto z : coos(*this))
            if (double excess = at(z); excess > 0) {
                for (int i = 0; i < 4; ++i) atp(z + dz[i]) += excess / 4;
                at(z) = 0;
            }
        if (H['r']) {
            double m = 0;
            for (auto z : coos(*this))
                if (double mm = abs(at(z)); mm > m) m = mm;
            if (m > 0)
                for (auto z : coos(*this)) at(z) = at(z) * (100 / m);
        }
        update();
    }
};

int main(int argc, char **argv) {
    H.init("Divisible sandpile", argc, argv, "n=500,m=.01,s=10,r");
    Sandpile S(H['n']);
    S.show();
    while (true) { S.swipe(); }
}
