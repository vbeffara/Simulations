#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Voter : public Image {
public:
    Voter(const Hub &H, size_t n, double p, int d) : Image(H.title, {n, n}) {
        for (auto z : coo_range(size))
            if (d > 1)
                put(z, Indexed(prng.uniform_int(d)));
            else
                put(z, prng.bernoulli(p) ? WHITE : BLACK);
    }

    void up() {
        auto z = prng.uniform_coo(size);
        put(z, atp(coo(z) + dz[prng.uniform_int(4u)]));
    }
};

auto main(int argc, char **argv) -> int {
    Hub H("Voter model", argc, argv, "n=500,p=.5,d=1");

    Voter V(H, H['n'], H['p'], H['d']);

    V.show();
    while (true) V.up();
}
