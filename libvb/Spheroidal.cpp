#include <cassert>
#include <vb/Figure.h>
#include <vb/Spheroidal.h>

namespace vb {
    Spheroidal::Spheroidal(Hypermap M) : Hypermap(std::move(M)) {
        assert(genus() == 0);
        from_hypermap();
    }

    void Spheroidal::pack() {
        for (auto &v : V) v.fixed = false;
        for (auto f : phi.cycles()) {
            if (f.size() != 3) continue;
            auto bad = std::any_of(begin(f), end(f), [&](auto i) { return V[E[i].src].adj.size() == 2; });
            if (bad && (phi.cycles().size() > 2)) continue;
            V[E[f[0]].src].fixed = true;
            E[f[0]].a            = M_PI / 3;
            E[sigma[f[0]]].a     = 0;
            V[E[f[1]].src].fixed = true;
            E[f[1]].a            = -M_PI / 3;
            E[sigma[f[1]]].a     = -2 * M_PI / 3;
            V[E[f[2]].src].fixed = true;
            E[f[2]].a            = M_PI;
            E[sigma[f[2]]].a     = 2 * M_PI / 3;
            V[E[f[0]].src].z     = 0;
            break;
        }

        acpa();

        auto ne   = sigma.size();
        bool flag = true;
        while (flag) {
            flag = false;
            for (size_t i = 0; i < ne; ++i) {
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

        flag = true;
        while (flag) {
            flag = false;
            for (size_t e = 0; e < ne; ++e) {
                auto i = E[e].src;
                if (std::isnan(real(V[i].z))) continue;
                auto   j = E[alpha[e]].src;
                double const l = V[i].r + V[j].r;
                cpx const    z = V[i].z + std::polar(l, E[e].a);
                if (std::isnan(real(V[j].z))) {
                    flag                 = true;
                    V[E[alpha[e]].src].z = z;
                }
            }
        }
    }

    void Spheroidal::linear(cpx a, cpx b) {
        for (auto &v : V) {
            v.z = a * v.z + b;
            v.r *= abs(a);
        }
        for (auto &e : E) { e.a += arg(a); }
    }

    void Spheroidal::inversion() {
        for (auto &v : V) {
            if (v.z == 0.0) {
                v.r = -1 / v.r;
                continue;
            }
            double r1 = 1 / (abs(v.z) - v.r), r2 = 1 / (abs(v.z) + v.r);
            double nr = (r1 - r2) / 2, nz = (r1 + r2) / 2;
            v.z *= nz / abs(v.z);
            v.r = nr;
        }
        for (size_t i = 0; i < sigma.size(); ++i) {
            E[i].a = arg(V[E[alpha[i]].src].z - V[E[i].src].z);
            if (V[E[alpha[i]].src].r < 0) E[i].a += M_PI;
        }
    }

    void Spheroidal::mobiusto0(cpx a) {
        if (a == 0.0) return;
        cpx const abar = conj(a);
        for (auto &v : V) {
            const cpx    z1 = 1.0 - abar * v.z;
            const double r1 = norm(z1) - v.r * v.r * norm(a);
            v.z             = ((1 - norm(a)) * z1 / r1 - 1.0) / abar;
            v.r *= (1 - norm(a)) / r1;
        }
        for (size_t i = 0; i < sigma.size(); ++i) {
            E[i].a = arg(V[E[alpha[i]].src].z - V[E[i].src].z);
            if (V[E[alpha[i]].src].r < 0) E[i].a += M_PI;
        }
    }

    void Spheroidal::output_pdf(const std::string &s, unsigned mode) {
        for (size_t e = 0; e < sigma.size(); ++e) {
            if (initial[e] == 0) continue;
            V[E[e].src].bone = std::max(V[E[e].src].bone, initial[e]);
        }

        Figure           F(s);
        std::vector<cpx> eee;
        Cycles           sc = sigma.cycles();

        for (auto &v : V) {
            cpx const z = v.z;
            if ((((mode & 1U) != 0) && (v.bone != 0)) || (((mode & 2U) != 0) && (v.bone == 0)))
                F.add(std::make_unique<Circle>(z, fabs(v.r), Pen(BLACK, .3)));
            for (auto e : sc[v.i]) {
                if ((((mode & 4U) != 0) && ((initial[e] & 1U) != 0)) || (((mode & 8U) != 0) && ((v.bone & 1U) != 0)) ||
                    (((mode & 16U) != 0) && ((v.bone & 1U) == 0))) {
                    eee.emplace_back(z);
                    eee.emplace_back(z + std::polar(v.r, E[e].a));
                    eee.emplace_back(NAN);
                }
            }
        }

        if (!eee.empty()) F.add(std::make_unique<Path>(eee, Pen(BLACK, .5)));

        for (const auto &v : V) {
            if (v.r < 0) continue;
            cpx const z = v.z;
            if (((mode & 32U) != 0) && ((v.bone & 2U) != 0)) F.add(std::make_unique<Circle>(z, .01, Pen(BLACK, 2, BLACK, true)));
            if (((mode & 64U) != 0) && ((v.bone & 4U) != 0)) F.add(std::make_unique<Circle>(z, .01, Pen(BLACK, 2, WHITE, true)));
            if (((mode & 128U) != 0) && ((v.bone & 8U) != 0)) F.add(std::make_unique<Circle>(z, .01, Pen(BLACK, 2, RED, true)));
            if (((mode & 256U) != 0) && ((v.bone & 8U) != 0)) {
                std::vector<cpx> ast;
                for (int i = 0; i < 3; ++i) {
                    ast.emplace_back(z + std::polar(.013, i * M_PI / 3));
                    ast.emplace_back(z + std::polar(.013, (i + 3) * M_PI / 3));
                    ast.emplace_back(NAN);
                }
                F.add(std::make_unique<Path>(ast, Pen(BLACK, 1)));
            }
        }

        F.output_pdf(s);
    }
} // namespace vb
