#include <vb/Sphere.h>
#include <vb/PRNG.h>

using namespace std; using namespace vb;

double legendre_next (int l,        double x, double Pl, double Plm1) { return ((2*l+1)*x*Pl - l*Plm1) / (l+1); }
double legendre_next (int l, int m, double x, double Pl, double Plm1) { return ((2*l+1)*x*Pl - (l+m)*Plm1) / (l+1-m); }

double legendre_p (int l, double x) { // l>=0, -1<=x<=1
    if (l==0) return 1;
    double p0=1, p1=x;
    for (int n=1; n<l; ++n) { std::swap(p0, p1); p1 = legendre_next(n, x, p0, p1); }
    return p1;
}

double legendre_p (int l, int m, double x) { // l>=0, 0<=m<=l, -1<=x<=1
    if (m==0) return legendre_p (l, x);

    double p0 = pow(1-x*x, m/2.0); if (m&1) p0 *= -1; if (m == l) return p0;
    double p1 = x * (2*m+1) * p0;
    for (int n=m+1; n<l; ++n) { std::swap(p0, p1); p1 = legendre_next(n, m, x, p0, p1); }
    return p1;
}

cpx spherical_harmonic (int n, int m, double theta, double phi) { // 0<=m<=n, 0<=theta<=pi, 0<=phi<=2pi
    return cpx { cos(m*phi), sin(m*phi) } * legendre_p (n, m, cos(theta));
}

class Wave : public Sphere { public:
    Wave (int n, int w) : Sphere (w,[this](double theta, double phi){ return Indexed (v(theta,phi)>0 ? 1 : 2); }), n(n) {
        detail=2.0/n;
        for (int m=0; m<=n; ++m) {
            cpx am {prng.gaussian(),prng.gaussian()};
            am *= sqrt((2*n+1) / (4*M_PI));
            for (int i=1; i<=m; ++i) am *= double(2*i-1) / sqrt(double(2*i-1+n-m)*double(2*i+n-m));
            a.push_back(am);
        }
    };

    double v (double theta, double phi) {
        double harm = 0;
        for (int m=0; m<=n; ++m) harm += (real(a[m])*cos(m*phi) - imag(a[m])*sin(m*phi)) * legendre_p (n, m, cos(theta));
        return harm;
    }

    int n;
    vector<cpx> a;
};

class Bargman : public Sphere { public:
    Bargman (int n, int w) : Sphere (w,[this](double x, double y, double z){ return Indexed (v(x,y,z)>0 ? 1 : 2); }), a(n+1), b(n+1), c(n+1), n(n) {
        for (int i=0; i<=n; ++i) for (int j=0; j<=n-i; ++j) a[i].push_back(prng.gaussian());
        for (int i=0; i<=n; ++i) for (int j=0; j<=n-i; ++j) b[i].push_back(a[i][n-i-j]);
        for (int i=0; i<=n; ++i) for (int j=0; j<=n-i; ++j) c[i].push_back(a[n-i-j][j]);
        for (int i=0; i<=n; ++i) for (int j=0; j<=n-i; ++j) eps = max (eps, abs(a[i][j])); eps *= double(H['e']);
    }

    double vv (vector<vector<double>> &a, double x, double y, double z) {
        int i0 = n * (x*x) / (z*z+x*x); i0 -= i0%2;
        int j0 = n * (y*y) / (z*z+y*y); j0 -= j0%2;
        double out=0, t=1;
        for (int i=i0; i<=n; ++i) {
            double tt = t;
            for (int j=j0; j<=n-i; ++j) {
                out += a[i][j] * tt;
                tt *= (y/z) * sqrt(n-i-j) / sqrt(j+1); if (abs(tt) < eps) break;
            }
            tt = t;
            for (int j=j0-1; j>=0; --j) {
                tt /= (y/z) * sqrt(n-i-j) / sqrt(j+1); if (abs(tt) < eps) break;
                out += a[i][j] * tt;
            }
            t *= (x/z) * sqrt(n-i-j0) / sqrt(i+1); if (abs(t) < eps) break;
        }
        t=1;
        for (int i=i0-1; i>=0; --i) {
            t /= (x/z) * sqrt(n-i-j0) / sqrt(i+1); if (abs(t) < eps) break;
            double tt = t;
            for (int j=j0; j<=n-i; ++j) {
                out += a[i][j] * tt;
                tt *= (y/z) * sqrt(n-i-j) / sqrt(j+1); if (abs(tt) < eps) break;
            }
            tt = t;
            for (int j=j0-1; j>=0; --j) {
                tt /= (y/z) * sqrt(n-i-j) / sqrt(j+1); if (abs(tt) < eps) break;
                out += a[i][j] * tt;
            }
        }
        if ((n%2)&&(z<0)) out = -out;
        return out;
    }

    double v (double x, double y, double z) {
        if ((abs(z)>abs(x)) && (abs(z)>abs(y))) return vv(a,x,y,z);
        else if (abs(y)>abs(x)) return vv(b,x,z,y);
        else return vv(c,z,y,x);
    }

    vector<vector<double>> a,b,c;
    double eps=0;
    int n;
};

int main (int argc, char ** argv) {
    H.init ("Random wave on the sphere", argc, argv, "n=50,p,s=0,t=wave,w=800,e=.000001");
    int s = H['s']; if (s) prng.seed(s);
    if (H['t'] == "wave") { Wave F (H['n'],H['w']); F.show(); if (H['p']) F.pause(); else F.output(); } else
    if (H['t'] == "barg") { Bargman F (H['n'],H['w']); F.show(); if (H['p']) F.pause(); else F.output(); } else
    { H.L->error ("Type should be 'wave' or 'barg', exiting..."); exit(1); }
}
