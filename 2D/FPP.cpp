#include <vb/CoarseImage.h>
#include <vb/Point.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

double cost() { return prng.exponential(); }

class FPP : public CoarseImage {
public:
    FPP(const Hub &H, int n) : CoarseImage(H.title, {n, n}, int(pow(n, .33))), area(0) {
        invasion = H['i'];
        twostep  = H['2'];
        trace    = H['t'];

        pq.push({{n / 2, n / 2}, cost()});
        if (twostep) pq.push({{int64_t(true_width / 2) + 1, int64_t(true_height) / 2}, cost()});
    };

    void spread(double t, const coo &z) {
        for (int d = 0; d < 4; ++d) {
            auto zz = z + dz[d];
            if (!at(zz)) pq.push({zz, t + cost()});
        }
    }

    void run() {
        while (true) {
            if (trace) std::cout << area << " " << pq.size() << std::endl;

            auto       pt = pq.get();
            const coo &z  = pt;
            if (!at(z)) {
                put(z, true);
                ++area;

                double curtime = invasion ? 0.0 : pt.t;

                int deg = 1;
                if (twostep)
                    for (int d = 0; d < 4; ++d) deg += (at(z + dz[d]) ? 1 : 0);

                for (int i = 0; i < deg; ++i) spread(curtime, z);
                if (twostep)
                    for (int d = 0; d < 4; ++d) {
                        auto zz = z + dz[d];
                        if (at(zz)) spread(curtime, zz);
                    }

                if ((z.x == 1) || (z.y == 1) || (z.x == true_width - 2) || (z.y == true_height - 2)) break;
            }
        }
    }

    int        area;
    bool       trace, invasion, twostep;
    PointQueue pq;
};

int main(int argc, char **argv) {
    Hub H("First-passage percolation", argc, argv, "n=5000,i,2,t");
    FPP F(H, H['n']);
    F.show();
    F.run();
    F.output(H.title);
}
