#include <vb/Bitmap.h>
#include <vb/Coloring.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

const Color LEFTSIDE(Indexed(1)), RIGHTSIDE(Indexed(2));

class Loewner : public std::vector<double> {
public:
    Loewner(size_t n, double k) : dt(1.0 / double(n * n)) {
        reserve(n * n);
        push_back(0);
        for (size_t i = 1; i < n * n; i++) push_back(back() + sqrt(k * dt) * prng.gaussian());
        bound();
    }

    void bound() {
        Max.operator=(*this);
        Min.operator=(*this);
        for (size_t i = size() - 1; i-- > 0;) {
            Min[i] = min(at(i), Min[i + 1]);
            Max[i] = max(at(i), Max[i + 1]);
        }
    }

    [[nodiscard]] auto compute(cpx z) const -> Color {
        for (size_t k = 0; k < size(); ++k) {
            if (z.imag() <= 0) return (z.real() < at(k)) ? LEFTSIDE : RIGHTSIDE;
            if (z.real() < Min[k]) return LEFTSIDE;
            if (z.real() > Max[k]) return RIGHTSIDE;
            z += 2 * dt / (z - at(k));
        }
        return (z.real() < back()) ? LEFTSIDE : RIGHTSIDE;
    }

    double              dt = 1;
    std::vector<double> Min, Max;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Schramm-Loewner Evolution");
    auto n = clp.param("n", size_t(300), "Grid size");
    auto k = clp.param("k", 2.666666666667, "Kappa parameter");
    auto r = clp.param("r", size_t(0), "Random seed");
    auto a = clp.flag("a", "Disable antialiasing");
    clp.finalize();

    if (r > 0) prng.seed(r);

    Loewner  L(n, k);
    double const w = L.Max[0] - L.Min[0];
    Coloring C(clp.title, cpx(-w, 0), cpx(w, 4.0 * w / 3.0), n, [&L](cpx z) { return L.compute(z); });
    if (a) C.antialias = false;
    C.show();
    C.output(clp.title);
}
