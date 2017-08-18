#include <vb/Coloring.h>
#include <vb/PRNG.h>

using namespace std; using namespace vb;

class Sphere : public Coloring { public:
    Sphere (function <Color(cpx)> f) : Coloring ({-1.0,-1.0},{1.0,1.0},800,f) {}

    Sphere (function <Color(double,double,double)> f) : Sphere ([f](cpx z){
        double x1=real(z), x2=imag(z), n = x1*x1 + x2*x2; if (n>1) return NOCOLOR;
        double x3=sqrt(1-n); return f(x1,x2,x3);
    }) {}

    Sphere (function <Color(double,double)> f) : Sphere ([f](cpx z){
        double x1=real(z), x2=imag(z), n = x1*x1 + x2*x2; if (n>1) return NOCOLOR;
        double theta=asin(sqrt(n)), phi=atan2(x1,x2); return f(theta,phi);
    }) {}

    void show () override {
        Coloring::show();
        for (int i=0; i<pixel_w(); ++i) for (int j=0; j<pixel_h(); ++j) {
            cpx z = c_to_z(coo(i,j)); Color & c = at(coo(i,j));
            double f=1, x1=real(z), x2=imag(z), z2=x1*x1+x2*x2;
            if (z2<=1) {
                double x3=sqrt(1-z2), y1=.3, y2=-.2, y3=sqrt(1-y1*y1-y2*y2);
                double xy = x1*y1 + x2*y2 + x3*y3; f = xy>0 ? xy : 0; f = .1 + .9*f*f;
            }
            f = f*c.a/255; c.r = f*c.r + 255-c.a; c.g = f*c.g + 255-c.a; c.b = f*c.b + 255-c.a; c.a=255;
        }
        update();
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

class Wave : public Sphere { public:
    Wave (int n) : Sphere ([this](double theta, double phi){ return Indexed (v(theta,phi)>0 ? 1 : 2); }), n(n) {
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
    Bargman (int n) : Sphere ([this](double x, double y, double z){ return Indexed (v(x,y,z)>0 ? 1 : 2); }), a(n+1), n(n) {
        for (int i=0; i<=n; ++i) for (int j=0; j<=n-i; ++j) {
            double aa = prng.gaussian() * sqrt((n+1)*(n+2)); int kk=1;
            for (int k=1; k<=i; ++k, ++kk) aa *= sqrt(double(kk)/double(k));
            for (int k=1; k<=j; ++k, ++kk) aa *= sqrt(double(kk)/double(k));
            for (int k=1; k<=n-i-j; ++k, ++kk) aa *= sqrt(double(kk)/double(k));
            a[i].push_back (aa);
        }
    }

    double v (double x, double y, double z) {
        double out=0, xz=1;
        for (int i=0; i<=n; ++i, xz*=x/z) { double yz=1; for (int j=0; j<=n-i; ++j, yz*=y/z) out += a[i][j] * xz * yz; }
        return out * pow(z,n);
    }

    vector<vector<double>> a;
    int n;
};

int main (int argc, char ** argv) {
    H.init ("Random wave on the sphere", argc, argv, "n=50,p,s=0,t=wave");
    int s = H['s']; if (s) prng.seed(s);
    if (H['t'] == "wave") { Wave F (H['n']); F.show(); if (H['p']) F.pause(); else F.output(); } else
    if (H['t'] == "barg") { Bargman F (H['n']); F.show(); if (H['p']) F.pause(); else F.output(); }
}
