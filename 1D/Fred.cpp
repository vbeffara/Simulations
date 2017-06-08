#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <fstream>

using namespace vb; using namespace std;

struct state {
    double v,x;
    void step () { x = 2*x + .394875; while (x>=1) x -= 1; }
};

class Model : public vector<state> { public:
    Model (int n) : vector<state> (n,{0,0}) {
        for (unsigned k=0; k<size(); ++k) at(k).x = prng.uniform_real();
    }

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
    H.init ("Deterministic Fourier law", argc, argv, "n=500,m=1000,t=100");
    int n=H['n']; vector<double> profile(n,0), boltzmann;
    {
        ProgressBar PB (H['m']);
        for (int i=0; i<int(H['m']); ++i) {
            PB.set(i);
            Model M(n); M[n/2].v = 1;
            for (int t=0; t<int(H['t']); ++t) M.swipe();
            for (int k=0; k<n; ++k) profile[k] += M[k].v*M[k].v;
            boltzmann.push_back(M[n/2].v);
        }
    }

    { ofstream of("out.profile"); for (auto u : profile) of << u << endl; }
    {
        int nclass = sqrt(double(H['m']));
        double bmin=0, bmax=0; for (auto b : boltzmann) { bmin=min(bmin,b); bmax=max(bmax,b); }
        vector<int> data(nclass); for (auto b : boltzmann) data[(nclass-.01)*(b-bmin)/(bmax-bmin)]++;
        ofstream of("out.boltzmann"); for (int i=0; i<nclass; ++i) of << bmin+i*(bmax-bmin)/nclass << " " << data[i] << endl;
    }
}
