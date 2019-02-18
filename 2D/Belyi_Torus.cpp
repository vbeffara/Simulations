// d: minimum degree of the triangulation
// f: identify maps obtained after mirror flip
// g: genus of the triangulation
// m: mode for the generated pdf (see vb/Toroidal.h)
// n: number of subdivisions for the circle packing
// o: enable splitting, circle packing and output (fails if g!=1)
// r: seed for the PRNG, set to 0 for time-based
// s: number of vertices

#include <iomanip>
#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/ProgressBar.h>
#include <vb/data/Pairings.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

int            ntri_[] = {0, 1, 5, 46, 669};
gsl::span<int> ntri{ntri_};

int main(int argc, char **argv) {
    Hub      H("Toroidal enumeration", argc, argv, "s=1,m=228,r=1,o,d=0,D=0,g=1,f,n=2,q");
    int      s = H['s'], g = H['g'], r = H['r'];
    unsigned D = H['D'], d = H['d'], a = 6 * (s + 2 * g - 2);
    assert(a > 0);
    if (g != 1) assert(!H['o']);
    if (r > 0) prng.seed(r);

    Cycles phi_c;
    for (unsigned i = 0; i < a / 3; ++i) phi_c.emplace_back(std::vector<unsigned>{3 * i, 3 * i + 1, 3 * i + 2});
    Permutation phi(phi_c);

    vector<Hypermap> v;
    unsigned         target = 0;
    if ((d == 0) && (g == 1) && (!H['f']) && (s < ntri.size())) target = ntri[s];

    Coloring img(H.title, cpx(-1, -1), cpx(1, 1), 500, [](cpx) { return BLACK; });

    while ((target == 0) || (v.size() < target)) {
        Permutation alpha = Pairings(a).rrand();
        if (!connected(phi, alpha)) continue;
        Permutation sigma = (alpha * phi).inverse();
        Hypermap    M(sigma, alpha, phi);
        if (M.genus() != g) continue;
        bool good = true;
        for (auto &c : sigma.cycles())
            if (c.size() < d) good = false;
        if (!good) continue;
        good = true;
        if (D > 0)
            for (auto &c : sigma.cycles())
                if (c.size() > D) good = false;
        if (!good) continue;

        M.normalize();
        bool there = false;
        for (Hypermap &O : v)
            if (O == M) there = true;
        if (H['f']) {
            Hypermap MM = M;
            MM.mirror();
            MM.normalize();
            for (Hypermap &O : v)
                if (O == MM) there = true;
        }
        if (!there) {
            v.push_back(M);
            cout << "Sigma: " << M.sigma << endl;
            cout << "Alpha: " << M.alpha << endl;
            cout << "Phi:   " << M.phi << endl;
            cout << endl;
            cout << "     Order number:    " << v.size() << endl;
            cout << "     Passport:        " << fmt::format("{}", M.sigma.passport()) << endl;

            H.title = fmt::format("Toroidal enumeration (s={}, pass {}, i={})", s, M.sigma.passport(), v.size());
            Constellation1<double> C(H, M);
            double                 er = C.cost();
            if (!H['q']) {
                cout << "     Final error:     " << er << endl;
                cout << "     Modulus:         " << C.tau() << endl;
                cout << "     Klein invariant: " << C.E.j() << endl;
                cout << "     g2 coefficient:  " << C.E.g2() << endl;
                cout << "     g3 coefficient:  " << C.E.g3() << endl;
                cout << endl;
            }

            if (H['o']) {
                img.label(H.title.c_str());
                auto bd = C.bounds();
                img.z1  = bd.first;
                img.z2  = bd.second;
                img.f   = [&](cpx z) { return Indexed((imag(C(z)) > 0) ? 1 : 2); };
                img.scale(1.3);
                img.show();
                img.output(H);
            }

            if (H['q']) {
                Constellation1<real_t> CC(C);
                real_t                 c   = CC.findn();
                int                    lc  = -int(log10(c));
                int                    nd  = std::max(10, lc / 2 - 15);
                real_t                 eps = pow(real_t(.1), nd);
                cout << fixed << setprecision(std::min(nd, 80));
                cout << "     Modulus:         " << CC.tau() << endl;
                if (nd > 30) {
                    auto P = guess(CC.tau(), nd);
                    if (P) cout << "        root of " << *P << std::endl;
                }
                cout << "     Klein invariant: " << CC.E.j() << endl;
                if (nd > 30) {
                    auto P = guess(CC.E.j(), nd);
                    if (P) cout << "        root of " << *P << std::endl;
                }
                cout << "     g2 coefficient:  " << CC.E.g2() << endl;
                if (nd > 30) {
                    auto P = guess(CC.E.g2(), nd);
                    if (P) cout << "        root of " << *P << std::endl;
                }
                cout << "     g3 coefficient:  " << CC.E.g3() << endl;
                if (nd > 30) {
                    auto P = guess(CC.E.g3(), nd);
                    if (P) cout << "        root of " << *P << std::endl;
                }
                cout << endl << scientific << setprecision(6);
            }
        }
    }
}
