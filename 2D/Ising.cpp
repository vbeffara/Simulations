#include <vb/Bitmap.h>
#include <vb/Ranges.h>

using namespace vb;
using namespace std;

class Ising : public Image {
public:
    Ising(int nn, double bb, double rr, bool cc) : Image(nn, nn), n(nn), c(cc), beta(bb) {
        if (rr != 0.0) {
            for (auto z : coos(*this)) put(z, prng.bernoulli(rr) ? BLACK : WHITE);
        } else {
            for (int x = 1; x < n - 1; x++)
                for (int y = 1; y < n - 1; y++) put({x, y}, x < (n / 2) ? BLACK : WHITE);
        }
        if (c)
            for (int i = 0; i < n; i++) {
                put({i, 0}, i < n / 2 ? BLACK : WHITE);
                put({i, n - 1}, i < n / 2 ? BLACK : WHITE);
                put({0, i}, BLACK);
                put({n - 1, i}, WHITE);
            }
    }

    int nnb(coo z) {
        int   out = 0;
        Color c   = atp(z);
        for (int d = 0; d < 4; ++d) {
            if (atp(z + dz[d]) == c)
                out++;
            else
                out--;
        }
        return out;
    }

    void run(int nstep, bool k) {
        vector<double> p(10, 0);
        for (int i = 0; i < 10; ++i) p[i] = exp(-i * beta);
        if (nstep == 0) nstep = 10 + int(n * 0.01 / fabs(beta - log(1 + sqrt(2))));

        for (int i = 0; i != nstep; i++)
            for (int j = 0; j < n * n; ++j) {
                step();
                coo z = rand(static_cast<int64_t>(c));
                if (k) {
                    coo zz = z + dz[prng.uniform_int(4)];
                    if (c && !contains(zz, static_cast<int64_t>(c))) continue;
                    if (atp(z) == atp(zz)) continue;
                    int s = nnb(z) + nnb(zz) + 2;
                    if ((s <= 0) || (prng.bernoulli(p[s]))) { swap(atp(z), atp(zz)); }
                } else {
                    int s = nnb(z);
                    if ((s <= 0) || (prng.bernoulli(p[s]))) put(z, Grey(255 - int(at(z))));
                }
            }
    }

    int    n;
    bool   c = false;
    double beta;
};

int main(int argc, char *argv[]) {
    H.init("Ising model", argc, argv, "b=.88137359,n=300,t=0,c,r=0,k,v");
    Ising I(H['n'], H['b'], H['r'], H['c']);
    I.show();
    if (H['v']) I.snapshot_setup("Ising", 10);
    I.run(H['t'], H['k']);
    I.output();
}
