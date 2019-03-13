#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

enum Type { VOID, SITE, EDGE, DUAL, DEDG, EMPH };

class Point {
public:
    Point() = default;
    explicit Point(Type t_, int d_ = 0) : t(t_), d(d_) {}
    explicit operator Color() {
        if (t == SITE) return ((d == 0) || (d == 1)) ? RED : Color(200, 200, 255);
        if (t == EDGE) return ((d == 0) || (d == 1)) ? RED : Color(200, 200, 255);
        if (t == DUAL) return ((d == 2) || (d == 3)) ? Color(200, 100, 0) : Color(128, 255, 128);
        if (t == DEDG) return ((d == 2) || (d == 3)) ? Color(200, 100, 0) : Color(128, 255, 128);

        if (t == EMPH) return BLACK;
        if (d < 0) return WHITE;
        return Indexed(d);
    }

    Type t = VOID;
    int  d = -1;
};

class SF : public Bitmap<Point> {
public:
    SF(const Hub &H, size_t n_, double a_)
        : Bitmap<Point>(H.title, {2 * n_, 2 * n_ + 1}), n(n_), a(a_), root({2 * n - 1, 2 * (n / 2)}), start({1, 2 * (n / 4)}) {
        ps = {a * a, a * a, 1, 1};
        for (auto &p : ps) p /= 2 * (1 + a * a);
    }

    void stage(const Hub &H) {
        if (H['s']) snapshot();
        if (H['v']) pause();
    }

    void path(coo z, Type t = EMPH) {
        while (contains(z) && (at(z).t != t)) {
            int d         = at(z).d;
            at(z).t       = t;
            at(z + dz[d]) = Point{t, d};
            z += dz[d] * 2;
        }
    }

    void lerw(coo z0, bool killed = false) {
        coo z = z0;
        while (at(z).t != SITE) {
            auto d  = prng.discrete(ps);
            at(z).d = int(d);
            step();
            if (contains(z + dz[d] * 2)) {
                z += dz[d] * 2;
                continue;
            }
            if (killed && (d == 2)) break;
            if (d == 2) z = z0;
        }
        path(z0, SITE);
    }

    void dual() {
        bool dirty = true;
        while (dirty) {
            dirty = false;
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j) {
                    coo z{2 * i, 1 + 2 * j};
                    int s = 0, dd = -1;
                    for (int d = 0; d < 4; ++d)
                        if (at(z + dz[d]).t == VOID) {
                            ++s;
                            dd = d;
                        }
                    if (s == 1) {
                        at(z)          = Point{DUAL, dd};
                        at(z + dz[dd]) = Point{DEDG, dd};
                        dirty          = true;
                    }
                }
        }
    }

    void special(const Hub &H) {
        coo    delta{root - start};
        double lambda = double(delta.y) / double(delta.x), L = lambda * lambda;
        double tca = a + 1 / a, Delta = 1 + L * L + L * (tca * tca - 2);
        double u = (tca - sqrt(Delta)) / (1 - L), mu = copysign(acosh(u), delta.x);
        double v = (-tca * L + sqrt(Delta)) / (1 - L), nu = copysign(acosh(v), delta.y);

        vector<double> cps{exp(mu), exp(nu), exp(-mu), exp(-nu)};
        double         s = 0;
        for (auto p : cps) s += p;
        for (auto &p : cps) p /= s;

        int nw = 0;
        coo z  = start;
        while (true) {
            auto d  = prng.discrete(cps);
            at(z).d = int(d);
            z += dz[d] * 2;
            step();
            if (z == root) break;
            if ((z.x <= start.x) || (!contains(z))) { z = start; }
            if (z.x == root.x) {
                nw++;
                if (nw == 1) stage(H);
                z = start;
            }
        }
        stage(H);
        path(start, SITE);
    }

    void go(const Hub &H) {
        put(root, Point{SITE});
        if (H['v']) show();
        if (a < 1)
            special(H);
        else
            lerw(start, false);
        stage(H);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= n; ++j) lerw(coo{2 * i + 1, 2 * j}, true);
        }
        stage(H);
        put(root, Point{EMPH});
        path(start);
        stage(H);
        dual();
        stage(H);
        output(H.title);
    }

    int            n;
    double         a;
    vector<double> ps;
    coo            root, start;
};

int main(int argc, char **argv) {
    Hub H("Spanning forest with 2-periodic weights", argc, argv, "n=400,a=.2,v,s");
    SF(H, H['n'], H['a']).go(H);
}
