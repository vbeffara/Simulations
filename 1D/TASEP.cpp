#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/misc.h>

using namespace vb;
using namespace std;

std::array<Color, 3> CC = {WHITE, RED, BLACK};
gsl::span<Color>     C{CC};

class TASEP : public vector<int> {
public:
    TASEP(size_t n, double r, double d, double e) : vector<int>(n, 0), p(n, 1) {
        for (int &elem : *this) {
            elem = prng.bernoulli(r) ? 2 : 0;
            if (prng.bernoulli(d) && (elem == 2)) elem = 1;
        }
        for (double &q : p) q = prng.uniform_real(e, 1.0);
    }
    void step() {
        auto i = prng.uniform_int(size());
        if (prng.bernoulli(1 - p[i])) return;
        if (at(i) > at((i + 1) % size())) { std::swap(at(i), at((i + 1) % size())); }
    }

    vector<double> p;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "TASEP");
    auto n = clp.param("n", size_t(1280), "System size");
    auto r = clp.param("r", 0.4, "Initial occupation probability");
    auto e = clp.param("e", 0.0, "Disorder parameter");
    auto d = clp.param("d", 0.0, "Defect parameter");
    clp.finalize();

    TASEP T(n, r, d, e);
    Image I(clp.title, {T.size(), 700});
    I.show();
    for (size_t t = 0, u = 0;; ++t, ++u) {
        for (size_t j = 0; j < u; ++j)
            for (size_t i = 0; i < T.size(); ++i) T.step();
        for (size_t i = 0; i < T.size(); ++i) I.put({i, t % size_t(I.size.y)}, C[to_unsigned(T[i])]);
        if (t == I.size.y - 1) I.pause();
    }
}
