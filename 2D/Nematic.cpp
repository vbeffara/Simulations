#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

namespace vb {
    template <> auto to_Color(size_t t) -> Color {
        static const std::vector<Color> key = {BLACK, RED, GREEN};
        return key[t];
    }
} // namespace vb

class Nematic : public vb::Bitmap<size_t> {
public:
    Nematic(const std::string &title, size_t n, size_t m, size_t k_, double b_) : Bitmap<size_t>(title, {n, m}), k(k_), b(b_), P(std::max(n, m), 0){};

    void prec() {
        ok = std::min(k, std::min(size.x, size.y));
        ob = b;
        std::vector<double> Z(P.size());
        double const        zz = exp(2 * ob / double(ok));
        for (size_t ii = 0; ii < ok; ++ii) Z[ii] = pow(zz, -double(ii));
        for (size_t ii = ok; ii < Z.size(); ++ii) Z[ii] = Z[ii - 1] / zz + Z[ii - ok];
        for (size_t ii = ok; ii < P.size(); ++ii) P[ii] = Z[ii - ok] / Z[ii];
        dd = double(ok) * Z[Z.size() - ok] / (Z[Z.size() - 1] / zz + double(ok) * Z[Z.size() - ok]);
    }

    void add(coo z, size_t d) {
        for (size_t ii = 0; ii < ok; ++ii) {
            atp(z) = d;
            z += dz[d - 1];
        }
    }

    void fill(coo z, size_t d, size_t l) {
        while (l >= ok) {
            if (prng.bernoulli(P[l])) {
                add(z, d);
                z += dz[d - 1] * int(ok);
                l -= ok;
            } else {
                z += dz[d - 1];
                l--;
            }
        }
    }

    void redo(coo z, size_t d) {
        bool   empty = true;
        size_t const hw    = (d == 1 ? size.x : size.y);
        for (size_t x = 0; x < hw; z += dz[d - 1], ++x) {
            if (at(ucoo(z)) == 3 - d) empty = false;
            if (at(ucoo(z)) == d) at(ucoo(z)) = 0;
        }
        if (empty) {
            if (prng.bernoulli(dd)) {
                z -= dz[d - 1] * int(prng.uniform_int(ok));
                add(z, d);
                fill(z + dz[d - 1] * int(ok), d, hw - ok);
            } else
                fill(z + dz[d - 1], d, hw - 1);
        } else {
            while (atp(z) != 3 - d) { z += dz[d - 1]; }
            z += dz[d - 1];
            for (coo zz = z - dz[d - 1] * int(hw); zz != z; zz += dz[d - 1]) {
                size_t l = 0;
                while (atp(zz) == 0) {
                    ++l;
                    zz += dz[d - 1];
                }
                if (l > 0) fill(zz - dz[d - 1] * int(l), d, l);
            }
        }
        step();
    }

    void go(bool v, int steps, bool l) {
        Console C;
        C.watch(order, "order");
        C.manage(b, -5.0, 10.0, "beta");
        C.manage(k, size_t(1), size_t(100), "k");
        show();
        C.show();
        if (v) snapshot_setup("movie", 10);
        for (int t = steps - 1; t != 0; --t) {
            if ((k != ok) || (b != ob)) prec();
            for (size_t ii = 0; ii < size.y; ++ii) redo({0, int64_t(ii)}, 1);
            for (size_t ii = 0; ii < size.x; ++ii) redo({int64_t(ii), 0}, 2);
            int nh = 0;
            for (auto z : coo_range(size))
                if (at(z) == 1) ++nh;
            int nv = 0;
            for (auto z : coo_range(size))
                if (at(z) == 2) ++nv;
            order = nh + nv > 0 ? double(nh - nv) / double(nh + nv) : 0;
            if (((t % 100) == 0) && l) std::cout << order << std::endl;
        }
    };

    size_t              k, ok = 0;
    double              b, ob = 0, order = 0, dd = 0;
    std::vector<double> P;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Nematic system on the square lattice");
    auto n = clp.param("n", 500, "grid width");
    auto m = clp.param("m", 0, "grid height (0 = same as width)");
    auto k = clp.param("k", 7, "segment length");
    auto b = clp.param("b", 2.0, "inverse temperature beta");
    auto v = clp.flag("v", "enable video snapshots");
    auto l = clp.flag("l", "log order parameter");
    auto t = clp.param("t", 0, "number of steps");
    clp.finalize();

    size_t height = (m != 0 ? size_t(m) : size_t(n));
    Nematic(clp.title, n, height, k, b).go(v, t, l);
}
