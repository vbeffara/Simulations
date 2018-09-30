#include <vb/Figure.h>
#include <vb/Toroidal.h>
#include <cassert>

namespace vb {
    Toroidal::Toroidal(Hypermap M) : Hypermap(std::move(M)), m(I) {
        assert(genus() == 1);
        from_hypermap();
        mode  = H['m'];
        title = H.title;
    }

    void Toroidal::pack() {
        acpa();

        E[0].a    = 0;
        int  ne   = sigma.size();
        bool flag = true;
        while (flag) {
            flag = false;
            for (int i = 0; i < ne; ++i) {
                if (std::isnan(E[i].a)) continue;
                if (std::isnan(E[alpha[i]].a)) {
                    E[alpha[i]].a = E[i].a + M_PI;
                    flag          = true;
                }
                if (std::isnan(E[sigma[i]].a)) {
                    double x = V[E[i].src].r, y = V[E[alpha[i]].src].r, z = V[E[alpha[sigma[i]]].src].r;
                    E[sigma[i]].a = E[i].a + alpha_xyz(x, y, z);
                    flag          = true;
                }
            }
        }

        V[0].z = 0;
        std::vector<cpx> periods;
        flag = true;
        while (flag) {
            flag = false;
            periods.clear();
            for (int e = 0; e < ne; ++e) {
                int i = E[e].src;
                if (std::isnan(real(V[i].z))) continue;
                int    j = E[alpha[e]].src;
                double l = V[i].r + V[j].r;
                cpx    z = V[i].z + std::polar(l, E[e].a);
                if (std::isnan(real(V[j].z))) {
                    flag                 = true;
                    V[E[alpha[e]].src].z = z;
                } else if (abs(V[j].z - z) > V[0].r / 2) {
                    periods.push_back(V[j].z - z);
                }
            }
        }

        cpx p1 = periods[0];
        for (cpx p : periods)
            if (abs(p) < abs(p1)) { p1 = p; }

        for (auto & v : V) {
            v.z /= p1;
            v.r /= abs(p1);
        }
        for (auto & e : E) { e.a -= arg(p1); }
        for (cpx & p : periods) p /= p1;

        std::vector<cpx> moduli;
        for (cpx p : periods)
            if (fabs(imag(p)) > .1) moduli.push_back(p);
        double n2 = abs(moduli[0]);
        m         = moduli[0];
        for (cpx p : moduli)
            if (abs(p) < n2) {
                n2 = abs(p);
                m  = p;
            }
        if (imag(m) < 0) m = -m;
        while (real(m) < -.499) m += 1;
        while (real(m) > .501) m -= 1;

        int mdeg = 0;
        cpx mpos = 0;
        for (const auto & v : V)
            if (v.adj.size() > mdeg) {
                mdeg = v.adj.size();
                mpos = v.z;
            }

        double slope = real(m) / imag(m);
        for (auto & v : V) {
            v.z -= mpos;
            while (imag(v.z) < 0) v.z += m;
            while (imag(v.z) > imag(m)) v.z -= m;
            while (real(v.z) < slope * imag(v.z)) v.z += 1;
            while (real(v.z) > slope * imag(v.z) + 1) v.z -= 1;
        }
    }

    void Toroidal::flip() {
        for (auto & e : E) e.a += M_PI;
        for (auto & v : V) v.z = 1.0 + m - v.z;
    }

    void Toroidal::output_pdf() {
        for (int e = 0; e < sigma.size(); ++e) {
            if (initial[e] == 0) continue;
            V[E[e].src].bone = std::max(V[E[e].src].bone, initial[e]);
        }

        Figure           F;
        std::vector<cpx> eee;
        Cycles           sc = sigma.cycles();

        for (int a = -2; a < 3; ++a) {
            for (int b = -1; b < 2; ++b) {
                for (auto & v : V) {
                    cpx z = v.z + cpx(a) + cpx(b) * m;
                    if ((imag(z) < -.6) || (imag(z) > 1.7 * std::max(1.0, imag(m))) || (real(z) < -.8) || (real(z) > 2.6)) continue;
                    if ((((mode & 1u) != 0) && (v.bone != 0)) || (((mode & 2u) != 0) && (v.bone == 0)))
                        F.add(std::make_unique<Circle>(z, v.r, Pen(BLACK, .3)));
                    for (int e : sc[v.i]) {
                        if ((((mode & 4u) != 0) && ((initial[e] & 1u) != 0)) || (((mode & 8u) != 0) && ((v.bone & 1u) != 0)) ||
                            (((mode & 16u) != 0) && ((v.bone & 1u) == 0))) {
                            eee.emplace_back(z);
                            eee.emplace_back(z + std::polar(v.r, E[e].a));
                            eee.emplace_back(NAN);
                        }
                    }
                }
            }
        }

        if (!eee.empty()) F.add(std::make_unique<Path>(eee, Pen(BLACK, .5)));

        for (int a = -2; a < 3; ++a) {
            for (int b = -1; b < 2; ++b) {
                for (const auto & v : V) {
                    cpx z = v.z + cpx(a) + cpx(b) * m;
                    if ((imag(z) < -.6) || (imag(z) > 1.7 * std::max(1.0, imag(m))) || (real(z) < -.8) || (real(z) > 2.6)) continue;
                    if (((mode & 32u) != 0) && ((v.bone & 2u) != 0)) F.add(std::make_unique<Circle>(z, .015, Pen(BLACK, .5, BLACK, true)));
                    if (((mode & 64u) != 0) && ((v.bone & 4u) != 0)) F.add(std::make_unique<Circle>(z, .01, Pen(BLACK, 2, WHITE, true)));
                    if (((mode & 128u) != 0) && ((v.bone & 8u) != 0)) F.add(std::make_unique<Circle>(z, .01, Pen(BLACK, 2, RED, true)));
                    if (((mode & 256u) != 0) && ((v.bone & 8u) != 0)) {
                        std::vector<cpx> ast;
                        for (int i = 0; i < 3; ++i) {
                            ast.emplace_back(z + std::polar(.013, i * M_PI / 3));
                            ast.emplace_back(z + std::polar(.013, (i + 3) * M_PI / 3));
                            ast.emplace_back(NAN);
                        }
                        F.add(std::make_unique<Path>(ast, Pen(BLACK, 1)));
                    }
                }
            }
        }

        F.add(std::make_unique<Polygon>(std::vector<cpx>{0, 1, cpx(1) + m, m}, Pen(BLACK, 0, Color(0, 0, 0, 50), true)));
        F.output_pdf();
    }
} // namespace vb
