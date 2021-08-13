#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Ising : public Image {
public:
    Ising(const Hub &H, size_t nn, double bb, double rr, bool cc) : Image(H.title, {nn, nn}), n(nn), c(cc), beta(bb) {
        if (rr != 0.0) {
            for (auto z : coo_range(size)) put(z, prng.bernoulli(rr) ? BLACK : WHITE);
        } else {
            for (size_t x = 1; x < n - 1; x++)
                for (size_t y = 1; y < n - 1; y++) put({x, y}, x < (n / 2) ? BLACK : WHITE);
        }
        if (c)
            for (size_t i = 0; i < n; i++) {
                put({i, 0}, i < n / 2 ? BLACK : WHITE);
                put({i, n - 1}, i < n / 2 ? BLACK : WHITE);
                put({0, i}, BLACK);
                put({n - 1, i}, WHITE);
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
        for (size_t i = 0; i < 10; ++i) p[i] = exp(-double(i) * beta);
        if (nstep == 0) nstep = 10 + size_t(double(n) * 0.01 / fabs(beta - log(1 + sqrt(2))));

        for (size_t i = 0; i != nstep; i++)
            for (size_t j = 0; j < n * n; ++j) {
                step();
                const auto z = prng.uniform_coo(size, size_t(c));
                if (k) {
                    coo zz = coo(z) + dz[prng.uniform_int(4U)];
                    if (c && !fits(zz, static_cast<size_t>(c))) continue;
                    if (atp(coo(z)) == atp(zz)) continue;
                    int s = nnb(coo(z)) + nnb(zz) + 2;
                    if ((s <= 0) || (prng.bernoulli(p[size_t(s)]))) { swap(at(z), atp(zz)); }
                } else {
                    int s = nnb(coo(z));
                    if ((s <= 0) || (prng.bernoulli(p[size_t(s)]))) put(z, Grey(uint8_t(255 - int(at(z)))));
                }
            }
    }

    size_t n;
    bool   c = false;
    double beta;
};

auto main(int argc, char *argv[]) -> int {
    Hub   H("Ising model", argc, argv, "b=.88137359,n=300,t=0,c,r=0,k,v");
    Ising I(H, H['n'], H['b'], H['r'], H['c']);
    I.show();
    if (H['v']) I.snapshot_setup("Ising", 10);
    I.run(H['t'], H['k']);
    I.output(H.title);
}
