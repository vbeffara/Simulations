#include <vb/Bitmap.h>
#include <vb/Coloring.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

const Color LEFTSIDE(Indexed(1)), RIGHTSIDE(Indexed(2));

class Loewner : public std::vector<double> {
public:
    Loewner(int64_t n, double k) {
        dt = 1.0 / (n * n);
        reserve(n * n);
        push_back(0);
        for (int i = 1; i < n * n; i++) push_back(back() + sqrt(k * dt) * prng.gaussian());
        bound();
    }

    void bound() {
        Max.operator=(*this);
        Min.operator=(*this);
        for (int64_t i = size() - 2; i >= 0; i--) {
            Min[i] = min(at(i), Min[i + 1]);
            Max[i] = max(at(i), Max[i + 1]);
        }
    }

    Color compute(cpx z) const {
        for (unsigned k = 0; k < size(); ++k) {
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

int main(int argc, char **argv) {
    Hub H("Schramm-Loewner Evolution", argc, argv, "n=300,k=2.666666666667,r=0,a");
    int r = H['r'];
    if (r > 0) prng.seed(r);

    Loewner  L(H['n'], H['k']);
    double   w = L.Max[0] - L.Min[0];
    Coloring C(H.title, cpx(-w, 0), cpx(w, 4.0 * w / 3.0), H['n'], [&L](cpx z) { return L.compute(z); });
    if (H['a']) C.aa = false;
    C.show();
    C.output(H);
}
