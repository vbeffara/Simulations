#include <vb/CoarseImage.h>
#include <vb/math/math.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Bounces : public map<string, function<coo(coo)>> {
public:
    explicit Bounces(const Hub &H) {
        emplace("none", [&](coo /*unused*/) { return coo{0, 0}; });
        emplace("line", [&](coo z) {
            double const p = H['p'];
            return coo{int(-p * double(z.x)), int(-p * double(z.y))};
        });
        emplace("idla", [&](coo z) { return -z; });
        emplace("sign", [&](coo z) { return coo{-sign(z.x), -sign(z.y)}; });
        emplace("sig2", [&](coo z) {
            int64_t a = H['a'], k = sign(z.x * z.y);
            return coo{-sign(z.x), -sign(z.y)} * (1 + (a - 1) * (1 + k) / 2);
        });
        emplace("soft", [&](coo z) {
            int const k = z.x > 2 * abs(z.y) ? H['a'] : 1;
            return coo{-sign(z.x), -sign(z.y)} * k;
        });
        emplace("sqrt", [&](coo z) {
            double const l = 1 + sqrt(norm(z));
            return coo{int(-double(z.x) / sqrt(l)), int(-double(z.y) / sqrt(l))};
        });

        emplace("maxi", [&](coo z) { return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -sign(z.y)}; });
        emplace("max2", [&](coo z) {
            int const a = H['a'];
            return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -a * sign(z.y)};
        });
        emplace("maxb", [&](coo z) {
            double const l = pow(norm(z), double(H['b']));
            return (abs(z.x) > abs(z.y)) ? coo{int(-l * sign(double(z.x))), 0} : coo{0, int(-l * sign(double(z.y)))};
        });

        emplace("dent", [&](coo z) {
            if (z.x + z.y > 2 * abs(z.x - z.y)) return (z.y > z.x) ? coo{-1, 0} : coo{0, -1};
            /* else */ return (abs(z.y) > abs(z.x)) ? coo{0, -sign(z.y)} : coo{-sign(z.x), 0};
        });

        emplace("octo", [&](coo z) {
            double a = H['a'], b = H['b'];
            if (abs(z.x) > 2 * abs(z.y)) return coo{int(-a * sign(double(z.x))), 0};
            if (abs(z.y) > 2 * abs(z.x)) return coo{0, int(-a * sign(double(z.y)))};
            return coo{int(-b * sign(double(z.x))), int(-b * sign(double(z.y)))};
        });
    }
};

class Bouncy : public CoarseImage {
public:
    Bouncy(const Hub &H, size_t n, const string &j)
        : CoarseImage(H.title, {n, n}, size_t(pow(n, .25))), jump(Bounces(H).at(j)), mid(size / 2) {
        if (H['g']) {
            tree = make_unique<Image>(H.title, ucoo{2 * n - 1, 2 * n - 1});
            tree->put(mid * 2, WHITE);
            tree->show();
        } else {
            show();
        }
    };

    void run(const Hub &H) {
        coo z{0, 0};
        while (true) {
            auto nz = z;
            if (at(mid + nz)) {
                nz += dz[prng.uniform_int(4U)];
                if (!fits(nz + mid)) break;
            }
            if (!at(mid + nz)) {
                if (H['g']) {
                    // TODO: fix appearances of at(ucoo(...))
                    Color c = tree->at((mid + z) * 2);
                    if ((c == BLACK) || (prng.bernoulli(H['u']))) c = HSV(prng.uniform_real(), 1, 1);
                    if (H['i'] && (norm(nz) < 10)) c = nz.y > 0 ? Indexed(0) : Indexed(1);
                    tree->put((mid + nz) * 2, c);
                    tree->put((mid * 2 + z + nz), c);
                }
                put(mid + nz, true);
                nz += jump(nz);
            }
            z = nz;
        }
    }

    function<coo(coo)> jump;
    unique_ptr<Image>  tree;
    ucoo               mid;
};

auto main(int argc, char **argv) -> int {
    Hub const H("Internal DLA", argc, argv, "n=1000,j=idla,p=.1,a=2,b=1,g,u=0,i");
    Bouncy B(H, H['n'], H['j']);
    B.run(H);
    if (H['g'])
        B.tree->output(H.title);
    else
        B.output(H.title);
}
