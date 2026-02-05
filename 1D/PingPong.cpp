#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using std::cout;
using std::endl;
using std::max;
using vb::prng;

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "PingPong process");
    auto    n    = clp.param("n", size_t(1000), "Target distance");
    auto    a    = clp.param("a", 1.0, "A parameter");
    auto    b    = clp.param("b", 0.5, "B parameter");
    auto    fast = clp.flag("f", "Fast mode");
    clp.finalize();

    int             x = 0, xmin = -1, xmax = 1;
    vb::ProgressBar PB(n, fast ? 1 : 3);

    while (size_t(abs(x)) < n) {
        if (fast)
            x = prng.bernoulli(double(x - xmin) / (xmax - xmin)) ? xmax : xmin;
        else
            x = prng.bernoulli(.5) ? x + 1 : x - 1;
        if (x <= xmin) {
            cout << x << endl;
            xmin = x - 1;
            x += int(a * pow(-x, b));
        }
        if (x >= xmax) {
            cout << x << endl;
            xmax = x + 1;
            x -= int(1 * pow(+x, b));
        }
        PB.set(size_t(max(xmax, -xmin)));
    }
}
