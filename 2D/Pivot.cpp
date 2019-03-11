#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

coo rot(coo z, coo p) { return p + coo{p.y - z.y, z.x - p.x}; }

class Pivot : public Image {
public:
    Pivot(const Hub &H, unsigned nn_) : Image(H.title, {4 * nn_, 4 * nn_}), nn(nn_), n(nn * nn), z(n, {0, 0}) { show(); }

    unsigned piv() {
        for (auto z : coo_range(size)) put(z, BLACK);
        for (unsigned k = 0; k < n / 4; k++) putp(z[k], WHITE);
        for (unsigned k = n / 4; k < 3 * n / 4; k++) {
            bool piv = true;
            for (unsigned l = k; l < n; l++) {
                if ((atp(z[l]) == WHITE) || (atp(rot(z[l], z[k])) == WHITE)) {
                    piv = false;
                    break;
                }
            }
            if (piv) return k;
            putp(z[k], WHITE);
        }
        return 0;
    }

    void run() {
        unsigned p = 0;
        while (p == 0) {
            z[0] = {2 * nn, 2 * nn};
            for (unsigned k = 1; k < n; k++) z[k] = z[k - 1] + dz[prng.uniform_int(4)];
            p = piv();
        }

        spdlog::info("Pivot found at time {} and location {}.", p, z[p]);

        for (auto z : coo_range(size)) put(z, BLACK);
        for (unsigned k = 0; k < n; k++) putp(z[k] - z[p] + coo{2 * nn, 2 * nn}, WHITE);
        for (unsigned k = p + 1; k < n; k++) {
            coo zz = rot(z[k] - z[p], {0, 0}) + coo{2 * nn, 2 * nn};
            if (atp(zz) != WHITE) putp(zz, RED);
        }
    }

    unsigned    nn, n;
    vector<coo> z;
};

int main(int argc, char **argv) {
    Hub   H("Pivot", argc, argv, "n=200");
    Pivot img(H, H['n']);
    img.run();
    img.pause();
}
