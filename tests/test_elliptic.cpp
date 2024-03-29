#include <spdlog/spdlog.h>
#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    Hub const H("Hypermap of genus 1", argc, argv, "m=228,v,w,q,n=0,g=lat_csquare,p,f=0,s=0,a,b,r=0");
    auto M = HLib().at(H['g']);

    if (unsigned const n = H['n']; n != 0) M = H_genus1(n);
    if (unsigned const r = H['r']; r != 0) M = H_artem(r);
    if (unsigned const f = H['f']; f != 0) {
        if (unsigned const s = (H['s']); s != 0) prng.seed(s);
        for (unsigned i = 0; i < f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
    }

    spdlog::info("{}", M);
    Constellation1<double> C{M};
    C.findn();

    if (!H['q']) { cout << endl << C; }
    if (H['v']) {
        // Image I (800,800); I.show(); C.draw(I,H['a'],H['b']); I.pause(); I.hide();
        auto     bd = C.bounds();
        Coloring CC(H.title, bd.first, bd.second, 800, [&](cpx z) { return HSV((imag(C(z)) > 0) ? 0 : .5, .8, .8); });
        CC.scale(1.5);
        CC.show();
        while (CC.visible()) {
            CC.update();
            Fl::wait();
        }
    }

    if (H['q']) {
        Constellation1<real_t> Cq(C);
        Cq.findn();
        cout << endl << Cq;
    }
}
