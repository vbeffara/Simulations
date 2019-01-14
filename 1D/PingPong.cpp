#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using std::cout;
using std::endl;
using std::max;
using vb::H;
using vb::prng;

int main(int argc, char **argv) {
    H.init("PingPong process", argc, argv, "n=1000,a=1,b=.5,f");
    int    n = H['n'];
    double a = H['a'], b = H['b'];
    bool   fast = H['f'];

    int             x = 0, xmin = -1, xmax = 1;
    vb::ProgressBar PB(n, fast ? 1 : 3);

    while (abs(x) < n) {
        if (fast)
            x = prng.bernoulli(double(x - xmin) / (xmax - xmin)) ? xmax : xmin;
        else
            x = prng.bernoulli(.5) ? x + 1 : x - 1;
        if (x <= xmin) {
            cout << x << endl;
            xmin = x - 1;
            x += int64_t(a * pow(-x, b));
        }
        if (x >= xmax) {
            cout << x << endl;
            xmax = x + 1;
            x -= int64_t(1 * pow(+x, b));
        }
        PB.set(max(xmax, -xmin));
    }
}
