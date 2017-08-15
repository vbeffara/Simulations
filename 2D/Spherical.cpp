#include <vb/Coloring.h>
#include <vb/PRNG.h>

using namespace std; using namespace vb;

template <typename T> T check (T x, T y) {
    static double merr = -1.0;
    double err = abs(x-y); if (err>merr) {
        merr = err;
        H.L->info ("max error: {}", merr);
    }
    return x;
}

class Field : public Coloring { public:
    Field (int n) : Coloring ({-1.0,-1.0},{1.0,1.0},800,[this](cpx z){return c(z);}), n(n){
        for (int m=0; m<=n; ++m) {
            cpx am {prng.gaussian(),prng.gaussian()};
            am *= sqrt((2*n+1) / (4*M_PI));
            for (int i=1; i<=m; ++i) am *= double(2*i-1) / sqrt(double(2*i-1+n-m)*double(2*i+n-m));
            a.push_back(am);
        }
    };

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

    double v (double theta, double phi) {
        double harm = 0;
        for (int m=0; m<=n; ++m) harm += (real(a[m])*cos(m*phi) - imag(a[m])*sin(m*phi)) * legendre_p (n, m, cos(theta));
        return harm;
    }

    Color c (cpx z) {
        double x1=real(z), x2=imag(z), z2=x1*x1+x2*x2; if (z2>1) return WHITE;
        double x3=sqrt(1-z2), theta=asin(sqrt(z2)), phi=atan2(x1,x2);
        Color out = Indexed (v(theta,phi)>0 ? 1 : 2);

        double y1=.3, y2=.2, y3=sqrt(1-y1*y1-y2*y2);
        double xy = x1*y1 + x2*y2 + x3*y3; double f = xy>0 ? xy : 0; f = .1 + .9*f*f;
        out.r *= f; out.g *= f; out.b *= f; return out;
    }

    int n;
    vector<cpx> a;
};

int main (int argc, char ** argv) {
    H.init ("Random wave on the sphere", argc, argv, "n=50,p");
    Field F (H['n']); F.show(); F.output(); if (H['p']) F.pause();
}
