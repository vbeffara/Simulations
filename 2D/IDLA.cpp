#include <vb/CoarseImage.h>
#include <vb/math/math.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <map>

using namespace vb;
using namespace std;

auto make_bounces(double p, int a, double b) -> map<string, function<coo(coo)>> {
    map<string, function<coo(coo)>> bounces;
    bounces.emplace("none", [](coo /*unused*/) { return coo{0, 0}; });
    bounces.emplace("line", [p](coo z) { return coo{int(-p * double(z.x)), int(-p * double(z.y))}; });
    bounces.emplace("idla", [](coo z) { return -z; });
    bounces.emplace("sign", [](coo z) { return coo{-sign(z.x), -sign(z.y)}; });
    bounces.emplace("sig2", [a](coo z) {
        int64_t k = sign(z.x * z.y);
        return coo{-sign(z.x), -sign(z.y)} * (1 + (a - 1) * (1 + k) / 2);
    });
    bounces.emplace("soft", [a](coo z) {
        int const k = z.x > 2 * abs(z.y) ? a : 1;
        return coo{-sign(z.x), -sign(z.y)} * k;
    });
    bounces.emplace("sqrt", [](coo z) {
        double const l = 1 + sqrt(norm(z));
        return coo{int(-double(z.x) / sqrt(l)), int(-double(z.y) / sqrt(l))};
    });
    bounces.emplace("maxi", [](coo z) { return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -sign(z.y)}; });
    bounces.emplace("max2", [a](coo z) { return (abs(z.x) > abs(z.y)) ? coo{-sign(z.x), 0} : coo{0, -a * sign(z.y)}; });
    bounces.emplace("maxb", [b](coo z) {
        double const l = pow(norm(z), b);
        return (abs(z.x) > abs(z.y)) ? coo{int(-l * sign(double(z.x))), 0} : coo{0, int(-l * sign(double(z.y)))};
    });
    bounces.emplace("dent", [](coo z) {
        if (z.x + z.y > 2 * abs(z.x - z.y)) return (z.y > z.x) ? coo{-1, 0} : coo{0, -1};
        return (abs(z.y) > abs(z.x)) ? coo{0, -sign(z.y)} : coo{-sign(z.x), 0};
    });
    bounces.emplace("octo", [a, b](coo z) {
        if (abs(z.x) > 2 * abs(z.y)) return coo{int(-a * sign(double(z.x))), 0};
        if (abs(z.y) > 2 * abs(z.x)) return coo{0, int(-a * sign(double(z.y)))};
        return coo{int(-b * sign(double(z.x))), int(-b * sign(double(z.y)))};
    });
    return bounces;
}

class Bouncy : public CoarseImage {
public:
    Bouncy(const string &title, size_t n, function<coo(coo)> j, bool g)
        : CoarseImage(title, {n, n}, size_t(pow(n, .25))), jump(std::move(j)), mid(size / 2) {
        if (g) {
            tree = make_unique<Image>(title, ucoo{2 * n - 1, 2 * n - 1});
            tree->put(mid * 2, WHITE);
            tree->show();
        } else {
            show();
        }
    };

    void run(bool g, double u, bool initial) {
        coo z{0, 0};
        while (true) {
            auto nz = z;
            if (at(mid + nz)) {
                nz += dz[prng.uniform_int(4U)];
                if (!fits(nz + mid)) break;
            }
            if (!at(mid + nz)) {
                if (g) {
                    Color c = tree->at((mid + z) * 2);
                    if ((c == BLACK) || (prng.bernoulli(u))) c = HSV(prng.uniform_real(), 1, 1);
                    if (initial && (norm(nz) < 10)) c = nz.y > 0 ? Indexed(0) : Indexed(1);
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
    CLP  clp(argc, argv, "Internal DLA");
    auto n       = clp.param("n", size_t(1000), "Grid size");
    auto j       = clp.param("j", string("idla"), "Jump type");
    auto p       = clp.param("p", 0.1, "Line parameter");
    auto a       = clp.param("a", 2, "A parameter");
    auto b       = clp.param("b", 1.0, "B parameter");
    auto g       = clp.flag("g", "Show tree");
    auto u       = clp.param("u", 0.0, "Color mutation rate");
    auto initial = clp.flag("i", "Initial coloring");
    clp.finalize();

    auto   bounces = make_bounces(p, a, b);
    Bouncy B(clp.title, n, bounces.at(j), g);
    B.run(g, u, initial);
    if (g)
        B.tree->output(clp.title);
    else
        B.output(clp.title);
}
