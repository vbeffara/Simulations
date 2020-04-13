#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto rot(coo z, coo p) -> coo { return p + coo{p.y - z.y, z.x - p.x}; }

class Pivot : public Image {
public:
    Pivot(const string &title, size_t nn_) : Image(title, {4 * nn_, 4 * nn_}), nn(nn_), n(nn * nn), zs(n, {0, 0}) { show(); }

    auto piv() -> size_t {
        for (auto z : coo_range(size)) put(z, BLACK);
        for (size_t k = 0; k < n / 4; k++) putp(zs[k], WHITE);
        for (size_t k = n / 4; k < 3 * n / 4; k++) {
            bool piv = true;
            for (size_t l = k; l < n; l++) {
                if ((atp(zs[l]) == WHITE) || (atp(rot(zs[l], zs[k])) == WHITE)) {
                    piv = false;
                    break;
                }
            }
            if (piv) return k;
            putp(zs[k], WHITE);
        }
        return 0;
    }

    void run() {
        coo    center{2 * int64_t(nn), 2 * int64_t(nn)};
        size_t p = 0;
        while (p == 0) {
            zs[0] = center;
            for (size_t k = 1; k < n; k++) zs[k] = zs[k - 1] + dz[prng.uniform_int(4)];
            p = piv();
        }

        spdlog::info("Pivot found at time {} and location {}.", p, zs[p]);

        for (auto z : coo_range(size)) put(z, BLACK);
        for (size_t k = 0; k < n; k++) putp(zs[k] - zs[p] + center, WHITE);
        for (size_t k = p + 1; k < n; k++) {
            coo z = rot(zs[k] - zs[p], {0, 0}) + center;
            if (atp(z) != WHITE) putp(z, RED);
        }
    }

    size_t      nn, n;
    vector<coo> zs;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Pivot probabiilties for 2D random walk");
    auto n = clp.param("n", size_t(200), "Simulation size");
    clp.finalize();

    Pivot img(clp.title, n);
    img.run();
    img.pause();
}
