#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

const Color none(BLACK), prey(GREEN), pred(RED);

class SIR : public Image {
public:
    SIR(const Hub &H, size_t n, double l_) : Image(H.title, {n, n}), l(l_) {
        int n0 = H['d'] ? 0 : int(H['n']) / 2;
        for (int i = n0 - 10; i < n0 + 10; ++i)
            for (int j = n0 - 10; j < n0 + 10; ++j)
                if (contains({i, j})) put({i, j}, prey);
        put({n0, n0}, pred);
        for (auto z : coo_range(size))
            if (at(z) == prey) fringe.push_back(z);
        show();
    }

    void go(const Hub &H) {
        auto i  = prng.uniform_int(fringe.size());
        coo  z  = fringe[i];
        coo  nz = z + dz[prng.uniform_int(H['d'] ? 2 : 4)];
        if (!contains(nz)) return;
        if ((at(nz) == none) && ((l > 1) || prng.bernoulli(l))) {
            put(nz, prey);
            fringe.push_back(nz);
        }
        if ((at(nz) == pred) && ((l < 1) || prng.bernoulli(1 / l))) {
            put(z, pred);
            fringe[i] = fringe.back();
            fringe.pop_back();
        }
    }

    double           l;
    std::vector<coo> fringe;
};

int main(int argc, char **argv) {
    Hub H("SIR process on the lattice", argc, argv, "n=600,l=.5,d,s=1");

    SIR img(H, H['n'], H['l']);
    for (int t = 0; !img.fringe.empty(); ++t) {
        img.go(H);
        if (t % int(H['s']) == 1) img.update();
    }
    img.output(H.title);
}
