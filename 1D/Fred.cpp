#include <fstream>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

struct state {
    static double a,b;
    double v,x;
    void step () { x = a*x+b; while (x>=1) x -= 1; }
};

double state::a, state::b;

class Model : public std::vector<state> { public:
    explicit Model (int n) : std::vector<state> (n,{0,0}) { for (auto & s : *this) s.x = vb::prng.uniform_real(); }

    void rotate (int k, double a) {
        double c=cos(a), s=sin(a), &x=at(k).v, &y=at((k+1)%size()).v;
        std::tie(x,y) = std::make_tuple (c*x-s*y, s*x+c*y);
    }

    void step (int k) { rotate (k,at(k).x*2*M_PI); at(k).step(); }

    void swipe () {
        for (unsigned k=0; k<size(); k+=2) step(k);
        for (unsigned k=1; k<size(); k+=2) step(k);
    }
};

int main (int argc, char ** argv) {
    vb::H.init ("Deterministic Fourier law", argc, argv, "a=2,b=.2394879347,n=500,m=1000,t=100");
    int nn=vb::H['n'], mm=vb::H['m'], tt=vb::H['t']; state::a=vb::H['a']; state::b=vb::H['b'];
    std::vector<double> profile(nn,0), boltzmann(mm,0), var(tt,0);

    vb::ProgressBar PB (mm);
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

    { std::ofstream of("out.profile"); for (auto u : profile) of << u << std::endl; }
    { std::ofstream of("out.variance"); for (auto u : var) of << u << std::endl; }
    {
        int nclass = sqrt(double(vb::H['m']));
        double bmin=0, bmax=0; for (auto b : boltzmann) { bmin=std::min(bmin,b); bmax=std::max(bmax,b); }
        std::vector<int> data(nclass); for (auto b : boltzmann) data[(nclass-.01)*(b-bmin)/(bmax-bmin)]++;
        std::ofstream of("out.boltzmann"); for (int i=0; i<nclass; ++i) of << bmin+i*(bmax-bmin)/nclass << " " << data[i] << std::endl;
    }
}
