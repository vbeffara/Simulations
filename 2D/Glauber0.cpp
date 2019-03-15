#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Glauber : public Image {
public:
    Glauber(const Hub &H, size_t n_) : Image(H.title, {n_, n_}), n(n_){};
    void fill(double p);
    void step(int i = -1, int j = -1);

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

void Glauber::step(int i, int j) {
    if (i == -1) i = prng.uniform_int(int(n));
    if (j == -1) j = prng.uniform_int(int(n));

    int c = 0;
    if (at(coo{(i + 1) % int(n), j}) == WHITE) ++c;
    if (at(coo{(i + int(n) - 1) % int(n), j}) == WHITE) ++c;
    if (at(coo{i, (j + 1) % int(n)}) == WHITE) ++c;
    if (at(coo{i, (j + int(n) - 1) % int(n)}) == WHITE) ++c;

    if ((c > 2) || ((c == 2) && prng.bernoulli(.5)))
        put(coo{i, j}, WHITE);
    else
        put(coo{i, j}, BLACK);
}

int main(int argc, char **argv) {
    Hub    H("Glauber dynamics at zero temperature", argc, argv, "n=500,p=.51");
    double p = H['p'];

    Glauber G(H, H['n']);
    G.fill(p);
    G.show();

    while (true) G.step();
    return 0;
}
