#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <fstream>

using namespace vb; using namespace std;

struct state {
    static double a,b;
    double v,x;
    void step () { x = a*x+b; while (x>=1) x -= 1; }
};
double state::a, state::b;

class Model : public vector<state> { public:
    Model (int n) : vector<state> (n,{0,0}) { for (auto & s : *this) s.x = prng.uniform_real(); }

    void rotate (int k, double a) {
        double c=cos(a), s=sin(a), &x=at(k).v, &y=at((k+1)%size()).v;
        tie(x,y) = make_tuple (c*x-s*y, s*x+c*y);
    }

    void step (int k) { rotate (k,at(k).x*2*M_PI); at(k).step(); }

    void swipe () {
        for (unsigned k=0; k<size(); k+=2) step(k);
        for (unsigned k=1; k<size(); k+=2) step(k);
    }
};

int main (int argc, char ** argv) {
    H.init ("Deterministic Fourier law", argc, argv, "a=2,b=.2394879347,n=500,m=1000,t=100");
    int nn=H['n'], mm=H['m'], tt=H['t']; state::a=H['a']; state::b=H['b'];
    vector<double> profile(nn,0), boltzmann(mm,0), var(tt,0);

    ProgressBar PB (mm);
    for (int i=0; i<mm; ++i) {
        Model M(nn); M[nn/2].v = 1;
        for (int t=0; t<tt; ++t) {
            PB.set(i);
            M.swipe();
            double v=0; for (int k=0; k<nn; ++k) v += (k-nn/2)*(k-nn/2)*M[k].v*M[k].v; var[t]+=v;
        }
        for (int k=0; k<nn; ++k) profile[k] += M[k].v*M[k].v;
        boltzmann[i] = M[nn/2].v;
    }

    { ofstream of("out.profile"); for (auto u : profile) of << u << endl; }
    { ofstream of("out.variance"); for (auto u : var) of << u << endl; }
    {
        int nclass = sqrt(double(H['m']));
        double bmin=0, bmax=0; for (auto b : boltzmann) { bmin=min(bmin,b); bmax=max(bmax,b); }
        vector<int> data(nclass); for (auto b : boltzmann) data[(nclass-.01)*(b-bmin)/(bmax-bmin)]++;
        ofstream of("out.boltzmann"); for (int i=0; i<nclass; ++i) of << bmin+i*(bmax-bmin)/nclass << " " << data[i] << endl;
    }
}
