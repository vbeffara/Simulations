#include <set>
#include <vb/data/Array.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using cooo = std::pair<int, double>;
using ptt  = std::pair<cooo, cooo>;

class pt : public ptt {
public:
    pt(int i, int j, double x, double y) : ptt(cooo(i, x), cooo(j, y)){};
    pt() : pt(-1, -1, -1, -1){};

    int &   xi() { return first.first; }
    double &xf() { return first.second; }
    double  x() const { return static_cast<double>(first.first) + first.second; }
    int &   yi() { return second.first; }
    double &yf() { return second.second; }
    double  y() const { return static_cast<double>(second.first) + second.second; }

    double dist2(const pt &o) const {
        double dx = x() - o.x(), dy = y() - o.y();
        return dx * dx + dy * dy;
    }

    void step(const vb::Array<double> &o) {
        int    r  = 0;
        double oo = o.at({xi(), yi()});

        if (yf() == 1) {
            r    = 1;
            oo   = -1 / oo;
            yf() = xf();
            xf() = 0;
        } else if (xf() == 1) {
            r    = 2;
            yf() = 1 - yf();
            xf() = 0;
        } else if (yf() == 0) {
            r    = 3;
            oo   = -1 / oo;
            yf() = 1 - xf();
            xf() = 0;
        }

        int di = 0, dj = 0;

        if (oo < -yf()) {
            xf() = -yf() / oo;
            yf() = 0;
            dj   = -1;
        } else if (oo < 1 - yf()) {
            xf() = 1;
            yf() = yf() + oo;
            di   = 1;
        } else {
            xf() = (1 - yf()) / oo;
            yf() = 1;
            dj   = 1;
        }

        if (r == 1) {
            int ti    = di;
            di        = dj;
            dj        = -ti;
            double tx = xf();
            xf()      = yf();
            yf()      = 1 - tx;
        } else if (r == 2) {
            di   = -di;
            dj   = -dj;
            xf() = 1 - xf();
            yf() = 1 - yf();
        } else if (r == 3) {
            int ti    = di;
            di        = -dj;
            dj        = ti;
            double tx = xf();
            xf()      = 1 - yf();
            yf()      = tx;
        }

        xf() -= di;
        xi() += di;
        yf() -= dj;
        yi() += dj;
    }

    void reverse() {
        if (xf() == 0) {
            xf() = 1;
            xi()--;
        } else if (xf() == 1) {
            xf() = 0;
            xi()++;
        } else if (yf() == 0) {
            yf() = 1;
            yi()--;
        } else if (yf() == 1) {
            yf() = 0;
            yi()++;
        }
    }
};

std::ostream &operator<<(std::ostream &os, const pt p) { return os << p.x() << " " << p.y() << std::endl; }

using ptpair = std::pair<pt, pt>;

class Lamination : public vb::Array<double> {
public:
    explicit Lamination(size_t n) : vb::Array<double>({n, n}, 0) {
        for (auto z : vb::coo_range(size)) { put(z, tan(vb::prng.uniform_real(0, M_PI))); }
    }

    pt geodesique(pt p, std::ostream *os = nullptr) const {
        std::set<pt> S;
        while (true) {
            if (!contains({p.xi(), p.yi()})) { break; }
            if (os != nullptr) { (*os) << p; }
            p.step(*this);
            if (S.count(p) != 0) { break; }
            S.insert(p);
        }
        if (os != nullptr) { (*os) << std::endl; }

        if (!contains({p.xi(), p.yi()})) { return pt(); }

        pt p_min = p;
        p.step(*this);
        while (p != p_min) {
            if (p < p_min) { p_min = p; }
            p.step(*this);
        }

        return p_min;
    }

    std::pair<pt, pt> leaf(pt p, std::ostream *os = nullptr) const {
        pt p1 = geodesique(p, os);
        p.reverse();
        pt p2 = geodesique(p, os);
        return (p1 < p2) ? ptpair{p1, p2} : ptpair{p2, p1};
    }

    std::set<pt> connections() const {
        std::set<ptpair> S;
        std::set<pt>     P;

        for (int i = 0; i < size.x; ++i) {
            for (int j = 0; j < size.y; ++j) {
                for (int xx = 1; xx < 100; ++xx) {
                    double            x  = .01 * xx;
                    std::pair<pt, pt> pp = leaf(pt(i, j, x, 0), nullptr);

                    if ((pp.first != pt()) && (pp.second != pt())) {
                        if (S.count(pp) == 0) {
                            S.insert(pp);
                            P.insert(pt(i, j, x, 0));
                        }
                    }

                    pp = leaf(pt(i, j, 0, x), nullptr);

                    if ((pp.first != pt()) && (pp.second != pt())) {
                        if (S.count(pp) == 0) {
                            S.insert(pp);
                            P.insert(pt(i, j, 0, x));
                        }
                    }
                }
            }
        }
        return P;
    }
};

int main(int argc, char **argv) {
    vb::Hub    H("Random lamination", argc, argv, "n=20,c");
    Lamination o(H['n']);

    for (int i = 0; i < o.size.x; ++i) {
        for (int j = 0; j < o.size.y; ++j) {
            o.geodesique(pt(i, j, 0, 0), &std::cout);
            o.geodesique(pt(i, j, 1, 0), &std::cout);
            o.geodesique(pt(i, j, 0, 1), &std::cout);
            o.geodesique(pt(i, j, 1, 1), &std::cout);
        }
    }

    if (H['c']) {
        std::set<pt> P = o.connections();
        std::set<pt> E;
        for (pt i : P) {
            auto pp = o.leaf(i, nullptr);
            E.insert(pp.first);
            E.insert(pp.second);
        }
        for (pt i : E) { o.geodesique(i, &std::cerr); }
    }
}
