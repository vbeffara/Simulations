#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Glauber : public Image {
public:
    Glauber(const Hub &H, size_t n_) : Image(H.title, {n_, n_}), n(n_){};
    void fill(double p);
    void step(ucoo z);

private:
    size_t n;
};

void Glauber::fill(double p) {
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            if (prng.bernoulli(p))
                put({i, j}, WHITE);
            else
                put({i, j}, BLACK);
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

int main(int argc, char **argv) {
    Hub    H("Glauber dynamics at zero temperature", argc, argv, "n=500,p=.51");
    double p = H['p'];

    Glauber G(H, H['n']);
    G.fill(p);
    G.show();

    while (true) G.step(prng.uniform_coo(G.size));
    return 0;
}
