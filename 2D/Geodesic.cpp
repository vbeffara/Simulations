#include <vb/Image.h>
#include <vb/ProgressBar.h>
#include <vb/Ranges.h>
#include <fftw3.h>
#include <limits>
#include <queue>

using namespace vb;
using namespace std;

class Info {
public:
    Info(coo _z, coo _n, double _d, double _f) : z(_z), next(_n), d(_d), f(_f) {}
    bool operator<(const Info & o) const { return d > o.d; }

    coo    z, next;
    double d, f;
};

class QG : public Image {
public:
    explicit QG(const Hub & H)
        : Image(1u << unsigned(H['n']), 1u << unsigned(H['n'])), I(w(), h(), Info({0, 0}, {0, 0}, 0, 0)), g(H['g']), n(H['n']) {

        map<string, function<void()>> fields;
        fields["boolean"]  = [&, this] { fill_boolean(H['z']); };
        fields["dyadic"]   = [&, this] { fill_dyadic(H['z']); };
        fields["free"]     = [&, this] { fill_free(H['z']); };
        fields["gaussian"] = [&, this] { fill_radial([](double x) { return exp(-x * x); }, H['l']); };
        fields["power"]    = [&, this] { fill_radial([&](double x) { return pow(1 + x * x, -double(H['a']) / 2); }, H['l']); };
        fields["white"]    = [&, this] { fill_white(); };
        fields[H['w']]();

        minf = maxf = I.at({0, 0}).f;
        for (auto z : coos(I)) {
            minf = min(minf, I.at(z).f);
            maxf = max(maxf, I.at(z).f);
        }
        H.L->info("Renormalized field: min = {}, max = {}", minf / log(w()), maxf / log(w()));

        for (auto z : coos(*this)) {
            put(z, Grey(255 * (I.at(z).f - minf) / (maxf - minf)));
            if (H['c']) put(z, Indexed(int(at(z)) > 128 ? 1 : 2));
            I.at(z) = Info(z, z, numeric_limits<double>::infinity(), exp(g * I.at(z).f));
        }
    };

    void fill_dyadic(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            int ll = 1u << unsigned(l);
            for (int i = 0; i < ww / ll; ++i)
                for (int j = 0; j < hh / ll; ++j) {
                    double g = prng.gaussian();
                    for (int x = i * ll; x < (i + 1) * ll; ++x)
                        for (int y = j * ll; y < (j + 1) * ll; ++y) I.at(coo(x, y)).f += g;
                }
        }
    }

    void fill_boolean(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            int ll = 1u << unsigned(l);
            for (int i = 0; i < ww / ll; ++i)
                for (int j = 0; j < hh / ll; ++j) {
                    double g = prng.uniform_real(-1, 1);
                    for (int x = i * ll; x < (i + 1) * ll; ++x)
                        for (int y = j * ll; y < (j + 1) * ll; ++y) I.at(coo(x, y)).f += g;
                }
        }
    }

    void fill_white() {
        for (auto z : coos(I)) I[z].f = prng.gaussian() * sqrt(static_cast<double>(n));
    }

    void fill_free(int n0 = 0) {
        auto      in = fftw_alloc_complex(ww * hh), out = fftw_alloc_complex(ww * hh);
        fftw_plan p = fftw_plan_dft_2d(ww, hh, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

        vector<double> sinarrayi(ww), sinarrayj(hh);
        for (int i = 0; i < ww; ++i) sinarrayi[i] = sin(M_PI * i / ww);
        for (int j = 0; j < hh; ++j) sinarrayj[j] = sin(M_PI * j / hh);

        for (auto [i, j] : coos(*this)) {
            if ((i == 0) && (j == 0)) continue;
            double norm       = sqrt(ww * hh * (sinarrayi[i] * sinarrayi[i] + sinarrayj[j] * sinarrayj[j]));
            auto   fij        = cpx(prng.gaussian(), prng.gaussian()) * sqrt(M_PI / 2) / norm;
            in[i + ww * j][0] = real(fij);
            in[i + ww * j][1] = imag(fij);
            if (norm > sqrt(ww * hh) * (1 - n0 / 100.0)) {
                in[i + ww * j][0] = 0;
                in[i + ww * j][1] = 0;
            }
        }
        in[0][0] = 0;
        in[0][1] = 0;

        fftw_execute(p);
        for (auto [i, j] : coos(*this)) I.at(coo(i, j)).f = out[i + ww * j][0];
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }

    void fill_radial(function<double(double)> f, double l) {
        auto d  = fftw_alloc_complex(ww * hh);
        auto p1 = fftw_plan_dft_2d(ww, hh, d, d, FFTW_FORWARD, FFTW_ESTIMATE);
        auto p2 = fftw_plan_dft_2d(ww, hh, d, d, FFTW_BACKWARD, FFTW_ESTIMATE);

        for (auto [i, j] : coos(*this)) {
            auto ii = min(i, ww - i), jj = min(j, hh - j);
            d[i + ww * j][0] = d[i + ww * j][1] = f(sqrt(ii * ii + jj * jj) / l);
        }

        fftw_execute(p1);
        for (int i = 0; i < ww * hh; ++i) {
            d[i][0] *= prng.gaussian();
            d[i][1] *= prng.gaussian();
        }
        fftw_execute(p2);
        for (auto z : coos(*this)) I.at(z).f = sign(d[z.x + ww * z.y][0]);

        fftw_destroy_plan(p1);
        fftw_destroy_plan(p2);
        fftw_free(d);
    }

    void dijkstra() {
        coo                  mid(w() / 2, h() / 2);
        priority_queue<Info> Q;
        I.at(mid).d = 0;
        Q.push(I.at(mid));
        ProgressBar PB(w() * h());

        for (int t = 0; t < w() * h(); ++t) {
            PB.set(t);
            while (I.at(Q.top().z).d < Q.top().d) Q.pop();
            Info im = Q.top();
            Q.pop();
            for (int k = 0; k < 8; ++k) {
                coo nz = im.z + dz[k];
                if (!(contains(nz))) continue;
                Info & ni = I.at(nz);
                double nd = im.d + (k < 4 ? .5 : sqrt(.5)) * (im.f + ni.f);
                if (ni.d > nd) {
                    ni.d    = nd;
                    ni.next = im.z;
                    Q.push(ni);
                }
            }
        }
    }

    double radius() {
        double r = I.at({0, 0}).d;
        for (int i = 0; i < w(); ++i) {
            r = min(r, I.at(coo(i, 0)).d);
            r = min(r, I.at(coo(0, i)).d);
            r = min(r, I.at(coo(i, h() - 1)).d);
            r = min(r, I.at(coo(w() - 1, i)).d);
        }
        return r;
    }

    void trace(coo z, Color c = RED) {
        while (at(z) != c) {
            put(z, c);
            z = I.at(z).next;
        }
    }

    void ball() {
        double r = radius();
        for (auto z : coos(*this))
            if (I.at(z).d <= r) put(z, Color(0, 0, 127 + at(z).b / 2));
    }

    Array<Info> I;
    double      g, minf, maxf;
    int         n;
};

int main(int argc, char ** argv) {
    H.init("Random 2D geometry", argc, argv, "w=free,n=9,z=0,g=1,s=0,b,i,q,c,l=10,a=1");
    if (int s = H['s']) prng.seed(s);
    unsigned n = H['n'], nn = 1u << n;

    QG img(H);
    if (!H['i']) img.show();

    if (!H['q']) {
        img.dijkstra();
        if (H['b']) img.ball();
        for (int i = 0; i <= nn - 1; i += 1) {
            img.trace(coo(0, i));
            img.trace(coo(nn - 1, i));
            img.trace(coo(i, 0));
            img.trace(coo(i, nn - 1));
        }
    }

    if (!H['i']) { img.pause(); }
    img.output();
}
