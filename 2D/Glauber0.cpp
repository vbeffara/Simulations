#include <vb/Bitmap.h>
#include <vb/util/PRNG.h>

using namespace vb;

class Glauber : public Image {
public:
    Glauber(const Hub &H, int n_) : Image(H.title, {n_, n_}), n(n_){};
    void fill(double p);
    void step(int i = -1, int j = -1);

private:
    int n;
};

void Glauber::fill(double p) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (prng.bernoulli(p))
                put({i, j}, WHITE);
            else
                put({i, j}, BLACK);
}

void Glauber::step(int i, int j) {
    if (i == -1) i = prng.uniform_int(n);
    if (j == -1) j = prng.uniform_int(n);

    int c = 0;
    if (at({(i + 1) % n, j}) == WHITE) ++c;
    if (at({(i + n - 1) % n, j}) == WHITE) ++c;
    if (at({i, (j + 1) % n}) == WHITE) ++c;
    if (at({i, (j + n - 1) % n}) == WHITE) ++c;

    if ((c > 2) || ((c == 2) && prng.bernoulli(.5)))
        put({i, j}, WHITE);
    else
        put({i, j}, BLACK);
}

int main(int argc, char **argv) {
    Hub    H("Glauber dynamics at zero temperature", argc, argv, "n=500,p=.51");
    int    n = H['n'];
    double p = H['p'];

    Glauber G(H, n);
    G.fill(p);
    G.show();

    while (true) G.step();
    return 0;
}
