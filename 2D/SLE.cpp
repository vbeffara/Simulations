#include <vb/Hub.h>
#include <vb/Image.h>
using namespace vb; using namespace std;

const Color INSIDE(0), LEFTSIDE(HSV(0,.8,.8)), RIGHTSIDE(HSV(.5,.8,.8));

class SLE : public Image { public:
    SLE (int n_, double k_) : Image(2*n_,n_), n(h()*h()), kappa(k_), W(n,0), Wmin(n+1,0), Wmax(n+1,0) {
        for (int i=0; i<w(); i++) put (coo(i,0),INSIDE);
        for (int i=1; i<n; i++) W[i] = W[i-1] + prng.gaussian();
        double d = W[n-1]/(n-1); for (int i=1; i<n;i++) W[i] -= i*d;

        Wmax[n]=W[n-1]; Wmin[n]=W[n-1];
        for (int i=n-1; i>0; i--) { Wmin[i] = min (W[i-1], Wmin[i+1]); Wmax[i] = max (W[i-1], Wmax[i+1]); }
        Wmin[0]=Wmin[1]; Wmax[0]=Wmax[1];
    }

    Color compute (cpx z) {
        z -= w()/2;
        int k;

        for (k=0; (k<n) && (z.imag()>0); ++k) {
            if (z.real()<Wmin[k]) return LEFTSIDE;
            if (z.real()>Wmax[k]) return RIGHTSIDE;
            z += (2/kappa) / (z-W[k]);
        }
        return (z.real()<W[k]) ? LEFTSIDE : RIGHTSIDE;
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

    int n;
    double kappa;
    vector<double> W,Wmin,Wmax;
};

int main (int argc, char ** argv) {
    H.init ("Schramm-Loewner Evolution", argc,argv, "n=300,k=2.666666666667,r=0,a");
    int r = H['r']; if (r) prng.seed(r);

    SLE sle (H['n'],H['k']); sle.show();
    if (H['a']) sle.tessel (0,1,sle.w()-1,sle.h()-1, aa<double> ([&](cpx z){ return sle.compute(z); }));
    else { sle.tessel (0,1,sle.w()-1,sle.h()-1, [&](coo c){ return sle.compute({double(c.x),double(c.y)}); }); sle.edge_detect(); }
    sle.output_png();
}
