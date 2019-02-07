#include <vb/Sphere.h>
#include <vb/util/PRNG.h>

vb::Color f_to_c(double f) {
    if (isnan(f)) { return vb::RED; }
    if (f > 0) { return vb::Indexed(1); }
    return vb::Indexed(2);
}

double legendre_p(int l, int m, double x) {
    // l>=0, 0<=m<=l, -1<=x<=1
    double p0 = pow(1 - x * x, m / 2.0);
    if ((m % 2) != 0) { p0 *= -1; }
    if (m == l) { return p0; }
    double p1 = x * (2 * m + 1) * p0;
    for (int n = m + 1; n < l; ++n) { std::tie(p0, p1) = std::make_tuple(p1, ((2 * n + 1) * x * p1 - (n + m) * p0) / (n + 1 - m)); }
    return p1;
}

double legendre_p(int l, double x) { return legendre_p(l, 0, x); }

vb::cpx spherical_harmonic(int n, int m, double theta, double phi) { // 0<=m<=n, 0<=theta<=pi, 0<=phi<=2pi
    return vb::cpx{cos(m * phi), sin(m * phi)} * legendre_p(n, m, cos(theta));
}

class Wave : public vb::Sphere {
public:
    Wave(const vb::Hub &H, int n, int w) : vb::Sphere(H, w, [this](double theta, double phi) { return f_to_c(v(theta, phi)); }), n(n) {
        detail = 2.0 / n;
        for (int m = 0; m <= n; ++m) {
            vb::cpx am{vb::prng.gaussian(), vb::prng.gaussian()};
            am *= sqrt((2 * n + 1) / (4 * M_PI));
            for (int i = 1; i <= m; ++i) { am *= double(2 * i - 1) / sqrt(double(2 * i - 1 + n - m) * double(2 * i + n - m)); }
            a.push_back(am);
        }
    };

    double v(double theta, double phi) {
        double harm = 0;
        for (int m = 0; m <= n; ++m) { harm += (real(a[m]) * cos(m * phi) - imag(a[m]) * sin(m * phi)) * legendre_p(n, m, cos(theta)); }
        return harm;
    }

    int                  n;
    std::vector<vb::cpx> a;
};

class Bargman : public vb::Sphere {
public:
    Bargman(const vb::Hub &H, int n, int w)
        : vb::Sphere(H, w, [this](double x, double y, double z) { return f_to_c(v(x, y, z)); }), a(n + 1), b(n + 1), c(n + 1), sqsq(n + 1),
          n(n) {
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n - i; ++j) { a[i].push_back(vb::prng.gaussian()); }
        }
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n - i; ++j) { b[i].push_back(a[i][n - i - j]); }
        }
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n - i; ++j) { c[i].push_back(a[n - i - j][j]); }
        }
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n - i; ++j) { eps = std::max(eps, abs(a[i][j])); }
        }
        for (int i = 0; i <= n; ++i) {
            for (int j = 0; j <= n; ++j) { sqsq[i].push_back(sqrt(i) / sqrt(j)); }
        }
        eps *= double(H['e']);
    }

    double vv(const std::vector<std::vector<double>> &a, double x, double y, double z) {
        auto i0 = int(n * x * x), j0 = int(n * y * y);
        i0 -= i0 % 2;
        j0 -= j0 % 2;
        double out = 0, t = 1;
        for (int i = i0; i <= n; ++i) {
            double tt = t;
            for (int j = j0; j <= n - i; ++j) {
                out += a[i][j] * tt;
                tt *= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (int j = j0 - 1; j >= 0; --j) {
                tt /= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
                out += a[i][j] * tt;
            }
            t *= (x / z) * sqsq[n - i - j0][i + 1];
            if (abs(t) < eps) { break; }
        }
        t = 1;
        for (int i = i0 - 1; i >= 0; --i) {
            t /= (x / z) * sqsq[n - i - j0][i + 1];
            if (abs(t) < eps) { break; }
            double tt = t;
            for (int j = j0; j <= n - i; ++j) {
                out += a[i][j] * tt;
                tt *= (y / z) * sqsq[n - i - j][j + 1];
                if (abs(tt) < eps) { break; }
            }
            tt = t;
            for (int j = j0 - 1; j >= 0; --j) {
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
    int                              n;
};

int main(int argc, char **argv) {
    vb::Hub H("Random wave on the sphere", argc, argv, "n=50,p,s=0,t=wave,w=800,e=.001");
    int     s = H['s'];
    if (s != 0) { vb::prng.seed(s); }
    if (H['t'] == "wave") {
        Wave F(H, H['n'], H['w']);
        F.show();
        if (H['p']) {
            F.pause();
        } else {
            F.output(H);
        }
    } else if (H['t'] == "barg") {
        Bargman F(H, H['n'], H['w']);
        F.show();
        if (H['p']) {
            F.pause();
        } else {
            F.output(H);
        }
    } else {
        H.L->error("Type should be 'wave' or 'barg', exiting...");
        exit(1);
    }
}
