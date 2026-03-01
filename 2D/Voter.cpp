#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Voter : public Image {
public:
    Voter(const std::string &title, size_t n, double p, int d) : Image(title, {n, n}) {
        for (auto z : coo_range(size))
            if (d > 1)
                put(z, Indexed(prng.uniform_int(d)));
            else
                put(z, prng.bernoulli(p) ? WHITE : BLACK);
    }

    void up() {
        auto z = prng.uniform_coo(size);
        put(z, atp(coo(z) + dz[prng.uniform_int(4U)]));
    }
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Voter model");
    auto n = clp.param("n", size_t(500), "Grid size");
    auto p = clp.param("p", 0.5, "Initial probability");
    auto d = clp.param("d", 1, "Number of colors");
    clp.finalize();

    Voter V(clp.title, n, p, d);

    V.show();
    while (true) V.up();
}
