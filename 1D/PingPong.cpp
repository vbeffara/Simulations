#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using std::cout;
using std::endl;
using std::max;
using vb::Hub;
using vb::prng;

auto main(int argc, char **argv) -> int {
    Hub const    H("PingPong process", argc, argv, "n=1000,a=1,b=.5,f");
    size_t const n = H['n'];
    double a = H['a'], b = H['b'];
    bool const   fast = H['f'];

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
