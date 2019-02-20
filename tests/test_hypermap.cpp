#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub H("Hypermap of genus 0", argc, argv, "m=228,v,q,g=m_cube,p,a,f=0,s=0,u=0,d=1,D=0,o");

    auto M = HLib().at(H['g']);
    if (int u = H['u']; u != 0) M = H_genus0(u);
    if (int f = H['f']; f != 0) {
        if (int s = (H['s']); s != 0) prng.seed(s);
        unsigned d = H['d'], D = H['D'];
        bool     bad = true;
        while (bad) {
            for (int i = 0; i < f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
            bad = false;
            f   = 1;
            for (const auto &s : M.sigma.cycles()) {
                if (s.size() < d) bad = true;
                if ((D > 0) && (s.size() > D)) bad = true;
            }
        }
    }
    std::cout << M;

    Constellation0<double> C{M, H['m']};
    C.belyi();
    if (!H['q']) std::cout << std::endl << C;

    if (H['v']) {
        auto [ul, br] = C.bounds();
        Coloring CC(H.title, ul, br, 800, [&](cpx z) { return Indexed(imag(C(z)) > 0 ? 1 : 2); });
        CC.scale(1.5);
        CC.show();
        if (H['o']) CC.output(H.title);
        while (CC.visible()) {
            CC.update();
            Fl::wait();
        }
    }

    if (!H['q']) return 0;
    Constellation0<real_t> Cq(C);
    Cq.findn();
    Cq.belyi();
    std::cout << std::endl << Cq;
}
