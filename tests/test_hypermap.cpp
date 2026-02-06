#include <spdlog/spdlog.h>
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Hypermap of genus 0");
    auto m = clp.param("m", 228, "Map type");
    auto v_flag = clp.flag("v", "Visualize");
    auto q = clp.flag("q", "Quiet mode");
    auto g = clp.param("g", std::string("m_cube"), "Graph name");
    auto p_flag = clp.flag("p", "P flag");
    auto a_flag = clp.flag("a", "A flag");
    auto f_val = clp.param("f", 0, "Number of flips");
    auto s = clp.param("s", size_t(0), "Random seed");
    auto u = clp.param("u", size_t(0), "Genus 0 hypermap size");
    auto d = clp.param("d", size_t(1), "Minimum cycle size");
    auto D = clp.param("D", size_t(0), "Maximum cycle size");
    auto o = clp.flag("o", "Output image");
    clp.finalize();

    auto M = HLib().at(g);
    if (u != 0) M = H_genus0(u);
    int f = f_val;
    if (f != 0) {
        if (s != 0) prng.seed(s);
        bool     bad = true;
        while (bad) {
            for (int i = 0; i < f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
            bad = false;
            f   = 1;
            for (const auto &cyc : M.sigma.cycles()) {
                if (cyc.size() < d) bad = true;
                if ((D > 0) && (cyc.size() > D)) bad = true;
            }
        }
    }
    spdlog::info("{}", M);

    Constellation0<double> C{M};
    C.belyi();
    if (!q) std::cout << std::endl << C;

    if (v_flag) {
        auto [ul, br] = C.bounds();
        Coloring CC(clp.title, ul, br, 800, [&](cpx z) { return Indexed(imag(C(z)) > 0 ? 1 : 2); });
        CC.scale(1.5);
        CC.show();
        if (o) CC.output(clp.title);
        while (CC.visible()) {
            CC.update();
            Fl::wait();
        }
    }

    if (!q) return 0;
    Constellation0<real_t> Cq(C);
    Cq.findn();
    Cq.belyi();
    std::cout << std::endl << Cq;
}
