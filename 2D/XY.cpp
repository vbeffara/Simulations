static const auto usage = R"(
The XY model on Z^2

Usage: XY [options]

Options:
  -n <size>   Size of the simulation [default: 500]
  -b <beta>   Inverse temperature [default: 1.0]
  -h, --help  Print usage help and exit
  
The boundary conditions are periodic, and for now the dynamics is just a
continuous-time Glauber process with uniform updates.)";

#include <docopt/docopt.h>
#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
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
            double nt = prng.uniform_real();
            for (unsigned i = 0; i < 4; ++i) oh += cos(2.0 * M_PI * (atp(z) - atp(z + dz[i])));
            for (unsigned i = 0; i < 4; ++i) nh += cos(2.0 * M_PI * (nt - atp(z + dz[i])));
            if ((nh >= oh) || prng.bernoulli(exp(beta * (nh - oh)))) putp(z, nt);
        }
    };
} // namespace vb

auto main(int argc, char **argv) -> int {
    auto args = docopt::docopt(usage, {argv + 1, argv + argc}, true);
    auto n    = std::stoul(args["-n"].asString());
    auto b    = std::stod(args["-b"].asString());
    auto xy   = vb::XY(n, b);
    xy.show();
    while (true) xy.up();
}
