// m: mode for the generated pdf (see vb/Toroidal.h)
// n: number of subdivisions for the circle packing

#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/math/Pairings.h>
#include <vb/util/CLP.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

const vector<size_t> ntri{0, 1, 5, 46, 669};

int main(int argc, char **argv) {
    CLP  clp(argc, argv, "Toroidal map enumeration");
    auto d = clp("d", size_t(0), "Minimum vertex degree");
    auto D = clp("D", size_t(0), "Maximum vertex degree (0 for unbounded)");
    auto f = clp("f", "Identify maps obtained after mirror flip");
    auto o = clp("o", "Enable splitting, circle packing and output");
    auto q = clp("q", "Refine map embedding using high precision arithmetic");
    auto r = clp("r", size_t(0), "Seed for the PRNG (0 for time-based)");
    auto s = clp("s", size_t(2), "Number of vertices");
    clp.finalize();

    Hub H("Toroidal enumeration", argc, argv, "m=228,n=2,q");
    if (r > 0) prng.seed(r);

    Cycles phi_c;
    for (size_t i = 0; i < 2 * s; ++i) phi_c.emplace_back(std::vector<size_t>{3 * i, 3 * i + 1, 3 * i + 2});
    Permutation phi(phi_c);

    vector<Hypermap> v;
    size_t           target = ((d == 0) && (!f) && (s < ntri.size())) ? ntri[s] : 0;

    Coloring img("Toroidal enumeration", cpx(-1, -1), cpx(1, 1), 500, [](cpx /*unused*/) { return BLACK; });

    while ((target == 0) || (v.size() < target)) {
        Permutation alpha = Pairings(6 * s).rrand();
        if (!connected(phi, alpha)) continue;

        Permutation sigma = (alpha * phi).inverse();
        Hypermap    M(sigma, alpha, phi);
        if (M.genus() != 1) continue;

        auto cc = sigma.cycles();
        if ((d > 0) && std::any_of(begin(cc), end(cc), [d](const auto &c) { return c.size() < d; })) continue;
        if ((D > 0) && std::any_of(begin(cc), end(cc), [D](const auto &c) { return c.size() > D; })) continue;

        M.normalize();
        if (std::find(begin(v), end(v), M) != end(v)) continue;
        if (f) {
            Hypermap MM = M;
            MM.mirror();
            MM.normalize();
            if (std::find(begin(v), end(v), MM) != end(v)) continue;
        }

        v.push_back(M);
        spdlog::info("{}", M);
        spdlog::info("     Order number:    {}", v.size());
        spdlog::info("     Passport:        {}", M.sigma.passport());

        Constellation1<double> C(M);

        if (o) {
            auto title = fmt::format("Toroidal enumeration (s={}, pass {}, i={})", s, M.sigma.passport(), v.size());
            img.label(title.c_str());
            auto bd = C.bounds();
            img.z1  = bd.first;
            img.z2  = bd.second;
            img.f   = [&](cpx z) { return Indexed((imag(C(z)) > 0) ? 1 : 2); };
            img.scale(1.3);
            img.show();
            img.output(title);
        }

        if (!q) {
            spdlog::info("     Final error:     {}", C.cost());
            spdlog::info("     Modulus:         {}", C.tau());
            spdlog::info("     Klein invariant: {}", C.E.j());
            spdlog::info("     g2 coefficient:  {}", C.E.g2());
            spdlog::info("     g3 coefficient:  {}", C.E.g3());
            continue;
        }

        Constellation1<real_t> CC(C);
        real_t                 c  = CC.findn();
        int                    lc = -int(log10(c));
        unsigned               nd = unsigned(std::max(10, lc / 2 - 15));
        cout << fixed << setprecision(std::min(int(nd), 80));
        cout << "     Modulus:         " << CC.tau() << endl;
        if (nd > 30) {
            auto P = guess(CC.tau(), nd);
            if (P) cout << fmt::format("        root of {}", *P) << std::endl;
        }
        cout << "     Klein invariant: " << CC.E.j() << endl;
        if (nd > 30) {
            auto P = guess(CC.E.j(), nd);
            if (P) cout << fmt::format("        root of {}", *P) << std::endl;
        }
        cout << "     g2 coefficient:  " << CC.E.g2() << endl;
        if (nd > 30) {
            auto P = guess(CC.E.g2(), nd);
            if (P) cout << fmt::format("        root of {}", *P) << std::endl;
        }
        cout << "     g3 coefficient:  " << CC.E.g3() << endl;
        if (nd > 30) {
            auto P = guess(CC.E.g3(), nd);
            if (P) cout << fmt::format("        root of {}", *P) << std::endl;
        }
        cout << endl << scientific << setprecision(6);
    }
}
