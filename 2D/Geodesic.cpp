#include <fftw3.h>
#include <limits>
#include <map>
#include <queue>
#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/math/math.h>
#include <vb/util/CLP.h>
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
    explicit QG(const string &title, int n_, double g_, const string &w, int z_param, double l, double a, bool c)
        : Image(title, {1U << unsigned(n_), 1U << unsigned(n_)}), I(size, Info({0, 0}, {0, 0}, 0, 0)), g(g_), n(n_) {
        map<string, function<void()>> fields;
        fields["boolean"]  = [&, this] { fill_boolean(z_param); };
        fields["dyadic"]   = [&, this] { fill_dyadic(z_param); };
        fields["free"]     = [&, this] { fill_free(z_param); };
        fields["gaussian"] = [&, this] { fill_radial([](double x) { return exp(-x * x); }, l); };
        fields["power"]    = [&, this, a] { fill_radial([a](double x) { return pow(1 + x * x, -a / 2); }, l); };
        fields["white"]    = [&, this] { fill_white(); };
        fields[w]();

        minf = maxf = I.at({0, 0}).f;
        for (auto zz : coo_range(I.size)) {
            minf = min(minf, I.at(zz).f);
            maxf = max(maxf, I.at(zz).f);
        }
        spdlog::info("Renormalized field: min = {}, max = {}", minf / log(size.x), maxf / log(size.x));

        for (auto zz : coo_range(size)) {
            put(zz, Grey(uint8_t(255 * (I.at(zz).f - minf) / (maxf - minf))));
            if (c) put(zz, Indexed(int(at(zz)) > 128 ? 1 : 2));
            I.at(zz) = Info(zz, zz, numeric_limits<double>::infinity(), exp(g * I.at(zz).f));
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
    CLP  clp(argc, argv, "Random 2D geometry");
    auto w = clp.param("w", "free"s, "field type (free|boolean|dyadic|gaussian|power|white)");
    auto n = clp.param("n", 9, "log2 of grid size");
    auto z = clp.param("z", 0, "field parameter");
    auto g = clp.param("g", 1.0, "gamma parameter");
    auto s = clp.param("s", size_t(0), "random seed");
    auto b = clp.flag("b", "show ball");
    auto i = clp.flag("i", "invisible mode");
    auto q = clp.flag("q", "quick mode (skip dijkstra)");
    auto c = clp.flag("c", "color mode");
    auto l = clp.param("l", 10.0, "length scale");
    auto a = clp.param("a", 1.0, "power exponent");
    clp.finalize();

    if (s != 0) prng.seed(s);
    unsigned nn = 1U << n;

    QG img(clp.title, n, g, w, z, l, a, c);
    if (!i) img.show();

    if (!q) {
        img.dijkstra();
        if (b) img.ball();
        for (unsigned ii = 0; ii <= nn - 1; ii += 1) {
            img.trace({0, ii});
            img.trace({nn - 1, ii});
            img.trace({ii, 0});
            img.trace({ii, nn - 1});
        }
    }

    if (!i) { img.pause(); }
    img.output(clp.title);
}
