#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>
#include <vb/util/misc.h>

using namespace vb;
using namespace std;

std::array<Color, 3> CC = {WHITE, RED, BLACK};
gsl::span<Color>     C{CC};

class TASEP : public vector<int> {
public:
    explicit TASEP(const Hub &H) : vector<int>(size_t(H['n']), 0), p(size(), 1) {
        for (int &e : *this) {
            e = prng.bernoulli(H['r']) ? 2 : 0;
            if (prng.bernoulli(H['d']) && (e == 2)) e = 1;
        }
        for (double &q : p) q = prng.uniform_real(H['e'], 1.0);
    }
    void step() {
        auto i = prng.uniform_int(size());
        if (prng.bernoulli(1 - p[i])) return;
        if (at(i) > at((i + 1) % size())) { std::swap(at(i), at((i + 1) % size())); }
    }

    vector<double> p;
};

auto main(int argc, char **argv) -> int {
    Hub H("TASEP", argc, argv, "n=1280,r=.4,e=0,d=0");

    TASEP T(H);
    Image I(H.title, {T.size(), 700});
    I.show();
    for (size_t t = 0, u = 0;; ++t, ++u) {
        for (size_t j = 0; j < u; ++j)
            for (size_t i = 0; i < T.size(); ++i) T.step();
        for (size_t i = 0; i < T.size(); ++i) I.put({i, t % size_t(I.size.y)}, C[to_unsigned(T[i])]);
        if (t == I.size.y - 1) I.pause();
    }
}
