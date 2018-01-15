// Initial code in Python by Sunil Chhita, ported to C++ by VB.
#include <vb/Figure.h>
#include <vb/Stream_lib.h>
#include <fstream>

using namespace vb;
using namespace std;

auto dupe(const Array<double> & a) {
    Array<double> aa(2 * a.ww, 2 * a.hh);
    for (auto z : coos(aa)) aa[z] = a[z / 2];
    return aa;
}

auto twoperiodic(double a, double b) { return dupe(Array<double>({{a, b}, {b, a}})); }
auto threeperiodic(double a, double b, double c) { return dupe(Array<double>({{a, b, c}, {b, c, a}, {c, a, b}})); }
auto threebytwo(double b) {
    Array<double> p(6, 6, 1);
    p[coo(0, 0)] = b;
    p[coo(4, 2)] = b;
    p[coo(2, 4)] = b;
    return p;
}

Array<int> delslide(Array<int> a0) {
    int n = a0.ww;
    a0.resize(n + 2, n + 2, 0);
    for (int y = a0.hh - 2; y > 0; --y)
        for (int x = a0.ww - 2; x > 0; --x) swap(a0[{x, y}], a0[{(x - 1) + n * (y - 1), 0}]);

    for (int i = 0; i < n / 2; ++i) {
        for (int j = 0; j < n / 2; ++j) {
            coo z{2 * i, 2 * j};
            if ((a0[z] == 1) && (a0[z + coo{1, 1}] == 1)) {
                a0[z]             = 0;
                a0[z + coo{1, 1}] = 0;
            } else if ((a0[z + coo{0, 1}] == 1) && (a0[z + coo{1, 0}] == 1)) {
                a0[z + coo{1, 0}] = 0;
                a0[z + coo{0, 1}] = 0;
            }
        }
    }
    for (int i = 0; i <= n; i += 2) {
        for (int j = 0; j <= n; j += 2) {
            if (a0[coo(i + 1, j + 1)] == 1) {
                a0[coo(i, j)]         = 1;
                a0[coo(i + 1, j + 1)] = 0;
            } else if (a0[coo(i, j)] == 1) {
                a0[coo(i, j)]         = 0;
                a0[coo(i + 1, j + 1)] = 1;
            } else if (a0[coo(i + 1, j)] == 1) {
                a0[coo(i, j + 1)] = 1;
                a0[coo(i + 1, j)] = 0;
            } else if (a0[coo(i, j + 1)] == 1) {
                a0[coo(i + 1, j)] = 1;
                a0[coo(i, j + 1)] = 0;
            }
        }
    }
    return a0;
}

auto create(Array<int> x0, const Array<double> & p) {
    int n = x0.ww;
    for (int i = 0; i < n / 2; ++i) {
        for (int j = 0; j < n / 2; ++j) {
            if ((x0[coo(2 * i, 2 * j)] == 0) && (x0[coo(2 * i + 1, 2 * j)] == 0) && (x0[coo(2 * i, 2 * j + 1)] == 0) &&
                (x0[coo(2 * i + 1, 2 * j + 1)] == 0)) {
                bool a1 = true, a2 = true, a3 = true, a4 = true;
                if (j > 0) a1 = (x0[coo(2 * i, 2 * j - 1)] == 0) && (x0[coo(2 * i + 1, 2 * j - 1)] == 0);
                if (j < n / 2 - 1) a2 = (x0[coo(2 * i, 2 * j + 2)] == 0) && (x0[coo(2 * i + 1, 2 * j + 2)] == 0);
                if (i > 0) a3 = (x0[coo(2 * i - 1, 2 * j)] == 0) && (x0[coo(2 * i - 1, 2 * j + 1)] == 0);
                if (i < n / 2 - 1) a4 = (x0[coo(2 * i + 2, 2 * j)] == 0) && (x0[coo(2 * i + 2, 2 * j + 1)] == 0);
                if (a1 && a2 && a3 && a4) {
                    if (prng.bernoulli(p.atp(coo(i, j)))) {
                        x0[coo(2 * i, 2 * j)]         = 1;
                        x0[coo(2 * i + 1, 2 * j + 1)] = 1;
                    } else {
                        x0[coo(2 * i + 1, 2 * j)] = 1;
                        x0[coo(2 * i, 2 * j + 1)] = 1;
                    }
                }
            }
        }
    }
    return x0;
}

auto height(const Array<int> & A) {
    int        m = A.ww / 2;
    Array<int> h(m + 1, m + 1, 0);
    int        z = 0;
    for (int x = 0; x < m; ++x) {
        z += 4 * A[coo(2 * x, 0)] + 4 * A[coo(2 * x + 1, 0)] - 2;
        h[coo(x + 1, 0)] = z;
    }
    for (int y = 0; y < m; ++y) {
        int z            = h[coo(0, y)] + 4 * A[coo(0, 2 * y)] + 4 * A[coo(0, 2 * y + 1)] - 2;
        h[coo(0, y + 1)] = z;
        for (int x = 0; x < m; ++x) {
            z -= 4 * A[coo(2 * x, 2 * y + 1)] + 4 * A[coo(2 * x + 1, 2 * y + 1)] - 2;
            h[coo(x + 1, y + 1)] = z;
        }
    }
    return h;
}

struct Tiling {
    auto probs() {
        vector<Array<pair<double, double>>> A(n, {per, per});

        for (auto z : coos(A[0]))
            if (double w = TP.atp(z))
                A[0][z] = {w, 0};
            else
                A[0][z] = {1, 1};

        for (int k = 1; k < n; ++k) {
            const auto & AA = A[k - 1];
            for (auto z : coos(A[k])) {
                int    i = z.x, j = z.y, i1 = (i + 1) % per, j1 = (j + 1) % per, ii = (i + 2 * (i % 2)) % per, jj = (j + 2 * (j % 2)) % per;
                double a20, a21;
                auto & a1 = AA[{ii, jj}];
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

        pbs = vector<Array<double>>(n, {per / 2, per / 2});

        for (int k = 0; k < n; ++k) {
            const auto & a0nk1 = A[n - k - 1];
            for (auto z : coos(pbs[k])) {
                int i = z.x, j = z.y;
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

    void output_pdf(const Array<int> & A1, const string & name, int off = 0) {
        Figure F;
        int    ddx[4] = {0, 2, 0, 2}, ddy[4] = {0, -2, -4, -6};
        int    offx = ddx[off % 4], offy = ddy[off % 4];
        for (auto z : coos(A1))
            if (A1[z] != 0) {
                coo  edge(1, ((z.x + z.y) % 2) != 0 ? 1 : -1);
                auto s = [=](coo z) {
                    z += coo(offx, offy);
                    coo zz = (z + coo{1, 1}) / 2;
                    coo sh = dz[(zz.y + ((((zz.x + 1) % 4) / 2) != 0 ? 5 : 3)) % 4];
                    return cpx(z) + 2 * double(H['r']) * cpx(sh) - cpx(offx, offy);
                };
                double gr = a * TP.atp(z + coo(offx / 2, offy / 2)) + b;
                F.add(make_unique<Segment>(s(z * 2 - edge), s(z * 2 + edge), Pen(Grey(255 * gr), 130.0 / A1.ww)));
            }
        if (H['v']) F.show();
        F.output(name);
    }

    auto aztecgen() {
        Array<int> as{0, 0};
        for (int i = 0; i < n; ++i) {
            as = create(delslide(std::move(as)), pbs[i]);
            if (H['v']) output_pdf(as, "snapshots/" + fmt::format("snapshot_{:04d}", i), n - i - 1);
        }
        return as;
    }

    void run() {
        auto as = aztecgen(), H1 = height(as);

        output_pdf(as, name);
        ofstream dat(H.dir + name + ".dat");
        for (auto z : coos(H1)) {
            dat << H1[z] << " ";
            if (z.x == H1.ww - 1) dat << "\n";
        }
    }

    Tiling(Array<double> TP_, int m) : TP(move(TP_)), m(m), per(lcm(TP.ww, TP.hh)), n(m * per / 2) {
        probs();
        for (auto z : coos(TP)) {
            if (TP[z] == 0) continue;
            if (TP[z] > pmax) pmax = TP[z];
            if (TP[z] < pmin) pmin = TP[z];
        }
        if (pmin != pmax) {
            a = .7 / (pmax - pmin);
            b = -.7 * pmin / (pmax - pmin);
        }
    }

    string                name = H.title;
    const Array<double>   TP;
    int                   m, per, n;
    double                pmin{1.0}, pmax{0.0}, a{0.0}, b{0.0};
    vector<Array<double>> pbs;
};

int main(int argc, char ** argv) {
    H.init("Domino shuffle", argc, argv, "m=50,a=1,b=.5,c=.3,s=0,r=0,w=two,v");
    if (int seed = H['s']; seed != 0) prng.seed(seed);

    std::map<string, function<Array<double>()>> TPs;
    TPs["unif"]   = [&] { return Array<double>(2, 2, 1.0); };
    TPs["two"]    = [&] { return twoperiodic(H['a'], H['b']); };
    TPs["three"]  = [&] { return threeperiodic(H['a'], H['b'], H['c']); };
    TPs["kenyon"] = [&] { return threebytwo(H['b']); };

    Tiling{TPs[H['w']](), H['m']}.run();
}
