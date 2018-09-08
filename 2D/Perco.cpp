#include <vb/Image.h>

using namespace vb;
using namespace std;

const Color IN{128, 128, 128};

class Perco : public Image {
public:
    Perco(int n, double p) : Image(n, n), mid({n / 2, n / 2}), p(p) {
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                if ((x % 2 == 0) && (y % 2 == 0))
                    put({x, y}, IN);
                else if ((x % 2 == 0) || (y % 2 == 0))
                    put({x, y}, prng.bernoulli(p) ? IN : BLACK);
            }
        }
        fill(mid, RED);
        show();
    }

    void run() {
        int  s = H['s'], t = 0;
        bool c = H['c'];

        while (true) {
            if ((s > 0) && ((t++ % s) == 0)) snapshot();
            auto z = rand();
            if ((z.x + z.y) % 2 == 0) continue;
            fill(mid, IN);
            if (c)
                swap(at(z), atp(z + dz[4 + prng.uniform_int(4)]));
            else
                at(z) = prng.bernoulli(p) ? IN : BLACK;
            fill(mid, RED);
            step();
        }
    }

    coo    mid;
    double p;
};

int main(int argc, char ** argv) {
    H.init("Percolation", argc, argv, "n=500,p=.5,c,d,s=0");
    Perco P(H['n'], H['p']);
    if (H['d'])
        P.run();
    else
        P.pause();
}
