#include <vb/Coloring.h>
#include <vb/PRNG.h>

using namespace std; using namespace vb;

#include <boost/math/special_functions/factorials.hpp>

class Field : public Coloring { public:
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

        double p0 = df[m] * pow(1-x*x, m/2.0); if (m&1) p0 *= -1; if (m == l) return p0;
        double p1 = x * (2*m+1) * p0;
        for (int n=m+1; n<l; ++n) { std::swap(p0, p1); p1 = legendre_next(n, m, x, p0, p1); }
        return p1;
    }

    cpx spherical_harmonic (int n, int m, double theta, double phi) { // 0<=m<=n, 0<=theta<=pi, 0<=phi<=2pi
        return cpx { cos(m*phi), sin(m*phi) } * p[m] * legendre_p (n, m, cos(theta));
    }

    Field (int n) : Coloring ({-1.0,-1.0},{1.0,1.0},800,[this](cpx z){return c(z);}), n(n){
        for (int i=0; i<=n; ++i)  a.push_back ({prng.gaussian(),prng.gaussian()});
        for (int m=0; m<=n; ++m)  p.push_back (sqrt(boost::math::tgamma_delta_ratio (double(n-m+1), double(2*m)) * (2*n+1) / (4*M_PI)));
        for (int m=0; m<=n; ++m) df.push_back (m==0 ? 1 : boost::math::double_factorial<double>(2*m-1));
    };

    double v (double theta, double phi) {
        double harm = 0;
        for (int m=0; m<=n; ++m) harm += real(a[m] * spherical_harmonic (n,m,theta,phi));
        return real(harm);
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
    vector<double> p, df;
};

int main (int argc, char ** argv) {
    H.init ("Random wave on the sphere", argc, argv, "n=50,p");
    Field F (H['n']); F.show(); F.output(); if (H['p']) F.pause();
}
