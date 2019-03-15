#include <vb/Path.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Self-avoiding walk", argc, argv, "n=500,t=5000");
    size_t n = H['n'], t = H['t'];

    OldPath     P(n, true);
    ProgressBar PB(t);

    for (size_t i = 0; i < t;) {
        PB.set(i);
        auto j = prng.uniform_int(n);
        auto k = uint8_t(1) + prng.uniform_int(uint8_t(3));
        auto l = P[j];
        P[j]   = (P[j] + k) % 4;
        if (P.self_avoiding())
            i++;
        else
            P[j] = l;
    }

    P.output(H.title);
}
