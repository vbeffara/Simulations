#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

namespace vb {
    template <> auto to_Color(size_t t) -> Color { return t == 0 ? BLACK : Indexed(int(t)); }
} // namespace vb

class ACP : public Bitmap<size_t> {
public:
    ACP(const string &title, size_t n, double d, double a, size_t z, size_t m, double b, double r)
        : Bitmap<size_t>(title, {n, n}), kid(z), maxage(m) {
        put(size / 2, 1);
        P = {d, a};
        for (size_t ii = 0; ii < m - kid; ++ii) P.push_back(b + r * double(ii));
        double s = 0;
        for (double const u : P) s += u;
        for (double &u : P) u /= s;
    }
    void run() {
        auto z = prng.uniform_coo(size);
        if (at(z) == 0) return;
        auto action = prng.discrete(P);
        if (action == 0) {
            put(z, 0);
        } else if ((action == 1) && (at(z) < maxage)) {
            put(z, at(z) + 1);
        } else if (action + kid - 1 <= at(z)) {
            coo const nz = coo(z) + dz[prng.uniform_int(4U)];
            if (atp(nz) == 0) putp(nz, 1);
        }
    }
    size_t         kid, maxage;
    vector<double> P;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Contact process with aging");
    auto n = clp.param("n", size_t(400), "Grid size");
    auto d = clp.param("d", 0.01, "Death probability");
    auto a = clp.param("a", 0.1, "Aging probability");
    auto z = clp.param("z", size_t(0), "Kid threshold");
    auto m = clp.param("m", size_t(5), "Max age");
    auto b = clp.param("b", 0.2, "Birth base rate");
    auto r = clp.param("r", 0.1, "Birth rate increment");
    clp.finalize();
    ACP A(clp.title, n, d, a, z, m, b, r);
    A.show();
    while (true) A.run();
}
