#include <vb/Hub.h>
#include <vb/Path.h>
#include <vb/ProgressBar.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

int main(int argc, char **argv) {
    H.init("Self-avoiding walk", argc, argv, "n=500,t=5000");
    int n = H['n'];
    int t = H['t'];

    OldPath     P(n, true);
    ProgressBar PB(t);

    for (int i = 0; i < t;) {
        PB.set(i);
        int j = prng() % n;
        int k = 1 + (prng() % 3);
        int l = P[j];
        P[j]  = (P[j] + k) % 4;
        if (P.self_avoiding())
            i++;
        else
            P[j] = l;
    }

    P.output();
    return 0;
}
