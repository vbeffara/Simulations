#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

namespace vb {
    template <> Color to_Color(size_t t) {
        static const std::vector<Color> key = {BLACK, RED, GREEN};
        return key[t];
    }
} // namespace vb

class Nematic : public vb::Bitmap<size_t> {
public:
    Nematic(const Hub &H, size_t n, size_t m, size_t k, double b) : Bitmap<size_t>(H.title, {n, m}), k(k), b(b), P(std::max(n, m), 0){};

    void prec() {
        // TODO: make w() and h() return unsigned values
        ok = std::min(k, size_t(std::min(w(), h())));
        ob = b;
        std::vector<double> Z(P.size());
        double              zz = exp(2 * ob / double(ok));
        for (unsigned i = 0; i < ok; ++i) Z[i] = pow(zz, -i);
        for (auto i = ok; i < Z.size(); ++i) Z[i] = Z[i - 1] / zz + Z[i - ok];
        for (auto i = ok; i < P.size(); ++i) P[i] = Z[i - ok] / Z[i];
        dd = ok * Z[Z.size() - ok] / (Z[Z.size() - 1] / zz + ok * Z[Z.size() - ok]);
    }

    void add(coo z, size_t d) {
        for (size_t i = 0; i < ok; ++i) {
            atp(z) = d;
            z += dz[gsl::index(d - 1)];
        }
    }

    void fill(coo z, size_t d, size_t l) {
        while (l >= ok) {
            if (prng.bernoulli(P[l])) {
                add(z, d);
                // TODO: somehow get rid of gsl::index
                z += dz[gsl::index(d - 1)] * int(ok);
                l -= ok;
            } else {
                z += dz[gsl::index(d - 1)];
                l--;
            }
        }
    }

    void redo(coo z, size_t d) {
        bool   empty = true;
        size_t hw    = (d == 1 ? size_t(w()) : size_t(h()));
        for (size_t x = 0; x < hw; z += dz[gsl::index(d - 1)], ++x) {
            if (at(z) == 3 - d) empty = false;
            if (at(z) == d) at(z) = 0;
        }
        if (empty) {
            if (prng.bernoulli(dd)) {
                z -= dz[gsl::index(d - 1)] * int(prng.uniform_int(ok));
                add(z, d);
                fill(z + dz[gsl::index(d - 1)] * int(ok), d, hw - ok);
            } else
                fill(z + dz[gsl::index(d - 1)], d, hw - 1);
        } else {
            while (atp(z) != 3 - d) { z += dz[gsl::index(d - 1)]; }
            z += dz[gsl::index(d - 1)];
            for (coo zz = z - dz[gsl::index(d - 1)] * int(hw); zz != z; zz += dz[gsl::index(d - 1)]) {
                size_t l = 0;
                while (atp(zz) == 0) {
                    ++l;
                    zz += dz[gsl::index(d - 1)];
                }
                if (l > 0) fill(zz - dz[gsl::index(d - 1)] * int(l), d, l);
            }
        }
        step();
    }

    void go(const Hub &H) {
        Console C;
        C.watch(order, "order");
        C.manage(b, -5.0, 10.0, "beta");
        C.manage(k, size_t(1), size_t(100), "k");
        show();
        C.show();
        if (H['v']) snapshot_setup("movie", 10);
        for (int t = int(H['t']) - 1; t != 0; --t) {
            if ((k != ok) || (b != ob)) prec();
            for (int i = 0; i < h(); ++i) redo({0, i}, 1);
            for (int i = 0; i < w(); ++i) redo({i, 0}, 2);
            int nh = 0;
            for (auto z : coo_range(size))
                if (at(z) == 1) ++nh;
            int nv = 0;
            for (auto z : coo_range(size))
                if (at(z) == 2) ++nv;
            order = nh + nv > 0 ? double(nh - nv) / double(nh + nv) : 0;
            if (((t % 100) == 0) && H['l']) std::cout << order << std::endl;
        }
    };

    size_t              k, ok = 0;
    double              b, ob = 0, order = 0, dd = 0;
    std::vector<double> P;
};

int main(int argc, char **argv) {
    Hub H("Nematic system on the square lattice", argc, argv, "n=500,m=0,k=7,b=2,v,l,t=0");
    Nematic(H, H['n'], int(H['m']) != 0 ? H['m'] : H['n'], H['k'], H['b']).go(H);
}
