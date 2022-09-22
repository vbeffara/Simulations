#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/math/Pairings.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Toroidal map enumeration");
    auto d = clp.param("d", size_t(0), "Minimum vertex degree");
    auto D = clp.param("D", size_t(0), "Maximum vertex degree (0 for unbounded)");
    auto r = clp.param("r", size_t(0), "Seed for the PRNG (0 for time-based)");
    auto s = clp.param("s", size_t(2), "Number of vertices");
    auto f = clp.flag("f", "Identify maps obtained after mirror flip");
    auto o = clp.flag("o", "Enable splitting, circle packing and output");
    auto q = clp.flag("q", "Refine map embedding using high precision arithmetic");
    clp.finalize();

    if (r > 0) prng.seed(r);

    Permutation phi(6 * s);
    std::generate(begin(phi), end(phi), [n = 0]() mutable { return ((++n) % 3) != 0 ? n : n - 3; });

    vector<Hypermap>     v;
    const vector<size_t> ntri{0, 1, 5, 46, 669, 11096, 196888, 3596104, 66867564, 1258801076}; // https://oeis.org/A292408
    size_t const         target = ((d == 0) && (!f) && (s < ntri.size())) ? ntri[s] : 0;

    Coloring img("Toroidal enumeration", cpx(-1, -1), cpx(1, 1), 500, [](cpx /*unused*/) { return BLACK; });

    while ((target == 0) || (v.size() < target)) {
        Permutation const alpha = Pairings(6 * s).rrand();
        if (!connected(phi, alpha)) continue;

        Hypermap M((alpha * phi).inverse(), alpha, phi);
        if (M.genus() != 1) continue;

        auto cc = M.sigma.cycles();
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

        spdlog::info("Toroidal enumeration (s={}, pass {}, i={})", s, M.sigma.passport(), v.size());
        spdlog::info("     Order number:    {}", v.size());
        spdlog::info("     Passport:        {}", M.sigma.passport());
        spdlog::info("     Sigma:           {}", M.sigma);
        spdlog::info("     Alpha:           {}", M.alpha);
        spdlog::info("     Phi:             {}", M.phi);

        v.push_back(M);
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
        real_t const           c  = CC.findn();
        int const              lc = -int(log10(c));
        unsigned const         nd = unsigned(std::max(10, lc / 2 - 15));
        spdlog::info("     Modulus:         {}", CC.tau());
        if (nd > 30)
            if (auto P = guess(CC.tau(), nd)) spdlog::info("        root of {}", *P);
        spdlog::info("     Klein invariant: {}", CC.E.j());
        if (nd > 30)
            if (auto P = guess(CC.E.j(), nd)) spdlog::info("        root of {}", *P);
        spdlog::info("     g2 coefficient:  {}", CC.E.g2());
        if (nd > 30)
            if (auto P = guess(CC.E.g2(), nd)) spdlog::info("        root of {}", *P);
        spdlog::info("     g3 coefficient:  {}", CC.E.g3());
        if (nd > 30)
            if (auto P = guess(CC.E.g3(), nd)) spdlog::info("        root of {}", *P);
    }
}
