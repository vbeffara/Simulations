#include <spdlog/spdlog.h>
#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Hypermap of genus 1");
    auto m = clp.param("m", 228, "Map type");
    auto v_flag = clp.flag("v", "Visualize");
    auto w_flag = clp.flag("w", "W flag");
    auto q = clp.flag("q", "Quiet mode");
    auto n = clp.param("n", size_t(0), "Genus 1 hypermap size");
    auto g = clp.param("g", std::string("lat_csquare"), "Graph name");
    auto p_flag = clp.flag("p", "P flag");
    auto f = clp.param("f", size_t(0), "Number of flips");
    auto s = clp.param("s", size_t(0), "Random seed");
    auto a_flag = clp.flag("a", "A flag");
    auto b_flag = clp.flag("b", "B flag");
    auto r = clp.param("r", size_t(0), "Artem parameter");
    unsigned const N = clp.param("N", size_t(100), "Precision"); default_precision(N);
    unsigned const W = clp.param("W", size_t(800), "Window size");
    double const S = clp.param("S", 2.0, "Scale factor");
    clp.finalize();

    auto M = HLib().at(g);

    if (n != 0) M = H_genus1(n);
    if (r != 0) M = H_artem(r);
    if (f != 0) {
        if (s != 0) prng.seed(s);
        for (unsigned i = 0; i < f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
    }

    spdlog::info("{}", M);
    Constellation1<double> C{M};
    C.findn();

    if (!q) { cout << endl << C; }
    if (v_flag) {
        // Image I (800,800); I.show(); C.draw(I,a_flag,b_flag); I.pause(); I.hide();
        auto     bd = C.bounds();
        Coloring CC(clp.title, bd.first, bd.second, W, [&](cpx z) { return Indexed (imag(C(z)) > 0 ? 1 : 2); });
        CC.scale(S);
        CC.show();
        while (CC.visible()) {
            CC.update();
            Fl::wait();
        }
        CC.output(clp.title);
    }

    if (q) {
        Constellation1<real_t> Cq(C);
        auto c = Cq.findn();
        cout << endl << Cq;
    }
}
