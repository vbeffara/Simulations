#include <vb/Image.h>
#include <vb/Ranges.h>

using namespace vb;

class Voter : public Image {
public:
    Voter(int n, double p, int d) : Image(n, n) {
        for (auto z : coos(*this))
            if (d > 1)
                put(z, Indexed(prng.uniform_int(d)));
            else
                put(z, prng.bernoulli(p) ? WHITE : BLACK);
    }

    void up() {
        coo z = rand();
        put(z, atp(z + dz[prng.uniform_int(4)]));
    }
};

int main(int argc, char ** argv) {
    H.init("Voter model", argc, argv, "n=500,p=.5,d=1");

    Voter V(H['n'], H['p'], H['d']);

    V.show();
    while (true) V.up();
}
