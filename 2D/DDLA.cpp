#include <vb/CoarseImage.h>
#include <vb/data/Queue.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class DDLA : public CoarseImage {
public:
    explicit DDLA(const string &title_, size_t n_, double p_, bool f_)
        : CoarseImage(title_, {n_, n_}, f_ ? unsigned(pow(n_, .33)) : 1), f(f_), n(n_), p(p_) {
        put({0, 0}, true);
        pq.push({{1, 0}, prng.exponential() / p});
        pq.push({{0, 1}, prng.exponential() / (1 - p)});
    };

    auto ok(ucoo z) -> bool {
        while (true) {
            if ((z.x >= n) || (z.y >= n) || (z.x + z.y > cursum)) return true;
            if (at(z)) return false;
            z += dz[prng.bernoulli(p) ? 1 : 0];
        }
    }

    void fill(ucoo z) {
        if ((z.x > 0) && (z.y < n - 1) && at(z + coo{-1, 1}) && !at(z - coo{1, 0})) {
            put(z - coo{1, 0}, true);
            fill(z - coo{1, 0});
        }
        if ((z.y > 0) && (z.x < n - 1) && at(z + coo{1, -1}) && !at(z - coo{0, 1})) {
            put(z - coo{0, 1}, true);
            fill(z - coo{0, 1});
        }
    }

    void run() {
        while (true) {
            const auto  pt = pq.get();
            const ucoo &z  = pt;

            if ((z.x == n - 1) || (z.y == n - 1)) return;
            if (!at(z)) {
                double const curtime = pt.t;
                if (ok(z)) {
                    put(z, true);
                    cursum = max(cursum, z.x + z.y);
                    if (f) fill(z);
                    if ((z.x < n - 1) && !at(z + coo{1, 0})) pq.push({z + coo{1, 0}, curtime + prng.exponential() / p});
                    if ((z.y < n - 1) && !at(z + coo{0, 1})) pq.push({z + coo{0, 1}, curtime + prng.exponential() / (1 - p)});
                } else {
                    pq.push({z, curtime + prng.exponential()});
                }
            }
        }
    }

    bool        f;
    size_t      n, cursum{0};
    double      p;
    Queue<ucoo> pq;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Directed DLA");
    auto n = clp.param("n", size_t(750), "Simulation size");
    auto p = clp.param("p", .5, "Random walk parameter");
    auto f = clp.flag("f", "Fill holes and build a CoarseImage");
    clp.finalize();

    DDLA img(clp.desc, n, p, f);
    img.show();
    img.run();
    img.output(clp.title);
}
