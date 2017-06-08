#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

struct state { double v,x; };

class Model : public vector<state> { public:
    Model (int n) : vector<state> (n,{0,0}) {
        for (unsigned k=0; k<size(); ++k) at(k).x = prng.uniform_real();
    }

    void step (int k) {
        double a=at(k).x*2*M_PI, c=cos(a), s=sin(a);
        double z=at(k).v;
        int l = (k+1)%size();
        at(k).v = c*z + s*at(l).v; at(l).v = c*at(l).v - s*z;
        at(k).x = 2*at(k).x + .394875; while (at(k).x>=1) at(k).x -= 1;
    }

    void swipe () {
        for (unsigned k=0; k<size(); k+=2) step(k);
        for (unsigned k=1; k<size(); k+=2) step(k);
    }
};

int main (int argc, char ** argv) {
    H.init ("Deterministic Fourier law", argc, argv, "n=500");
    int n=H['n']; vector<double> data (n,0);
    for (int i=0; i<1000; ++i) {
        Model M(n); M[n/2].v = 1;
        for (int t=0; t<100; ++t) M.swipe();
        for (int k=0; k<n; ++k) data[k] += M[k].v*M[k].v;
    }

    for (auto u : data) cout << u << endl;
}
