#include <vb/Bitmap.h>

using namespace vb;
using namespace std;

std::array<Color, 3> CC = {WHITE, RED, BLACK};
gsl::span<Color>     C{CC};

class TASEP : public vector<int> {
public:
    explicit TASEP(const Hub &H) : vector<int>(int(H['n']), 0), p(size(), 1) {
        for (int &e : *this) {
            e = prng.bernoulli(H['r']) ? 2 : 0;
            if (prng.bernoulli(H['d']) && (e == 2)) e = 1;
        }
        for (double &q : p) q = prng.uniform_real(H['e'], 1.0);
    }
    void step() {
        int i = prng.uniform_int(size());
        if (prng.bernoulli(1 - p[i])) return;
        if (at(i) > at((i + 1) % size())) { std::swap(at(i), at((i + 1) % size())); }
    }

    vector<double> p;
};

int main(int argc, char **argv) {
    Hub H("TASEP", argc, argv, "n=1280,r=.4,e=0,d=0");

    TASEP T(H);
    Image I(H, T.size(), 700);
    I.show();
    for (int t = 0, u = 0;; ++t, ++u) {
        for (int j = 0; j < u; ++j)
            for (unsigned i = 0; i < T.size(); ++i) T.step();
        for (unsigned i = 0; i < T.size(); ++i) I.put({i, t % I.h()}, C[T[i]]);
        if (t == I.h() - 1) I.pause();
    }
}
