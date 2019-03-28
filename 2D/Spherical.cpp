#include <spdlog/spdlog.h>
#include <vb/Sphere.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

vb::Color f_to_c(double f) {
    if (isnan(f)) { return vb::RED; }
    if (f > 0) { return vb::Indexed(1); }
    return vb::Indexed(2);
}

double legendre_p(size_t l, size_t m, double x) {
    // l>=0, 0<=m<=l, -1<=x<=1
    double p0 = pow(1 - x * x, m / 2.0);
    if ((m % 2) != 0) { p0 *= -1; }
    if (m == l) { return p0; }
    double p1 = x * double(2 * m + 1) * p0;
    for (auto n = m + 1; n < l; ++n) {
        // TODO: swap would be clearer
        std::tie(p0, p1) = std::make_tuple(p1, (double(2 * n + 1) * x * p1 - double(n + m) * p0) / double(n + 1 - m));
    }
    return p1;
}

double legendre_p(size_t l, double x) { return legendre_p(l, 0, x); }

vb::cpx spherical_harmonic(size_t n, size_t m, double theta, double phi) { // 0<=m<=n, 0<=theta<=pi, 0<=phi<=2pi
    return vb::cpx{cos(m * phi), sin(m * phi)} * legendre_p(n, m, cos(theta));
}

class Wave : public vb::Sphere {
public:
    Wave(const vb::Hub &H, size_t n, size_t w)
        : vb::Sphere(H.title, w, [this](double theta, double phi) { return f_to_c(v(theta, phi)); }), n(n) {
        detail = 2.0 / n;
        for (size_t m = 0; m <= n; ++m) {
            vb::cpx am{vb::prng.gaussian(), vb::prng.gaussian()};
            am *= sqrt(double(2 * n + 1) / (4 * M_PI));
            for (size_t i = 1; i <= m; ++i) { am *= double(2 * i - 1) / sqrt(double(2 * i - 1 + n - m) * double(2 * i + n - m)); }
            a.push_back(am);
        }
    };

    double v(double theta, double phi) {
        double harm = 0;
        for (size_t m = 0; m <= n; ++m) { harm += (real(a[m]) * cos(m * phi) - imag(a[m]) * sin(m * phi)) * legendre_p(n, m, cos(theta)); }
        return harm;
    }

    size_t               n;
    std::vector<vb::cpx> a;
};

class Bargman : public vb::Sphere {
public:
    Bargman(const vb::Hub &H, size_t n, size_t w)
        : vb::Sphere(H.title, w, [this](double x, double y, double z) { return f_to_c(v(x, y, z)); }), a(n + 1), b(n + 1), c(n + 1),
          sqsq(n + 1), n(n) {
        for (size_t i = 0; i <= n; ++i) {
            for (size_t j = 0; j <= n - i; ++j) { a[i].push_back(vb::prng.gaussian()); }
        }
        for (size_t i = 0; i <= n; ++i) {
            for (size_t j = 0; j <= n - i; ++j) { b[i].push_back(a[i][n - i - j]); }
        }
        for (size_t i = 0; i <= n; ++i) {
            for (size_t j = 0; j <= n - i; ++j) { c[i].push_back(a[n - i - j][j]); }
        }
        for (size_t i = 0; i <= n; ++i) {
            for (size_t j = 0; j <= n - i; ++j) { eps = std::max(eps, abs(a[i][j])); }
        }
        for (size_t i = 0; i <= n; ++i) {
            for (size_t j = 0; j <= n; ++j) { sqsq[i].push_back(sqrt(i) / sqrt(j)); }
        }
        eps *= double(H['e']);
    }

    double vv(const std::vector<std::vector<double>> &a, double x, double y, double z) {
        auto i0 = size_t(n * x * x), j0 = size_t(n * y * y);
        i0 -= i0 % 2;
        j0 -= j0 % 2;
        double out = 0, t = 1;
        for (size_t i = i0; i <= n; ++i) {
            double tt = t;
            for (size_t j = j0; j <= n - i; ++j) {
                out += a[i][j] * tt;
                tt *= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (size_t j = j0; j-- > 0;) {
                tt /= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
                out += a[i][j] * tt;
            }
            t *= (x / z) * sqsq[n - i - j0][i + 1];
            if (abs(t) < eps) { break; }
        }
        t = 1;
        for (size_t i = i0; i-- > 0;) {
            t /= (x / z) * sqsq[n - i - j0][i + 1];
            if (abs(t) < eps) { break; }
            double tt = t;
            for (size_t j = j0; j <= n - i; ++j) {
                out += a[i][j] * tt;
                tt *= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (size_t j = j0; j-- > 0;) {
                tt /= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
                out += a[i][j] * tt;
            }
        }
        if (((n % 2) != 0) && (z < 0)) { out = -out; }
        return out;
    }

    double v(double x, double y, double z) {
        if ((abs(z) > abs(x)) && (abs(z) > abs(y))) { return vv(a, x, y, z); }
        if (abs(y) > abs(x)) { return vv(b, x, z, y); }
        return vv(c, z, y, x);
    }

    std::vector<std::vector<double>> a, b, c, sqsq;
    double                           eps = 0;
    size_t                           n;
};

int main(int argc, char **argv) {
    vb::Hub H("Random wave on the sphere", argc, argv, "n=50,p,s=0,t=wave,w=800,e=.001");
    if (size_t s = H['s']; s != 0) { vb::prng.seed(s); }
    if (H['t'] == "wave") {
        Wave F(H, H['n'], H['w']);
        F.show();
        if (H['p']) {
            F.pause();
        } else {
            F.output(H.title);
        }
    } else if (H['t'] == "barg") {
        Bargman F(H, H['n'], H['w']);
        F.show();
        if (H['p']) {
            F.pause();
        } else {
            F.output(H.title);
        }
    } else {
        spdlog::error("Type should be 'wave' or 'barg', exiting...");
        exit(1);
    }
}
