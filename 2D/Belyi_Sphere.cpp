#include <spdlog/spdlog.h>
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

Stream<Hypermap> triangulations(unsigned n) {
    Cycles phic;
    for (unsigned i = 0; i < n / 3; ++i) phic.emplace_back(std::vector<unsigned>{i, i + n / 3, i + 2 * n / 3});
    Permutation phi(phic);
    unsigned    np = n / 6;
    vector<int> a(n / 2 - np, 2);

    return Stream<Hypermap>([a, phi, n, np](Sink<Hypermap> &yield) {
        std::vector<Hypermap> hs;
        for (auto alph : permutations(a)) {
            Permutation alpha(n);
            for (unsigned i = 0; i + 2 * np < n; ++i) alpha[i] = alph[i];
            for (unsigned i = 0; i < np; ++i) {
                alpha[n - 2 * np + 2 * i]     = n - 2 * np + 2 * i + 1;
                alpha[n - 2 * np + 2 * i + 1] = n - 2 * np + 2 * i;
            }
            if (!connected(phi, alpha)) continue;
            Permutation sigma = (alpha * phi).inverse();
            Hypermap    h(sigma, alpha, phi);
            h.normalize();
            bool done = false;
            for (auto &hh : hs)
                if (h == hh) {
                    done = true;
                    break;
                }
            if (!done) {
                hs.push_back(h);
                yield(h);
            }
        }
    });
}

int main(int argc, char **argv) {
    Hub      H("Spheroidal enumeration", argc, argv, "s=3,m=228,d=2,g=0,v,o,b,q");
    int      s = H['s'], g = 0, a = 6 * (s - 2);
    unsigned d = H['d'];
    assert(a > 0);

    int nb = 0;
    for (Hypermap &M : triangulations(a)) {
        if (M.genus() != g) continue;
        bool good = true;
        for (auto &c : M.sigma.cycles())
            if (c.size() < d) good = false;
        if (!good) continue;

        spdlog::info("{}: {}", ++nb, M);

        if (H['v'] || H['o'] || H['b'] || H['q']) {
            H.title = fmt::format("Spheroidal enumeration (s={}, d={}, i={})", s, d, nb);
            Constellation0<double> C{M};
            C.belyi();
            if (H['b']) spdlog::info("{}", C);
            if (H['v'] || H['o']) {
                auto     bd = C.bounds();
                Coloring CC(H.title, bd.first, bd.second, 800, [&](cpx z) { return Indexed((imag(C(z)) > 0) ? 1 : 2); });
                CC.scale(1.5);
                CC.show();
                if (H['o'])
                    CC.output(H.title);
                else {
                    while (CC.visible()) {
                        CC.update();
                        Fl::wait();
                    }
                };
                CC.hide();
            }
            if (H['q']) {
                Constellation0<real_t> Cq(C);
                Cq.findn();
                Cq.belyi();
                spdlog::info("{}", Cq);
            }
        }
    }
}
