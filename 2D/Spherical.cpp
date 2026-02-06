#include <cmath>
#include <spdlog/spdlog.h>
#include <vb/Sphere.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

auto f_to_c(double f) -> vb::Color {
    if (std::isnan(f)) { return vb::RED; }
    if (f > 0) { return vb::Indexed(1); }
    return vb::Indexed(2);
}

auto legendre_p(size_t l, size_t m, double x) -> double {
    // l>=0, 0<=m<=l, -1<=x<=1
    double p0 = pow(1 - x * x, double(m) / 2.0);
    if ((m % 2) != 0) { p0 *= -1; }
    if (m == l) { return p0; }
    double p1 = x * double(2 * m + 1) * p0;
    for (auto n = m + 1; n < l; ++n) {
        p0 = (double(2 * n + 1) * x * p1 - double(n + m) * p0) / double(n + 1 - m);
        std::swap(p0, p1);
    }
    return p1;
}

auto legendre_p(size_t l, double x) -> double { return legendre_p(l, 0, x); }

auto spherical_harmonic(size_t n, size_t m, double theta, double phi) -> vb::cpx { // 0<=m<=n, 0<=theta<=pi, 0<=phi<=2pi
    return vb::cpx{cos(double(m) * phi), sin(double(m) * phi)} * legendre_p(n, m, cos(theta));
}

class Wave : public vb::Sphere {
public:
    Wave(const std::string &title, size_t nn, size_t w)
        : vb::Sphere(title, w, [this](double theta, double phi) { return f_to_c(v(theta, phi)); }), n(nn) {
        detail = 2.0 / double(n);
        for (size_t m = 0; m <= n; ++m) {
            vb::cpx am{vb::prng.gaussian(), vb::prng.gaussian()};
            am *= sqrt(double(2 * n + 1) / (4 * M_PI));
            for (size_t ii = 1; ii <= m; ++ii) { am *= double(2 * ii - 1) / sqrt(double(2 * ii - 1 + n - m) * double(2 * ii + n - m)); }
            a.push_back(am);
        }
    };

    auto v(double theta, double phi) -> double {
        double harm = 0;
        for (size_t m = 0; m <= n; ++m) {
            harm += (real(a[m]) * cos(double(m) * phi) - imag(a[m]) * sin(double(m) * phi)) * legendre_p(n, m, cos(theta));
        }
        return harm;
    }

    size_t               n;
    std::vector<vb::cpx> a;
};

class Bargman : public vb::Sphere {
public:
    Bargman(const std::string &title, size_t nn, size_t w, double epsilon)
        : vb::Sphere(title, w, [this](double x, double y, double z) { return f_to_c(v(x, y, z)); }), a(nn + 1), b(nn + 1), c(nn + 1),
          sqsq(nn + 1), n(nn) {
        for (size_t ii = 0; ii <= n; ++ii) {
            for (size_t j = 0; j <= n - ii; ++j) { a[ii].push_back(vb::prng.gaussian()); }
        }
        for (size_t ii = 0; ii <= n; ++ii) {
            for (size_t j = 0; j <= n - ii; ++j) { b[ii].push_back(a[ii][n - ii - j]); }
        }
        for (size_t ii = 0; ii <= n; ++ii) {
            for (size_t j = 0; j <= n - ii; ++j) { c[ii].push_back(a[n - ii - j][j]); }
        }
        for (size_t ii = 0; ii <= n; ++ii) {
            for (size_t j = 0; j <= n - ii; ++j) { eps = std::max(eps, abs(a[ii][j])); }
        }
        for (size_t ii = 0; ii <= n; ++ii) {
            for (size_t j = 0; j <= n; ++j) { sqsq[ii].push_back(sqrt(double(ii)) / sqrt(double(j))); }
        }
        eps *= epsilon;
    }

    auto vv(const std::vector<std::vector<double>> &aa, double x, double y, double z) -> double {
        auto i0 = size_t(double(n) * x * x), j0 = size_t(double(n) * y * y);
        i0 -= i0 % 2;
        j0 -= j0 % 2;
        double out = 0, t = 1;
        for (size_t ii = i0; ii <= n; ++ii) {
            double tt = t;
            for (size_t j = j0; j <= n - ii; ++j) {
                out += aa[ii][j] * tt;
                tt *= (y / z) * sqsq[n - ii - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (size_t j = j0; j-- > 0;) {
                tt /= (y / z) * sqsq[n - ii - j][j + 1];
                if (abs(tt) < eps) { break; }
                out += aa[ii][j] * tt;
            }
            t *= (x / z) * sqsq[n - ii - j0][ii + 1];
            if (abs(t) < eps) { break; }
        }
        t = 1;
        for (size_t ii = i0; ii-- > 0;) {
            t /= (x / z) * sqsq[n - ii - j0][ii + 1];
            if (abs(t) < eps) { break; }
            double tt = t;
            for (size_t j = j0; j <= n - ii; ++j) {
                out += aa[ii][j] * tt;
                tt *= (y / z) * sqsq[n - ii - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (size_t j = j0; j-- > 0;) {
                tt /= (y / z) * sqsq[n - ii - j][j + 1];
                if (abs(tt) < eps) { break; }
                out += aa[ii][j] * tt;
            }
        }
        if (((n % 2) != 0) && (z < 0)) { out = -out; }
        return out;
    }

    auto v(double x, double y, double z) -> double {
        if ((abs(z) > abs(x)) && (abs(z) > abs(y))) { return vv(a, x, y, z); }
        if (abs(y) > abs(x)) { return vv(b, x, z, y); }
        return vv(c, z, y, x);
    }

    std::vector<std::vector<double>> a, b, c, sqsq;
    double                           eps = 0;
    size_t                           n;
};

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "Random wave on the sphere");
    auto n = clp.param("n", size_t(50), "Degree of harmonic");
    auto p = clp.flag("p", "Pause at end");
    auto s = clp.param("s", size_t(0), "Random seed");
    auto t = clp.param("t", std::string("wave"), "Type: wave or barg");
    auto w = clp.param("w", size_t(800), "Window size");
    auto e = clp.param("e", 0.001, "Epsilon for Bargman");
    clp.finalize();

    if (s != 0) { vb::prng.seed(s); }
    if (t == "wave") {
        Wave F(clp.title, n, w);
        F.show();
        if (p) {
            F.pause();
        } else {
            F.output(clp.title);
        }
    } else if (t == "barg") {
        Bargman F(clp.title, n, w, e);
        F.show();
        if (p) {
            F.pause();
        } else {
            F.output(clp.title);
        }
    } else {
        spdlog::error("Type should be 'wave' or 'barg', exiting...");
        return 1;
    }
}
