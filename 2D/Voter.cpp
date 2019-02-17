#include <vb/Bitmap.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Voter : public Image {
public:
    Voter(const Hub &H, int n, double p, int d) : Image(H.title, {n, n}) {
        for (auto z : coo_range(size))
            if (d > 1)
                put(z, Indexed(prng.uniform_int(d)));
            else
                put(z, prng.bernoulli(p) ? WHITE : BLACK);
    }

    void up() {
        coo z = prng.uniform_coo(size);
        put(z, atp(z + dz[prng.uniform_int(4)]));
    }
};

int main(int argc, char **argv) {
    Hub H("Voter model", argc, argv, "n=500,p=.5,d=1");

    Voter V(H, H['n'], H['p'], H['d']);

    V.show();
    while (true) V.up();
}
