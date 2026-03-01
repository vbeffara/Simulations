#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

namespace vb {
    template <> auto to_Color(double t) -> Color { return HSV(t, 1.0, 1.0); }

    class XY : public Bitmap<double> {
    public:
        double beta;

        XY(size_t n, double b) : Bitmap<double>("The XY model in Z^2", {n, n}), beta(b) {
            for (auto z : coo_range(size)) at(z) = prng.uniform_real();
        }

        void up() { up(prng.uniform_coo(coo(size))); }
        void up(coo z) {
            double oh = 0, nh = 0;
            double const nt = prng.uniform_real();
            for (unsigned ii = 0; ii < 4; ++ii) oh += cos(2.0 * M_PI * (atp(z) - atp(z + dz[ii])));
            for (unsigned ii = 0; ii < 4; ++ii) nh += cos(2.0 * M_PI * (nt - atp(z + dz[ii])));
            if ((nh >= oh) || prng.bernoulli(exp(beta * (nh - oh)))) putp(z, nt);
        }
    };
} // namespace vb

auto main(int argc, char **argv) -> int {
    auto clp = vb::CLP("The XY model in Z^2", argc, argv);
    auto n    = clp.param("n", size_t(500), "Size of the simulation");
    auto b    = clp.param("b", 1.0, "Inverse temperature");
    clp.finalize();
    auto xy   = vb::XY(n, b);
    xy.show();
    while (true) xy.up();
}
