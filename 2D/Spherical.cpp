#include <vb/Coloring.h>
#include <vb/PRNG.h>
#include <boost/math/special_functions/spherical_harmonic.hpp>

using namespace std; using namespace vb;

class Field : public Coloring { public:
    Field (int n) : Coloring ({-1.0,-1.0},{1.0,1.0},800,[this](cpx z){return c(z);}), n(n){
        for (int i=0; i<=n; ++i) a.push_back ({prng.gaussian(),prng.gaussian()});
    };

    cpx v (double theta, double phi) {
        cpx harm = 0;
        for (int i=0; i<=n; ++i) harm += a[i] * boost::math::spherical_harmonic (n,i,theta,phi);
        return harm;
    }

    Color c (cpx z) {
        double x1=real(z), x2=imag(z), z2=x1*x1+x2*x2; if (z2>1) return WHITE;
        double x3=sqrt(1-z2), theta=asin(sqrt(z2)), phi=atan2(x1,x2);
        Color out = Indexed (real(v(theta,phi))>0 ? 1 : 2);

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
