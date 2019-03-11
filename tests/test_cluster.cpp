#include <spdlog/spdlog.h>
#include <vb/Cluster.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Cluster2 : public Cluster {
public:
    int64_t dist_to_box(coo z) { return max(int64_t(0), min({z.x - (ul.x + w - 1), z.y - (ul.y + w - 1), ul.x - z.x, ul.y - z.y})); }
    int64_t dist_to_out(coo z) { return max(int64_t(0), min({z.x - ul.x + 1, z.y - ul.y + 1, (ul.x + w) - z.x, (ul.y + w) - z.y})); }

    int64_t inrad_go(coo z) {
        if (np == w * w) return dist_to_out(z);
        if (w == bs) return 1;
        int64_t ww = w / 3, i = (z.x - ul.x) / ww, j = (z.y - ul.y) / ww;
        auto    subij = static_cast<Cluster2 *>(static_cast<void *>(&sub[size_t(i + 3 * j)]));
        return subij->inrad_go(z);
    }
    int64_t inrad(coo z) {
        if ((dist_to_box(z) > 0) || (!at(z))) return 0;
        return inrad_go(z);
    }
};

int main(int argc, char **argv) {
    Hub    H("Once-reinforced random walk, Cluster version", argc, argv, "l=0,a=10,v,s=0");
    int    l = H['l'];
    double a = H['a'];
    a        = 1 / (1 + a);
    if (unsigned s = H['s']; s != 0) prng.seed(s);
    Image I(H.title, {729, 729});
    I.show();
    if (H['v']) I.snapshot_setup("ORRW_cluster", 60);

    Cluster2 W;
    coo      z{0, 0};
    W.insert(z);
    int64_t sup_ = 0, t_ = 0;

    for (int64_t t = 0;; t++) {
        int64_t ir = W.inrad_go(z), d = 1 + ir / 2;
        coo     nz    = z + dz[prng.uniform_int(4)] * d;
        bool    there = (d > 1) ? true : W.at(nz);
        if (there || prng.bernoulli(a)) {
            z = nz;
            if (!there) W.insert(z);
            int64_t ns = sup(z);
            if ((ns >= sup_ + 10) || (t >= t_ + 1000000)) {
                sup_ = max(sup_, ns);
                t_   = t;
                W.paint(I);
                spdlog::info("time = {:<15} radius = {:>5} {:>9} particles", double(t), sup_, W.np);
                if ((l != 0) && (sup_ >= l)) break;
            }
        }
    }
}
