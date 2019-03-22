#include <spdlog/spdlog.h>
#include <vb/CoarseImage.h>
#include <vb/Console.h>
#include <vb/QuadTree.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class DLA : public CoarseImage {
public:
    explicit DLA(const Hub &H)
        : CoarseImage(H.title, {H['n'], H['n']}, unsigned(pow(double(H['n']), .33))), n(H['n']), c(H['c']), r(1),
          QT({-n / 2, -n / 2}, {n / 2, n / 2}, H['l']), prec(unsigned(H['p'])), img(H.title, {512, 512}) {
        z0 = {n / 2, n / 2};
        W.watch(QT.n, "Nb of particles");
        W.watch(r, "Cluster radius");

        spdlog::info("Precomputing harmonic measures, d up to {} ...", int(H['p']));
        for (size_t r = 1; r < size_t(H['p']); ++r) {
            bool          dirty = true;
            Array<double> MM({2 * r + 1, 2 * r + 1});
            MM.at({r, r}) = 1;
            while (dirty) {
                dirty = false;
                for (size_t i = 1; i < 2 * r; ++i)
                    for (size_t j = 1; j < 2 * r; ++j) {
                        ucoo   z{i, j};
                        double t = MM.at(z);
                        MM.at(z) = 0;
                        if (t > 1e-13) {
                            dirty = true;

                            auto ii = (i <= r) ? i : 2 * r - i;
                            auto jj = (j <= r) ? j : 2 * r - j;
                            auto d  = min(ii, jj);
                            if (d == r) --d;

                            if (d <= 1)
                                for (gsl::index k = 0; k < 4; ++k) MM.at(ucoo(coo(z) + dz[k])) += t / 4;
                            else {
                                const auto &ps = prec[d];
                                for (size_t k = 0; k < size_t(d); ++k) {
                                    MM.at({i - d, j + k}) += t * ps[k] / 8;
                                    MM.at({i - d, j - k}) += t * ps[k] / 8;
                                    MM.at({i + d, j + k}) += t * ps[k] / 8;
                                    MM.at({i + d, j - k}) += t * ps[k] / 8;
                                    MM.at({i + k, j - d}) += t * ps[k] / 8;
                                    MM.at({i - k, j - d}) += t * ps[k] / 8;
                                    MM.at({i + k, j + d}) += t * ps[k] / 8;
                                    MM.at({i - k, j + d}) += t * ps[k] / 8;
                                }
                            }
                        }
                    }
            }

            prec[size_t(r)].push_back(4 * MM.at({0, r}));
            for (size_t i = 1; i < r; ++i) prec[r].push_back(8 * MM.at({0, r + i}));
        }
        start = now();
    };

    void show() override {
        W.show();
        img.show();
        CoarseImage::show();
    }

    bool at(coo z) const { return contains(z) && CoarseImage::at(z); }

    void put(coo z) {
        CoarseImage::put(z, true);
        QT.insert(z);
        r = std::max(r, sup(z));
    }

    bool neighbor(coo z) const {
        for (int i = 0; i < 4; ++i)
            if (at(z + dz[i])) return true;
        return false;
    }

    coo jump(long d) const {
        if (d <= 1) return dz[prng.uniform_int(4)];
        if (d < int(prec.size())) {
            coo w{d, prng.discrete(prec[size_t(d)])};
            if (prng.bernoulli()) w.x = -w.x;
            if (prng.bernoulli()) w.y = -w.y;
            if (prng.bernoulli()) swap(w.x, w.y);
            return w;
        }
        if (d < c) return jump(long(prec.size()) - 1);
        auto   l     = d - c / 2;
        double theta = prng.uniform_real(0, 2 * M_PI);
        auto   x = int(l * cos(theta)), y = int(l * sin(theta));
        return {x, y};
    }

    void runDLA() {
        put({0, 0});
        while (r < n / 2 - 1) {
            double    t = prng.uniform_real(0, 2 * M_PI);
            coo       z{int((2 * r + 20) * cos(t)), int((2 * r + 20) * sin(t))};
            QuadIndex qi{{0, 0}, sup(z)};
            while (!neighbor(z)) {
                qi.d = sup(z - qi.z);
                QT.nn(z, qi);
                z += jump(qi.d - 1);
                if (sup(z) > 100 * r) {
                    z.x -= int64_t(z.x / 10);
                    z.y -= int64_t(z.y / 10);
                }
            }
            put(z);
        }
    }

    // TODO: this should go in QuadTree
    void paint() override {
        QT.paint(img, {0, 0}, 512);
        CoarseImage::paint();
    }

    int                    n, c;
    int64_t                r;
    Console                W;
    QuadTree               QT;
    vector<vector<double>> prec;
    Image                  img;
};

int main(int argc, char **argv) {
    Hub H("Lattice DLA", argc, argv, "n=2000,p=64,c=50,l=30,f,s=0");
    if (auto s = unsigned(H['s'])) prng.seed(s);
    DLA dla(H);
    dla.show();
    dla.runDLA();
    dla.output(H.title);
    if (H['f']) dla.output_fine("dla.png");
    spdlog::info("Final cluster: {} particles, diameter = {}", dla.QT.n, dla.r);
}
