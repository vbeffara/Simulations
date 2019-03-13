#include <vb/CoarseImage.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Bounces : public map<string, function<coo(coo)>> {
public:
    explicit Bounces(const Hub &H) {
        emplace("none", [&](coo) { return coo{0, 0}; });
        emplace("line", [&](coo z) {
            double p = H['p'];
            return coo{int(-p * z.x), int(-p * z.y)};
        });
        emplace("idla", [&](coo z) { return -z; });
        emplace("sign", [&](coo z) { return coo{-sign(z.x), -sign(z.y)}; });
        emplace("sig2", [&](coo z) {
            int64_t a = H['a'], k = sign(z.x * z.y);
            return coo{-sign(z.x), -sign(z.y)} * (1 + (a - 1) * (1 + k) / 2);
        });
        emplace("soft", [&](coo z) {
            int k = z.x > 2 * abs(z.y) ? H['a'] : 1;
            return coo{-sign(z.x), -sign(z.y)} * k;
        });
        emplace("sqrt", [&](coo z) {
            double l = 1 + sqrt(norm(z));
            return coo{int(-z.x / sqrt(l)), int(-z.y / sqrt(l))};
        });

        emplace("maxi", [&](coo z) { return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -sign(z.y)}; });
        emplace("max2", [&](coo z) {
            int a = H['a'];
            return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -a * sign(z.y)};
        });
        emplace("maxb", [&](coo z) {
            double l = pow(norm(z), double(H['b']));
            return (abs(z.x) > abs(z.y)) ? coo{int(-l * sign(z.x)), 0} : coo{0, int(-l * sign(z.y))};
        });

        emplace("dent", [&](coo z) {
            if (z.x + z.y > 2 * abs(z.x - z.y)) return (z.y > z.x) ? coo{-1, 0} : coo{0, -1};
            /* else */ return (abs(z.y) > abs(z.x)) ? coo{0, -sign(z.y)} : coo{-sign(z.x), 0};
        });

        emplace("octo", [&](coo z) {
            double a = H['a'], b = H['b'];
            if (abs(z.x) > 2 * abs(z.y)) return coo{int(-a * sign(z.x)), 0};
            if (abs(z.y) > 2 * abs(z.x)) return coo{0, int(-a * sign(z.y))};
            return coo{int(-b * sign(z.x)), int(-b * sign(z.y))};
        });
    }
};

class Bouncy : public CoarseImage {
public:
    Bouncy(const Hub &H, size_t n, const string &j) : CoarseImage(H.title, {n, n}, size_t(pow(n, .25))), jump(Bounces(H).at(j)) {
        z0 = {int(n) / 2, int(n) / 2};
        if (H['g']) {
            tree = make_unique<Image>(H.title, ucoo{2 * n - 1, 2 * n - 1});
            tree->put(z0 * 2, WHITE);
            tree->show();
        } else {
            show();
        }
    };

    void run(const Hub &H) {
        coo z{0, 0};
        while (true) {
            coo nz = z;
            if (at(nz)) {
                nz += dz[prng.uniform_int(4)];
                if (!contains(nz)) break;
            }
            if (!at(nz)) {
                if (H['g']) {
                    Color c = tree->at((z + z0) * 2);
                    if ((c == BLACK) || (prng.bernoulli(H['u']))) c = HSV(prng.uniform_real(), 1, 1);
                    if (H['i'] && (norm(nz) < 10)) c = nz.y > 0 ? Indexed(0) : Indexed(1);
                    tree->put((nz + z0) * 2, c);
                    tree->put(z + nz + z0 * 2, c);
                }
                put(nz, true);
                nz += jump(nz);
            }
            z = nz;
        }
    }

    function<coo(coo)> jump;
    unique_ptr<Image>  tree;
};

int main(int argc, char **argv) {
    Hub    H("Internal DLA", argc, argv, "n=1000,j=idla,p=.1,a=2,b=1,g,u=0,i");
    Bouncy B(H, H['n'], H['j']);
    B.run(H);
    if (H['g'])
        B.tree->output(H.title);
    else
        B.output(H.title);
}
