#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Ising : public Image {
public:
    Ising(const string &title, size_t nn, double bb, double rr, bool cc) : Image(title, {nn, nn}), n(nn), c(cc), beta(bb) {
        if (rr != 0.0) {
            for (auto z : coo_range(size)) put(z, prng.bernoulli(rr) ? BLACK : WHITE);
        } else {
            for (size_t x = 1; x < n - 1; x++)
                for (size_t y = 1; y < n - 1; y++) put({x, y}, x < (n / 2) ? BLACK : WHITE);
        }
        if (c)
            for (size_t ii = 0; ii < n; ii++) {
                put({ii, 0}, ii < n / 2 ? BLACK : WHITE);
                put({ii, n - 1}, ii < n / 2 ? BLACK : WHITE);
                put({0, ii}, BLACK);
                put({n - 1, ii}, WHITE);
            }
    }

    auto nnb(coo z) -> int {
        int  out   = 0;
        auto state = atp(z);
        for (unsigned d = 0; d < 4; ++d) {
            if (atp(z + dz[d]) == state)
                out++;
            else
                out--;
        }
        return out;
    }

    void run(size_t nstep, bool k) {
        vector<double> p(10, 0);
        for (size_t ii = 0; ii < 10; ++ii) p[ii] = exp(-double(ii) * beta);
        if (nstep == 0) nstep = 10 + size_t(double(n) * 0.01 / fabs(beta - log(1 + sqrt(2))));

        for (size_t ii = 0; ii != nstep; ii++)
            for (size_t j = 0; j < n * n; ++j) {
                step();
                const auto z = prng.uniform_coo(size, size_t(c));
                if (k) {
                    coo const zz = coo(z) + dz[prng.uniform_int(4U)];
                    if (c && !fits(zz, static_cast<size_t>(c))) continue;
                    if (atp(coo(z)) == atp(zz)) continue;
                    int const s = nnb(coo(z)) + nnb(zz) + 2;
                    if ((s <= 0) || (prng.bernoulli(p[size_t(s)]))) { swap(at(z), atp(zz)); }
                } else {
                    int const s = nnb(coo(z));
                    if ((s <= 0) || (prng.bernoulli(p[size_t(s)]))) put(z, Grey(uint8_t(255 - int(at(z)))));
                }
            }
    }

    size_t n;
    bool   c = false;
    double beta;
};

auto main(int argc, char *argv[]) -> int {
    CLP  clp(argc, argv, "Ising model");
    auto n = clp.param("n", 300, "grid size");
    auto b = clp.param("b", .88137359, "inverse temperature beta");
    auto t = clp.param("t", 0, "number of steps");
    auto r = clp.param("r", 0.0, "random initial density");
    auto c = clp.flag("c", "Dobrushin boundary conditions");
    auto k = clp.flag("k", "Kawasaki dynamics");
    auto v = clp.flag("v", "enable video snapshots");
    clp.finalize();

    Ising I(clp.title, n, b, r, c);
    I.show();
    if (v) I.snapshot_setup("Ising", 10);
    I.run(t, k);
    I.output(clp.title);
}
