#include <fftw3.h>
#include <limits>
#include <queue>
#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Info {
public:
    Info(coo _z, coo _n, double _d, double _f) : z(_z), next(_n), d(_d), f(_f) {}
    bool operator<(const Info &o) const { return d > o.d; }

    coo    z, next;
    double d, f;
};

class QG : public Image {
public:
    explicit QG(const Hub &H)
        : Image(H.title, {1u << unsigned(H['n']), 1u << unsigned(H['n'])}), I({w(), h()}, Info({0, 0}, {0, 0}, 0, 0)), g(H['g']),
          n(H['n']) {
        map<string, function<void()>> fields;
        fields["boolean"]  = [&, this] { fill_boolean(H['z']); };
        fields["dyadic"]   = [&, this] { fill_dyadic(H['z']); };
        fields["free"]     = [&, this] { fill_free(H['z']); };
        fields["gaussian"] = [&, this] { fill_radial([](double x) { return exp(-x * x); }, H['l']); };
        fields["power"]    = [&, this] { fill_radial([&](double x) { return pow(1 + x * x, -double(H['a']) / 2); }, H['l']); };
        fields["white"]    = [&, this] { fill_white(); };
        fields[H['w']]();

        minf = maxf = I.at({0, 0}).f;
        for (auto z : coo_range(I.size)) {
            minf = min(minf, I.at(z).f);
            maxf = max(maxf, I.at(z).f);
        }
        spdlog::info("Renormalized field: min = {}, max = {}", minf / log(w()), maxf / log(w()));

        for (auto z : coo_range(size)) {
            put(z, Grey(uint8_t(255 * (I.at(z).f - minf) / (maxf - minf))));
            if (H['c']) put(z, Indexed(int(at(z)) > 128 ? 1 : 2));
            I.at(z) = Info(z, z, numeric_limits<double>::infinity(), exp(g * I.at(z).f));
        }
    };

    void fill_dyadic(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            int ll = 1u << unsigned(l);
            for (int i = 0; i < size.x / ll; ++i)
                for (int j = 0; j < size.y / ll; ++j) {
                    double g = prng.gaussian();
                    for (int x = i * ll; x < (i + 1) * ll; ++x)
                        for (int y = j * ll; y < (j + 1) * ll; ++y) I.at({x, y}).f += g;
                }
        }
    }

    void fill_boolean(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            int ll = 1u << unsigned(l);
            for (int i = 0; i < size.x / ll; ++i)
                for (int j = 0; j < size.y / ll; ++j) {
                    double g = prng.uniform_real(-1, 1);
                    for (int x = i * ll; x < (i + 1) * ll; ++x)
                        for (int y = j * ll; y < (j + 1) * ll; ++y) I.at({x, y}).f += g;
                }
        }
    }

    void fill_white() {
        for (auto z : coo_range(I.size)) I[z].f = prng.gaussian() * sqrt(static_cast<double>(n));
    }

    void fill_free(int n0 = 0) {
        auto                    in = fftw_alloc_complex(size.x * size.y), out = fftw_alloc_complex(size.x * size.y);
        fftw_plan               p = fftw_plan_dft_2d(size.x, size.y, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        gsl::span<fftw_complex> in_{in, size.x * size.y}, out_{out, size.x * size.y};

        vector<double> sinarrayi(size.x), sinarrayj(size.y);
        for (int i = 0; i < size.x; ++i) sinarrayi[i] = sin(M_PI * i / size.x);
        for (int j = 0; j < size.y; ++j) sinarrayj[j] = sin(M_PI * j / size.y);

        for (const auto [i, j] : coo_range(size)) {
            if ((i == 0) && (j == 0)) continue;
            double norm            = sqrt(size.x * size.y * (sinarrayi[i] * sinarrayi[i] + sinarrayj[j] * sinarrayj[j]));
            auto   fij             = cpx(prng.gaussian(), prng.gaussian()) * sqrt(M_PI / 2) / norm;
            in_[i + size.x * j][0] = real(fij);
            in_[i + size.x * j][1] = imag(fij);
            if (norm > sqrt(size.x * size.y) * (1 - n0 / 100.0)) {
                in_[i + size.x * j][0] = 0;
                in_[i + size.x * j][1] = 0;
            }
        }
        in_[0][0] = 0;
        in_[0][1] = 0;

        fftw_execute(p);
        for (auto [i, j] : coo_range(size)) I.at({i, j}).f = out_[i + size.x * j][0];
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }

    void fill_radial(const function<double(double)> &f, double l) {
        auto d  = fftw_alloc_complex(size.x * size.y);
        auto d_ = gsl::span<fftw_complex>{d, size.x * size.y};
        auto p1 = fftw_plan_dft_2d(size.x, size.y, d, d, FFTW_FORWARD, FFTW_ESTIMATE);
        auto p2 = fftw_plan_dft_2d(size.x, size.y, d, d, FFTW_BACKWARD, FFTW_ESTIMATE);

        for (const auto [i, j] : coo_range(size)) {
            auto ii = min(i, size.x - i), jj = min(j, size.y - j);
            d_[i + size.x * j][0] = d_[i + size.x * j][1] = f(sqrt(ii * ii + jj * jj) / l);
        }

        fftw_execute(p1);
        for (int i = 0; i < size.x * size.y; ++i) {
            d_[i][0] *= prng.gaussian();
            d_[i][1] *= prng.gaussian();
        }
        fftw_execute(p2);
        for (auto z : coo_range(size)) I.at(z).f = sign(d_[z.x + size.x * z.y][0]);

        fftw_destroy_plan(p1);
        fftw_destroy_plan(p2);
        fftw_free(d);
    }

    void dijkstra() {
        coo                  mid{w() / 2, h() / 2};
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
            r = min(r, I.at({i, 0}).d);
            r = min(r, I.at({0, i}).d);
            r = min(r, I.at({i, h() - 1}).d);
            r = min(r, I.at({w() - 1, i}).d);
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
        for (auto z : coo_range(size))
            if (I.at(z).d <= r) put(z, Color(0, 0, 127 + at(z).b / 2));
    }

    Array<Info> I;
    double      g, minf, maxf;
    int         n;
};

int main(int argc, char **argv) {
    Hub H("Random 2D geometry", argc, argv, "w=free,n=9,z=0,g=1,s=0,b,i,q,c,l=10,a=1");
    if (int s = H['s']) prng.seed(s);
    unsigned n = H['n'], nn = 1u << n;

    QG img(H);
    if (!H['i']) img.show();

    if (!H['q']) {
        img.dijkstra();
        if (H['b']) img.ball();
        for (unsigned i = 0; i <= nn - 1; i += 1) {
            img.trace({0, i});
            img.trace({nn - 1, i});
            img.trace({i, 0});
            img.trace({i, nn - 1});
        }
    }

    if (!H['i']) { img.pause(); }
    img.output(H.title);
}
