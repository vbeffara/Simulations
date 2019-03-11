#pragma once
#include <vb/Constellation0.h>
#include <vb/Spheroidal.h>
#include <vb/math/NumberTheory.h>

namespace vb {
    template <typename T> Constellation0<T>::Constellation0(const Hypermap &M) {
        Hypermap M2(M);
        M2.dessin();
        p = {T(1)};
        Hypermap M3(M2);
        M3.normalize();
        while (true) {
            M2.split_edges();
            Spheroidal S(M2);
            S.pack();
            size_t N = M.sigma.size(), inf = 0, dinf = 0;
            for (auto c : M.phi.cycles()) {
                size_t i = S.E[c[0] + 3 * N].src, d = S.V[i].adj.size();
                if (d > dinf) {
                    inf  = i;
                    dinf = d;
                }
            }
            S.linear(1, -S.V[inf].z);
            S.inversion();
            S.linear(-1 / S.V[inf].r, 0);
            S.mobiusto0(S.V[S.E[0].src].z);
            b.clear();
            for (auto c : M.sigma.cycles()) {
                const auto &z = S.V[S.E[c[0]].src].z;
                b.push_back({cplx(real(z), imag(z)), c.size()});
            }
            w.clear();
            for (auto c : M.alpha.cycles()) {
                const auto &z = S.V[S.E[c[0] + N].src].z;
                w.push_back({cplx(real(z), imag(z)), c.size()});
            }
            f.clear();
            for (auto c : M.phi.cycles()) {
                if (S.E[c[0] + 3 * N].src != inf) {
                    const auto &z = S.V[S.E[c[0] + 3 * N].src].z;
                    f.push_back({cplx(real(z), imag(z)), c.size()});
                }
            }
            dim = b.size() + w.size() + f.size();
            make_c_0();
            if (findn() > T(1e-6)) continue;
            if (auto M4 = explore()) {
                M4->normalize();
                if (M3 == M4) break;
            }
        }
    } // namespace vb

    template <typename T> template <typename U> Constellation0<T>::Constellation0(const Constellation0<U> &C) : Constellation<T>(C) {
        dim--;
    }

    template <typename T> auto Constellation0<T>::operator()(cplx z) const -> cplx {
        cplx out(p[0]);
        for (auto zd : b) out *= pow(z - zd.z, zd.d);
        for (auto zd : f) out /= pow(z - zd.z, zd.d);
        return out;
    }

    template <typename T> void Constellation0<T>::normalize() {
        p[0] = cplx(1);
        cplx avg(0), d(0);
        for (auto zd : w) {
            d += zd.d;
            avg += (*this)(zd.z) * cplx(zd.d);
        }
        p[0] = d / avg;
    }

    template <typename T> void Constellation0<T>::linear(cplx u, cplx v) {
        for (auto &zd : b) zd.z = u * zd.z + v;
        for (auto &zd : f) zd.z = u * zd.z + v;
        for (auto &zd : w) zd.z = u * zd.z + v;
    }

    template <typename T> void Constellation0<T>::make_l_1() {
        normalize();
        int deg = 0;
        for (auto zd : b) deg += zd.d;
        for (auto zd : f) deg -= zd.d;
        linear(pow(p[0], cplx(T(1) / T(deg))));
        p[0] = T(1);
    }

    template <typename T> void Constellation0<T>::make_c_0() {
        int  deg = 0;
        cplx sum(0);
        for (auto zd : b) {
            deg += zd.d;
            sum += T(zd.d) * zd.z;
        }
        sum /= deg;
        linear(T(1), -sum);
        normalize();
    }

    template <typename T> void Constellation0<T>::make_p_1() {
        T eps = real(pow(cost(), T(.5)));
        if (eps > T(.1)) eps = T(.1);
        Polynomial<cplx> P{1}, Q{1};
        for (auto zd : b) P.add_root(zd.z, zd.d);
        for (auto zd : f) Q.add_root(zd.z, zd.d);
        unsigned i = 0, j = 0;
        while (norm(P[i]) < eps) ++i;
        while (norm(Q[j]) < eps) ++j;
        bool is_P;

        if (Q.degree() == 0)
            is_P = true;
        else if (i == j)
            is_P = (norm(P[i]) > norm(Q[i]));
        else
            is_P = (i < j);

        cplx l(1);
        if ((is_P) && (i < P.degree())) l = pow(P[i], cplx(T(1) / T(P.degree() - i)));
        if ((!is_P) && (j < Q.degree())) l = pow(Q[j], cplx(T(1) / T(Q.degree() - j)));
        if ((l != cplx(0)) && (norm(l) > eps)) {
            linear(T(1) / l);
            normalize();
        }
        if (norm(l - cplx(1)) > eps) make_p_1();
    }

    template <typename T> void Constellation0<T>::belyi() {
        make_c_0();
        make_l_1();
        normalize();
        make_p_1();
    }

    template <typename T> auto Constellation0<T>::logder(cplx z, unsigned k) const -> cplx {
        if (k == 0) return log((*this)(z));
        cplx sum(0);
        for (auto zd : b) sum += cplx(zd.d) / pow(z - zd.z, k);
        for (auto zd : f) sum -= cplx(zd.d) / pow(z - zd.z, k);
        return sum;
    }

    template <typename T> void Constellation0<T>::readvec(const Vector<cplx> &xy) {
        int i = 0;
        for (auto &zd : b) zd.z = xy[i++];
        for (auto &zd : w) zd.z = xy[i++];
        for (auto &zd : f) zd.z = xy[i++];
    }

    template <typename T> auto Constellation0<T>::vec() const -> Vector<cplx> {
        Vector<cplx> bw(dim);
        int          i = 0;
        for (auto zd : b) { bw[i++] = zd.z; }
        for (auto zd : w) { bw[i++] = zd.z; }
        for (auto zd : f) { bw[i++] = zd.z; }
        return bw;
    }

    template <typename T> auto Constellation0<T>::vcost() const -> Vector<cplx> {
        Vector<cplx> out(dim);
        int          k = 0;
        for (auto zd : w)
            for (unsigned j = 0; j < zd.d; ++j) out[k++] = logder(zd.z, j);
        cplx sb(0);
        for (auto zd : b) sb += T(zd.d) * zd.z;
        out[k++] = sb;
        return out;
    }

    template <typename T> auto Constellation0<T>::jacvcost() const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
        Matrix<cplx>           out(dim, dim);
        unsigned               i = 0, j = 0;
        for (unsigned ii = 0; ii < w.size(); ++ii)
            for (unsigned id = 0; id < w[ii].d; ++id) {
                j = 0;
                for (unsigned jj = 0; jj < b.size(); ++jj)
                    if (id == 0)
                        out(i, j++) = T(-T(b[jj].d)) / (w[ii].z - b[jj].z);
                    else
                        out(i, j++) = T(id * b[jj].d) / pow(w[ii].z - b[jj].z, cplx(id + 1));
                for (unsigned jj = 0; jj < w.size(); ++jj)
                    if (jj != ii)
                        out(i, j++) = T(0);
                    else if (id == 0)
                        out(i, j++) = logder(w[ii].z, 1);
                    else
                        out(i, j++) = T(-T(id)) * logder(w[ii].z, id + 1);
                for (unsigned jj = 0; jj < f.size(); ++jj)
                    if (id == 0)
                        out(i, j++) = T(f[jj].d) / (w[ii].z - f[jj].z);
                    else
                        out(i, j++) = T(-T(id * f[jj].d)) / pow(w[ii].z - f[jj].z, cplx(id + 1));
                ++i;
            }
        j = 0;
        for (unsigned jj = 0; jj < b.size(); ++jj) out(i, j++) = T(b[jj].d);
        while (j < dim) out(i, j++) = T(0);
        ++i;
        return out;
    }

    template <typename T> std::ostream &operator<<(std::ostream &os, const Constellation0<T> &C) {
        T   err(C.cost());
        T   lerr(-log10(err));
        int nd = std::max(5, int(lerr) / 2 - 7);
        if (err == T(0)) nd = 10;
        os << std::setprecision(nd) << std::fixed;
        os << "Keeping " << nd << " digits."
           << "\n";

        os << "Black vertices / zeros: "
           << "\n";
        for (const auto &u : C.b) os << "| " << u.d << "\t" << u.z << "\n";
        os << "\n";
        os << "White vertices / ones: "
           << "\n";
        for (const auto &u : C.w) os << "| " << u.d << "\t" << u.z << "\n";
        os << "\n";
        os << "Red vertices / poles: "
           << "\n";
        for (const auto &u : C.f) os << "| " << u.d << "\t" << u.z << "\n";
        os << "\n";
        os << u8"λ     := " << C.p[0] << "\n";
        Polynomial<typename cpx_t<T>::type> P{1}, Q{1};
        for (auto zd : C.b) P.add_root(zd.z, zd.d);
        for (auto zd : C.f) Q.add_root(zd.z, zd.d);
        os << "P[z_] := " << fmt::format("{}", P) << "\n";
        os << "Q[z_] := " << fmt::format("{}", Q) << "\n";
        return os;
    }

    template <> std::ostream &operator<<(std::ostream &os, const Constellation0<real_t> &C) {
        using T = real_t;
        T    err(C.cost());
        T    lerr(-log10(err));
        auto nd = unsigned(std::max(5, int(lerr) / 2 - 10));
        if (err == T(0)) nd = 10;
        os << std::setprecision(10) << std::fixed;
        os << "Keeping " << nd << " digits."
           << "\n"
           << "\n";

        os << "Black vertices / zeros: "
           << "\n";
        for (auto &zd : C.b) {
            os << "| " << zd.d << "\t" << fmt::format("{:<30}", zd.z);
            if (auto P = guess(zd.z, nd)) os << "\troot of " << fmt::format("{}", *P);
            os << "\n";
        }
        os << "\n";
        os << "White vertices / ones: "
           << "\n";
        for (auto &zd : C.w) {
            os << "| " << zd.d << "\t" << fmt::format("{:<30}", zd.z);
            if (auto P = guess(zd.z, nd)) os << "\troot of " << fmt::format("{}", *P);
            os << "\n";
        }
        os << "\n";
        os << "Red vertices / poles: "
           << "\n";
        for (auto &zd : C.f) {
            os << "| " << zd.d << "\t" << fmt::format("{:<30}", zd.z);
            if (auto P = guess(zd.z, nd)) os << "\troot of " << fmt::format("{}", *P);
            os << "\n";
        }
        os << "\n";
        os << u8"λ     := " << C.p[0] << "\n";
        if (auto L = guess(C.p[0], nd)) os << u8"Λ[z_] := " << fmt::format("{}", *L) << "\n";
        Polynomial<complex_t> P{1}, Q{1};
        for (auto zd : C.b) P.add_root(zd.z, zd.d);
        for (auto zd : C.f) Q.add_root(zd.z, zd.d);
        for (unsigned i = 0; i < P.size(); ++i) {
            auto &x  = P[i];
            auto  xx = complex_t(round(real(x)), round(imag(x)));
            if (abs(x - xx) < pow(T(.1), nd)) x = xx;
        }
        for (unsigned i = 0; i < Q.size(); ++i) {
            auto &x  = Q[i];
            auto  xx = complex_t(round(real(x)), round(imag(x)));
            if (abs(x - xx) < pow(T(.1), nd)) x = xx;
        }
        os << "P[z_] := " << fmt::format("{}", P) << "\nQ[z_] := " << fmt::format("{}", Q) << "\n";
        return os;
    }

    template <typename T> auto Constellation0<T>::bounds() const -> std::pair<cplx, cplx> {
        T xmin(0), xmax(0), ymin(0), ymax(0);
        for (auto z : b) {
            xmin = std::min(xmin, real(z.z));
            xmax = std::max(xmax, real(z.z));
            ymin = std::min(ymin, imag(z.z));
            ymax = std::max(ymax, imag(z.z));
        }
        for (auto z : f) {
            xmin = std::min(xmin, real(z.z));
            xmax = std::max(xmax, real(z.z));
            ymin = std::min(ymin, imag(z.z));
            ymax = std::max(ymax, imag(z.z));
        }
        for (auto z : w) {
            xmin = std::min(xmin, real(z.z));
            xmax = std::max(xmax, real(z.z));
            ymin = std::min(ymin, imag(z.z));
            ymax = std::max(ymax, imag(z.z));
        }
        return {{xmin, ymin}, {xmax, ymax}};
    }

    template <typename T> std::optional<Hypermap> Constellation0<T>::explore() const {
        std::vector<Star<T>> Z;
        for (const auto &z : b) Z.push_back(z);
        for (const auto &z : f) Z.push_back(z);
        for (const auto &z : w) Z.push_back(z);

        size_t maxdeg = 0;
        for (auto z : Z) maxdeg = std::max(maxdeg, z.d);

        auto bd    = bounds();
        T    large = abs(bd.first - bd.second), mindist = large;
        for (unsigned i = 0; i < Z.size(); ++i)
            for (unsigned j = 0; j < Z.size(); ++j)
                if (i != j) mindist = std::min(mindist, abs(Z[j].z - Z[i].z));

        T rad = mindist / 5;

        std::vector<std::vector<cplx>>     hands;
        std::vector<std::vector<unsigned>> halfedges;
        unsigned                           index = 0;

        for (Star<T> z : Z) {
            std::vector<cplx>     hs;
            std::vector<unsigned> he;

            cplx u = z.z + rad * exp(cplx(0, .001));
            T    s = imag((*this)(u));
            for (unsigned i = 0; i < 10 * z.d; ++i) {
                u    = z.z + exp(cplx(0, 2 * pi_<T>() / (10 * z.d))) * (u - z.z);
                T ns = imag((*this)(u));
                if (s * ns < 0) {
                    hs.push_back(u);
                    s = ns;
                    he.push_back(index++);
                }
            }
            if (hs.size() != 2 * z.d) return std::nullopt;
            hands.push_back(hs);
            halfedges.push_back(he);
        }

        std::vector<cplx>     hs;
        std::vector<unsigned> he;
        cplx                  u = large * exp(cplx(0, .001));
        T                     s = imag((*this)(u));
        for (unsigned i = 0; i < 10 * maxdeg; ++i) {
            u    = exp(cplx(0, -2 * pi_<T>() / (10 * maxdeg))) * u;
            T ns = imag((*this)(u));
            if (s * ns < 0) {
                hs.push_back(u);
                s = ns;
                he.push_back(index++);
            }
        }
        hands.push_back(hs);
        halfedges.push_back(he);

        std::vector<std::vector<unsigned>> pairs;

        for (unsigned i = 0; i < Z.size(); ++i) {
            for (unsigned j = 0; j < hands[i].size(); ++j) {
                auto l       = hands[i][j];
                cplx r       = Z[i].z + exp(cplx(0, -2 * pi_<T>() / (10 * Z[i].d))) * (l - Z[i].z);
                auto sl      = imag((*this)(l));
                bool looking = true;
                while (looking) {
                    cplx nz = l + exp(cplx(0, pi_<T>() / 3)) * (r - l);
                    if (abs(nz) > large) {
                        T                     d = large;
                        std::optional<size_t> h;
                        auto                  k = hands.size() - 1;
                        for (unsigned kk = 0; kk < hands[k].size(); ++kk) {
                            T nd = abs(nz - hands[k][kk]);
                            if (nd < d) {
                                d = abs(nz - hands[k][kk]);
                                h = kk;
                            }
                        }
                        if (!h) return std::nullopt;
                        pairs.emplace_back(std::vector<unsigned>{halfedges[i][j], halfedges[k][*h]});
                        looking = false;
                    }
                    for (unsigned k = 0; k < Z.size(); ++k)
                        if (abs(nz - Z[k].z) < rad) {
                            T                     d = rad;
                            std::optional<size_t> h;
                            for (unsigned kk = 0; kk < hands[k].size(); ++kk) {
                                T nd = abs(nz - hands[k][kk]);
                                if (nd < d) {
                                    d = abs(nz - hands[k][kk]);
                                    h = kk;
                                }
                            }
                            if (!h) return std::nullopt;
                            if (halfedges[i][j] < halfedges[k][*h])
                                pairs.emplace_back(std::vector<unsigned>{halfedges[i][j], halfedges[k][*h]});
                            looking = false;
                        }
                    if (sl * imag((*this)(nz)) > 0)
                        l = nz;
                    else
                        r = nz;
                }
            }
        }

        if (pairs.size() != index / 2) return std::nullopt;
        Permutation sigma(halfedges), alpha(pairs), phi((sigma * alpha).inverse());
        for (auto &c : phi.cycles())
            if (c.size() != 3) return std::nullopt;

        return Hypermap(sigma, alpha, phi);
    }
} // namespace vb
