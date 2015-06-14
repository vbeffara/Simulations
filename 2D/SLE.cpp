#include <vb/Hub.h>
#include <vb/Image.h>
using namespace vb; using namespace std;

const Color INSIDE(0), LEFTSIDE(HSV(0,.8,.8)), RIGHTSIDE(HSV(.5,.8,.8));

class Loewner : public std::vector<double> { public:
    void bound() {
        Max=*this; Min=*this;
        for (int i=size()-2; i>=0; i--) { Min[i] = min (at(i), Min[i+1]); Max[i] = max (at(i), Max[i+1]); }
    }

    Color compute (cpx z) const {
        for (int k=0; k<size(); ++k) {
            if (z.real() < Min[k]) return LEFTSIDE;
            if (z.real() > Max[k]) return RIGHTSIDE;
            if (z.imag()<=0) return (z.real() < at(k)) ? LEFTSIDE : RIGHTSIDE;
            z += 2 * dt / (z-at(k));
        }
        return (z.real() < back()) ? LEFTSIDE : RIGHTSIDE;
    }

    double dt = 1;
    std::vector<double> Min,Max;
};

class Coloring : public Image { public:
    Coloring (std::function <Color(cpx)> f_, int w_, int h_, cpx c_, double s_) : Image(w_,h_), cc(c_), ss(s_), f(f_) {}

    cpx c_to_z (coo c) { cpx z (c.x,c.y); z -= cpx(w()/2,h()/2); z /= ss; z += cc; return z; }

    std::function <Color(coo)> ff () { return [&](coo c) { return f(c_to_z(c)); }; };

    std::function <Color(coo)> aa () { return [&](coo c) {
        cpx z { c_to_z(c) }; int r{0}, g{0}, b{0}, a{0};
        for (int i=-1; i<=1; ++i) {
            for (int j=-1; j<=1; ++j) {
                Color C { f(z + cpx(i,j)/ss/3.0) };
                r+=C.r; g+=C.g; b+=C.b; a+=C.a;
            }
        }
        return Color (r/9,g/9,b/9,a/9);
    }; }

    cpx cc; double ss;
    std::function <Color(cpx)> f;
};

class SLE : public Coloring { public:
    SLE (int n, double k) : Coloring ([&](cpx z){return W.compute(z);}, 2*n, n, cpx(0,sqrt(k)/2), int(H['n'])/sqrt(k)) {
        for (int i=0; i<w(); i++) put (coo(i,0),INSIDE);
        W.push_back(0); W.dt = 1.0/(h()*h());
        for (int i=1; i<h()*h(); i++) W.push_back (W.back() + sqrt(k * W.dt) * prng.gaussian());
        W.bound();
    }

    void edge_detect () {
        for (int y=1; y<h(); y++) {
            for (int x=0; x<w()-1; x++) {
                Color self  = at(coo(x,y));
                Color right = (x<w()-1) ? at(coo(x+1,y)) : self;
                Color down  = (y<h()-1) ? at(coo(x,y+1)) : self;
                if ((self!=right)||(self!=down)) put(coo(x,y),INSIDE);
            }
        }
    }

    Loewner W;
};

int main (int argc, char ** argv) {
    H.init ("Schramm-Loewner Evolution", argc,argv, "n=300,k=2.666666666667,r=0,a");
    int r = H['r']; if (r) prng.seed(r);

    SLE sle (H['n'],H['k']); sle.show();
    sle.tessel ({0,1},{sle.w()-1,sle.h()-1}, H['a'] ? sle.aa() : sle.ff());
    if (!H['a']) sle.edge_detect();
    sle.output_png();
}
