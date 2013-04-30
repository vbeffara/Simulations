#include "vb/Bitmap.h"
#include "vb/CL_Parser.h"
#include "vb/PRNG.h"

using namespace vb;

const Color C[6] = { RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN };

class Spin {
public:
    int s;
    Spin (int i) : s(i) {};
    operator Color() const { if ((s>=0)&&(s<6)) return C[s]; else return WHITE; }
};

class Potts : public Bitmap<Spin> {
public:
    Potts (CL_Parser & CLP) : Bitmap<Spin> (int(CLP('n')), int(CLP('n')), "Potts model"), q(CLP('q')), beta(CLP('b')), b(0) {
        for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put (coo(i,j), prng.uniform_int(q));
        beta *= log(1+sqrt(double(q)));
    }

    double H (Spin i, Spin j) { return i.s==j.s ? 0 : 1; }
    double H (coo z,  Spin i) { return H(i,atp(z+dz[0])) + H(i,atp(z+dz[1])) + H(i,atp(z+dz[2])) + H(i,atp(z+dz[3])); }

    void up (coo z) {
        int i = prng.uniform_int(q);
        double dH = H(z,i) - H(z,atp(z));
        if ((dH<=0) || prng.bernoulli(exp(-beta*dH))) put(z,i);
    }

    void up () { up (coo(b+prng.uniform_int(w()-2*b),b+prng.uniform_int(h()-2*b))); }

    void bc (int c) {
        b=1;
        for (int i=0; i<w(); ++i) { put (coo(i,0),c); put (coo(i,h()-1),c); }
        for (int i=0; i<h(); ++i) { put (coo(0,i),c); put (coo(w()-1,i),c); }
    }

    void bc_tripod () {
        b=1;
        for (int i=0; i<w()/2; ++i)   { put (coo(i,0),0); put (coo(i,h()-1),2); }
        for (int i=w()/2; i<w(); ++i) { put (coo(i,0),1); put (coo(i,h()-1),2); }
        for (int i=0; i<h(); ++i)     { put (coo(0,i),0); put (coo(w()-1,i),1); }
    }

    void bc_dobrushin () {
        b=1;
        for (int i=0; i<w(); ++i) for (int j=0; j<h()/2; ++j)   put(coo(i,j),0);
        for (int i=0; i<w(); ++i) for (int j=h()/2; j<h(); ++j) put(coo(i,j),1);
    }

    int q;
    double beta;
    int b;
};

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=500,q=3,b=1");
    Potts P(CLP);
    P.bc_dobrushin();
    P.show();

    while (true) P.up ();
}
