// Initial code in Python by Sunil Chhita, ported to C++ by VB.
#include <cmath>
#include <fstream>
#include <gsl/gsl>
#include <vb/Figure.h>
#include <vb/data/Array.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto dupe(const Array<double> &a) {
    Array<double> aa({2 * a.size.x, 2 * a.size.y});
    for (auto z : coo_range(aa.size)) aa[z] = a[z / 2U];
    return aa;
}

auto twoperiodic(double a, double b) { return dupe(Array<double>({{a, b}, {b, a}})); }
auto threeperiodic(double a, double b, double c) { return dupe(Array<double>({{a, b, c}, {b, c, a}, {c, a, b}})); }
auto threebytwo(double b) {
    Array<double> p({6, 6}, 1);
    p[{0, 0}] = b;
    p[{4, 2}] = b;
    p[{2, 4}] = b;
    return p;
}

struct Tiling {
    void delslide() {
        auto nn = state.size.x;
        state.resize({nn + 2, nn + 2}, 0);
        for (auto y = state.size.y - 2; y > 0; --y)
            for (auto x = state.size.x - 2; x > 0; --x) swap(state[{x, y}], state[{(x - 1) + nn * (y - 1), 0}]);

        for (size_t i = 0; i < nn / 2; ++i) {
            for (size_t j = 0; j < nn / 2; ++j) {
                ucoo const z{2 * i, 2 * j};
                if ((state[z] == 1) && (state[z + ucoo{1, 1}] == 1)) {
                    state[z]              = 0;
                    state[z + ucoo{1, 1}] = 0;
                } else if ((state[z + ucoo{0, 1}] == 1) && (state[z + ucoo{1, 0}] == 1)) {
                    state[z + ucoo{1, 0}] = 0;
                    state[z + ucoo{0, 1}] = 0;
                }
            }
        }
        for (size_t i = 0; i <= nn; i += 2) {
            for (size_t j = 0; j <= nn; j += 2) {
                if (state[{i + 1, j + 1}] == 1) {
                    state[{i, j}]         = 1;
                    state[{i + 1, j + 1}] = 0;
                } else if (state[{i, j}] == 1) {
                    state[{i, j}]         = 0;
                    state[{i + 1, j + 1}] = 1;
                } else if (state[{i + 1, j}] == 1) {
                    state[{i, j + 1}] = 1;
                    state[{i + 1, j}] = 0;
                } else if (state[{i, j + 1}] == 1) {
                    state[{i + 1, j}] = 1;
                    state[{i, j + 1}] = 0;
                }
            }
        }
    }

    void create(const Array<double> &p) {
        auto nn = state.size.x;
        for (size_t i = 0; i < nn / 2; ++i) {
            for (size_t j = 0; j < nn / 2; ++j) {
                if ((state[{2 * i, 2 * j}] == 0) && (state[{2 * i + 1, 2 * j}] == 0) && (state[{2 * i, 2 * j + 1}] == 0) &&
                    (state[{2 * i + 1, 2 * j + 1}] == 0)) {
                    bool a1 = true, a2 = true, a3 = true, a4 = true;
                    if (j > 0) a1 = (state[{2 * i, 2 * j - 1}] == 0) && (state[{2 * i + 1, 2 * j - 1}] == 0);
                    if (j < nn / 2 - 1) a2 = (state[{2 * i, 2 * j + 2}] == 0) && (state[{2 * i + 1, 2 * j + 2}] == 0);
                    if (i > 0) a3 = (state[{2 * i - 1, 2 * j}] == 0) && (state[{2 * i - 1, 2 * j + 1}] == 0);
                    if (i < nn / 2 - 1) a4 = (state[{2 * i + 2, 2 * j}] == 0) && (state[{2 * i + 2, 2 * j + 1}] == 0);
                    if (a1 && a2 && a3 && a4) {
                        if (prng.bernoulli(p.atp({int(i), int(j)}))) {
                            state[{2 * i, 2 * j}]         = 1;
                            state[{2 * i + 1, 2 * j + 1}] = 1;
                        } else {
                            state[{2 * i + 1, 2 * j}] = 1;
                            state[{2 * i, 2 * j + 1}] = 1;
                        }
                    }
                }
            }
        }
    }

    void probs() {
        vector<Array<pair<double, double>>> A(n, Array<pair<double, double>>{ucoo{per, per}});

        for (auto z : coo_range(A[0].size))
            if (double const w = TP.atp(coo(z)); w != 0)
                A[0][z] = {w, 0};
            else
                A[0][z] = {1, 1};

        for (size_t k = 1; k < n; ++k) {
            const auto &AA = A[k - 1];
            for (auto z : coo_range(A[k].size)) {
                auto   i = z.x, j = z.y, i1 = (i + 1) % per, j1 = (j + 1) % per, ii = (i + 2 * (i % 2)) % per, jj = (j + 2 * (j % 2)) % per;
                double a20 = NAN, a21 = NAN;
                const auto &a1 = AA[{ii, jj}];
                if (a1.second + AA[{i1, j1}].second == AA[{ii, j1}].second + AA[{i1, jj}].second) {
                    a20 = a1.first * AA[{i1, j1}].first + AA[{ii, j1}].first * AA[{i1, jj}].first;
                    a21 = a1.second + AA[{i1, j1}].second;
                } else if (a1.second + AA[{i1, j1}].second < AA[{ii, j1}].second + AA[{i1, jj}].second) {
                    a20 = a1.first * AA[{i1, j1}].first;
                    a21 = a1.second + AA[{i1, j1}].second;
                } else {
                    a20 = AA[{ii, j1}].first * AA[{i1, jj}].first;
                    a21 = AA[{ii, j1}].second + AA[{i1, jj}].second;
                }
                A[k][z] = {a1.first / a20, a1.second - a21};
            }
        }

        pbs = vector<Array<double>>(n, Array<double>{ucoo{per / 2, per / 2}});

        for (size_t k = 0; k < n; ++k) {
            const auto &a0nk1 = A[n - k - 1];
            for (auto z : coo_range(pbs[k].size)) {
                auto i = int64_t(z.x), j = int64_t(z.y);
                if (a0nk1.atp({2 * i, 2 * j}).second + a0nk1.atp({2 * i + 1, 2 * j + 1}).second >
                    a0nk1.atp({2 * i + 1, 2 * j}).second + a0nk1.atp({2 * i, 2 * j + 1}).second)
                    pbs[k][z] = 0;
                else if (a0nk1.atp({2 * i, 2 * j}).second + a0nk1.atp({2 * i + 1, 2 * j + 1}).second <
                         a0nk1.atp({2 * i + 1, 2 * j}).second + a0nk1.atp({2 * i, 2 * j + 1}).second)
                    pbs[k][z] = 1;
                else
                    pbs[k][z] = (a0nk1.atp({2 * i + 1, 2 * j + 1}).first * a0nk1.atp({2 * i, 2 * j}).first /
                                 (a0nk1.atp({2 * i + 1, 2 * j + 1}).first * a0nk1.atp({2 * i, 2 * j}).first +
                                  a0nk1.atp({2 * i + 1, 2 * j}).first * a0nk1.atp({2 * i, 2 * j + 1}).first));
            }
        }
    }

    void output_pdf(const std::string &s, const string &fname, size_t off = 0) const {
        Figure                   F(s);
        static const vector<int> ddx{0, 2, 0, 2}, ddy{0, -2, -4, -6};
        int                      offx = ddx[off % 4], offy = ddy[off % 4];
        for (auto z : coo_range(state.size))
            if (state[z] != 0) {
                coo const edge{1, ((z.x + z.y) % 2) != 0 ? 1 : -1};
                auto      ss = [=, this](ucoo z_) {
                    z_ += coo{offx, offy};
                    ucoo const zz = (z_ + coo{1, 1}) / 2;
                    coo const  sh = dz[(zz.y + ((((zz.x + 1) % 4) / 2) != 0 ? 5 : 3)) % 4];
                    return cpx(double(z_.x), double(z_.y)) + 2 * double(H['r']) * cpx(double(sh.x), double(sh.y)) - cpx(offx, offy);
                };
                double const gr = a * TP.atp(coo(z) + coo{offx / 2, offy / 2}) + b;
                F.add(make_unique<Segment>(ss(z * 2 - edge), ss(z * 2 + edge), Pen(Grey(uint8_t(255 * gr)), 130.0 / double(state.size.x))));
            }
        if (H['v']) F.show();
        F.output(fname);
    }

    [[nodiscard]] auto height() const {
        auto       mm = state.size.x / 2;
        Array<int> h({mm + 1, mm + 1}, 0);
        int        z = 0;
        for (size_t x = 0; x < mm; ++x) {
            z += 4 * state[{2 * x, 0}] + 4 * state[{2 * x + 1, 0}] - 2;
            h[{x + 1, 0}] = z;
        }
        for (size_t y = 0; y < mm; ++y) {
            int zz        = h[{0, y}] + 4 * state[{0, 2 * y}] + 4 * state[{0, 2 * y + 1}] - 2;
            h[{0, y + 1}] = zz;
            for (size_t x = 0; x < mm; ++x) {
                zz -= 4 * state[{2 * x, 2 * y + 1}] + 4 * state[{2 * x + 1, 2 * y + 1}] - 2;
                h[{x + 1, y + 1}] = zz;
            }
        }
        return h;
    }

    auto aztecgen() {
        for (size_t i = 0; i < n; ++i) {
            delslide();
            create(pbs[i]);
            if (H['v']) output_pdf(H.title, "snapshots/" + fmt::format("snapshot_{:04d}", i), n - i - 1);
        }
    }

    void run(const Hub &HH) {
        aztecgen();
        output_pdf(HH.title, name);

        auto     H1 = height();
        ofstream dat(name + ".dat");
        for (auto z : coo_range(H1.size)) {
            dat << H1[z] << " ";
            if (z.x == H1.size.x - 1) dat << "\n";
        }
    }

    Tiling(const Hub &H_, Array<double> TP_, size_t mm)
        : H(H_), name(H.title), TP(std::move(TP_)), m(mm), per(lcm(TP.size.x, TP.size.y)), n(m * per / 2) {
        probs();
        for (auto z : coo_range(TP.size)) {
            if (TP[z] == 0) continue;
            if (TP[z] > pmax) pmax = TP[z];
            if (TP[z] < pmin) pmin = TP[z];
        }
        if (pmin != pmax) {
            a = .7 / (pmax - pmin);
            b = -.7 * pmin / (pmax - pmin);
        }
    }

    const Hub            &H;
    string                name;
    const Array<double>   TP;
    size_t                m, per, n;
    double                pmin{1.0}, pmax{0.0}, a{0.0}, b{0.0};
    vector<Array<double>> pbs;
    Array<uint8_t>        state{{0, 0}};
};

auto main(int argc, char **argv) -> int {
    Hub H("Domino shuffle", argc, argv, "m=50,a=1,b=.5,c=.3,s=0,r=0,w=two,v");
    if (size_t const seed = H['s']; seed != 0) prng.seed(seed);

    std::map<string, function<Array<double>()>> TPs;
    TPs["unif"]   = [&] { return Array<double>({2, 2}, 1.0); };
    TPs["two"]    = [&] { return twoperiodic(H['a'], H['b']); };
    TPs["three"]  = [&] { return threeperiodic(H['a'], H['b'], H['c']); };
    TPs["kenyon"] = [&] { return threebytwo(H['b']); };

    Tiling{H, TPs[H['w']](), H['m']}.run(H);
}
