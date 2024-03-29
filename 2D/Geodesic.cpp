#include <fftw3.h>
#include <limits>
#include <queue>
#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/math/math.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Info {
public:
    Info(ucoo _z, ucoo _n, double _d, double _f) : z(_z), next(_n), d(_d), f(_f) {}
    auto operator<(const Info &o) const -> bool { return d > o.d; }

    ucoo   z, next;
    double d, f;
};

class QG : public Image {
public:
    explicit QG(const Hub &H)
        : Image(H.title, {1U << unsigned(H['n']), 1U << unsigned(H['n'])}), I(size, Info({0, 0}, {0, 0}, 0, 0)), g(H['g']), n(H['n']) {
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
        spdlog::info("Renormalized field: min = {}, max = {}", minf / log(size.x), maxf / log(size.x));

        for (auto z : coo_range(size)) {
            put(z, Grey(uint8_t(255 * (I.at(z).f - minf) / (maxf - minf))));
            if (H['c']) put(z, Indexed(int(at(z)) > 128 ? 1 : 2));
            I.at(z) = Info(z, z, numeric_limits<double>::infinity(), exp(g * I.at(z).f));
        }
    };

    void fill_dyadic(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            unsigned const ll = 1U << unsigned(l);
            for (size_t ii = 0; ii < size.x / ll; ++ii)
                for (size_t j = 0; j < size.y / ll; ++j) {
                    double const noise = prng.gaussian();
                    for (auto x = ii * ll; x < (ii + 1) * ll; ++x)
                        for (auto y = j * ll; y < (j + 1) * ll; ++y) I.at({x, y}).f += noise;
                }
        }
    }

    void fill_boolean(int n0) {
        for (int l = n - 1; l >= n0; --l) {
            unsigned const ll = 1U << unsigned(l);
            for (size_t ii = 0; ii < size.x / ll; ++ii)
                for (size_t j = 0; j < size.y / ll; ++j) {
                    double const noise = prng.uniform_real(-1, 1);
                    for (auto x = ii * ll; x < (ii + 1) * ll; ++x)
                        for (auto y = j * ll; y < (j + 1) * ll; ++y) I.at({x, y}).f += noise;
                }
        }
    }

    void fill_white() {
        for (auto z : coo_range(I.size)) I[z].f = prng.gaussian() * sqrt(static_cast<double>(n));
    }

    void fill_free(int n0 = 0) {
        auto                    in = fftw_alloc_complex(size_t(size.x * size.y)), out = fftw_alloc_complex(size_t(size.x * size.y));
        fftw_plan               p = fftw_plan_dft_2d(int(size.x), int(size.y), in, out, FFTW_FORWARD, 1U << 6U /* FFTW_ESTIMATE */);
        gsl::span<fftw_complex> in_{in, size.x * size.y}, out_{out, size.x * size.y};

        vector<double> sinarrayi(size_t(size.x)), sinarrayj(size_t(size.y));
        for (size_t ii = 0; ii < size.x; ++ii) sinarrayi[ii] = sin(M_PI * double(ii) / double(size.x));
        for (size_t jj = 0; jj < size.y; ++jj) sinarrayj[jj] = sin(M_PI * double(jj) / double(size.y));

        for (auto [ii, jj] : coo_range(size)) {
            if ((ii == 0) && (jj == 0)) continue;
            auto   ij   = ii + size.x * jj;
            double const norm = sqrt(double(size.x * size.y) * (sinarrayi[ii] * sinarrayi[ii] + sinarrayj[jj] * sinarrayj[jj]));
            auto   fij  = cpx(prng.gaussian(), prng.gaussian()) * sqrt(M_PI / 2) / norm;
            in_[ij][0]  = real(fij);
            in_[ij][1]  = imag(fij);
            if (norm > sqrt(size.x * size.y) * (1 - n0 / 100.0)) {
                in_[ij][0] = 0;
                in_[ij][1] = 0;
            }
        }
        in_[0][0] = 0;
        in_[0][1] = 0;

        fftw_execute(p);
        for (auto [ii, jj] : coo_range(size)) I.at({ii, jj}).f = out_[ii + size.x * jj][0];
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }

    void fill_radial(const function<double(double)> &f, double l) {
        auto *d  = fftw_alloc_complex(size.x * size.y);
        auto  d_ = gsl::span<fftw_complex>{d, size.x * size.y};
        auto *p1 = fftw_plan_dft_2d(int(size.x), int(size.y), d, d, FFTW_FORWARD, 1U << 6U /* FFTW_ESTIMATE */);
        auto *p2 = fftw_plan_dft_2d(int(size.x), int(size.y), d, d, FFTW_BACKWARD, 1U << 6U /* FFTW_ESTIMATE */);

        for (auto [ii, jj] : coo_range(size)) {
            auto iii = min(ii, size.x - ii), jjj = min(jj, size.y - jj);
            d_[ii + size.x * jj][0] = d_[ii + size.x * jj][1] = f(sqrt(iii * iii + jjj * jjj) / l);
        }

        fftw_execute(p1);
        for (size_t ii = 0; ii < size.x * size.y; ++ii) {
            d_[ii][0] *= prng.gaussian();
            d_[ii][1] *= prng.gaussian();
        }
        fftw_execute(p2);
        for (auto z : coo_range(size)) I.at(z).f = sign(d_[z.x + size.x * z.y][0]);

        fftw_destroy_plan(p1);
        fftw_destroy_plan(p2);
        fftw_free(d);
    }

    void dijkstra() {
        ucoo const           mid = size / 2;
        priority_queue<Info> Q;
        I.at(mid).d = 0;
        Q.push(I.at(mid));
        ProgressBar PB(size_t(size.x * size.y));

        for (size_t t = 0; t < size_t(size.x * size.y); ++t) {
            PB.set(t);
            while (I.at(Q.top().z).d < Q.top().d) Q.pop();
            Info const im = Q.top();
            Q.pop();
            for (unsigned k = 0; k < 8; ++k) {
                auto nz = im.z + dz[k];
                if (!fits(nz)) continue;
                Info & ni = I.at(nz);
                double const nd = im.d + (k < 4 ? .5 : sqrt(.5)) * (im.f + ni.f);
                if (ni.d > nd) {
                    ni.d    = nd;
                    ni.next = im.z;
                    Q.push(ni);
                }
            }
        }
    }

    auto radius() -> double {
        double r = I.at({0, 0}).d;
        for (size_t ii = 0; ii < size.x; ++ii) {
            r = min(r, I.at({ii, 0}).d);
            r = min(r, I.at({0, ii}).d);
            r = min(r, I.at({ii, size.y - 1}).d);
            r = min(r, I.at({size.x - 1, ii}).d);
        }
        return r;
    }

    void trace(ucoo z, Color c = RED) {
        while (at(z) != c) {
            put(z, c);
            z = I.at(z).next;
        }
    }

    void ball() {
        double const r = radius();
        for (auto z : coo_range(size))
            if (I.at(z).d <= r) put(z, Color(0, 0, 127 + at(z).b / 2));
    }

    Array<Info> I;
    double      g, minf, maxf;
    int         n;
};

auto main(int argc, char **argv) -> int {
    Hub const H("Random 2D geometry", argc, argv, "w=free,n=9,z=0,g=1,s=0,b,i,q,c,l=10,a=1");
    if (unsigned const s = H['s']) prng.seed(s);
    unsigned n = H['n'], nn = 1U << n;

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
