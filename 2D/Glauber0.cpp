#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Glauber : public Image {
public:
    Glauber(const std::string &title, size_t n_) : Image(title, {n_, n_}), n(n_){};
    void fill(double p);
    void step(ucoo z);

private:
    size_t n;
};

void Glauber::fill(double p) {
    // TODO coo_range
    for (size_t ii = 0; ii < n; ++ii)
        for (size_t jj = 0; jj < n; ++jj)
            if (prng.bernoulli(p))
                put({ii, jj}, WHITE);
            else
                put({ii, jj}, BLACK);
}

void Glauber::step(ucoo z) {
    auto cz = coo(z);
    int  c  = 0;
    for (unsigned d = 0; d < 4; ++d)
        if (atp(cz + dz[d]) == WHITE) ++c;

    if ((c > 2) || ((c == 2) && prng.bernoulli(.5)))
        put(z, WHITE);
    else
        put(z, BLACK);
}

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Glauber dynamics at zero temperature");
    auto n = clp.param("n", 500UL, "grid size");
    auto p = clp.param("p", 0.51, "initial probability");
    clp.finalize();

    Glauber G(clp.title, n);
    G.fill(p);
    G.show();

    while (true) G.step(prng.uniform_coo(G.size));
    return 0;
}
