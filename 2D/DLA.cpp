#include <spdlog/spdlog.h>
#include <vb/CoarseImage.h>
#include <vb/Console.h>
#include <vb/QuadTree.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto harmonic_measures(size_t p) {
    spdlog::info("Precomputing harmonic measures, d up to {} ...", p);
    vector<vector<double>> prec(p);
    for (size_t r = 1; r < p; ++r) {
        Array<double> MM({2 * r + 1, 2 * r + 1});
        MM.at({r, r}) = 1;
        for (bool dirty = true; dirty;) {
            dirty = false;
            for (const auto &z : coo_range(ucoo{1, 1}, ucoo{2 * r, 2 * r})) {
                if (double t = MM.at(z); t > 1e-13) {
                    MM.at(z) = 0;
                    dirty    = true;
                    auto d   = min({z.x, 2 * r - z.x, z.y, 2 * r - z.y});
                    if ((d == 1) || (d == r))
                        for (unsigned k = 0; k < 4; ++k) MM.at(z + dz[k]) += t / 4;
                    else {
                        for (size_t k = 0; k < d; ++k) {
                            MM.at(z + coo{-int64_t(d), +int64_t(k)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{-int64_t(d), -int64_t(k)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{+int64_t(d), +int64_t(k)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{+int64_t(d), -int64_t(k)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{+int64_t(k), -int64_t(d)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{-int64_t(k), -int64_t(d)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{+int64_t(k), +int64_t(d)}) += t * prec[d][k] / 8;
                            MM.at(z + coo{-int64_t(k), +int64_t(d)}) += t * prec[d][k] / 8;
                        }
                    }
                }
            }
        }

        prec[r].push_back(4 * MM.at({0, r}));
        for (size_t i = 1; i < r; ++i) prec[r].push_back(8 * MM.at({0, r + i}));
    }
    return prec;
}

auto uniform_circle(size_t r) -> coo {
    double theta = prng.uniform_real(0, 2 * M_PI);
    return {int64_t(double(r) * cos(theta)), int64_t(double(r) * sin(theta))};
}

class DLA : public CoarseImage {
public:
    DLA(size_t n_, const Hub &H)
        : CoarseImage(H.title, {n_, n_}), n(n_), c(H['c']), QT({0, 0}, {int64_t(n), int64_t(n)}, H['l']),
          prec(harmonic_measures(H['p'])), mid{n / 2, n / 2} {}

    void show() override {
        W.watch(QT.n, "Nb of particles");
        W.watch(r, "Cluster radius");
        W.show();
        img.show();
        CoarseImage::show();
        start = now();
    }

    [[nodiscard]] auto at(coo z) const -> bool { return fits(z) && CoarseImage::at(ucoo(z)); }

    void put(ucoo z) {
        CoarseImage::put(z, true);
        QT.insert(coo(z));
        r = std::max(r, size_t(sup(coo(z) - coo(mid))));
    }

    [[nodiscard]] auto neighbor(coo z) const -> bool {
        for (unsigned ii = 0; ii < 4; ++ii)
            if (at(z + dz[ii])) return true;
        return false;
    }

    [[nodiscard]] auto jump(size_t d) const -> coo {
        if (d <= 1) return dz[prng.uniform_int(4U)];
        if (d < prec.size()) {
            coo w{int64_t(d), prng.discrete(prec[d])};
            if (prng.bernoulli()) w.x = -w.x;
            if (prng.bernoulli()) w.y = -w.y;
            if (prng.bernoulli()) swap(w.x, w.y);
            return w;
        }
        if (d < c) return jump(prec.size() - 1);
        return uniform_circle(d - c / 2);
    }

    void runDLA() {
        put(mid);
        while (r < size_t(n) / 2 - 1) {
            coo       z = uniform_circle(2 * r + 20) + mid;
            QuadIndex qi{coo(mid), sup(z - mid)};
            while (!neighbor(z)) {
                qi.d = sup(z - qi.z);
                QT.nn(z, qi);
                z += jump(size_t(qi.d) - 1);
                if (size_t(sup(z)) > 100 * r) { z += (mid - z) / 10; }
            }
            put(ucoo(z));
        }
    }

    void paint() override {
        QT.paint(img, {0, 0}, 512);
        CoarseImage::paint();
    }

    size_t                 n, c, r = 1;
    Console                W;
    QuadTree               QT;
    vector<vector<double>> prec;
    Image                  img{"QuadTree", {512, 512}};
    ucoo                   mid;
};

auto main(int argc, char **argv) -> int {
    Hub H("Lattice DLA", argc, argv, "n=2000,p=64,c=50,l=30,f,s=0");
    if (auto s = size_t(H['s'])) prng.seed(s);
    DLA dla(H['n'], H);
    dla.show();
    dla.runDLA();
    dla.output(H.title);
    if (H['f']) dla.output_fine("dla.png");
    H.output("Final particle number", "m", dla.QT.n);
}
