#include "vb/Bitmap.h"
#include "vb/CL_Parser.h"
#include "vb/PRNG.h"
#include "vb/Console.h"

using namespace vb;

const Color C[6] = { RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN };

class Spin {
public:
    int s;
    Spin (int i) : s(i) {};
    operator Color() const { if ((s>=0)&&(s<6)) return C[s]; else return Indexed(s); }
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
        for (int j=0; j<h()-w()/2; ++j) {
            for (int i=0; i<w()/2; ++i)   put(coo(i,j),0);
            for (int i=w()/2; i<w(); ++i) put(coo(i,j),2);
        }
        for (int j=h()-w()/2; j<h(); ++j) {
            for (int i=0; i<h()-j; ++i)         put(coo(i,j),0);
            for (int i=h()-j; i<w()-h()+j; ++i) put(coo(i,j),1);
            for (int i=w()-h()+j; i<w(); ++i)   put(coo(i,j),2);
        }
    }

    void bc_quadripod () {
        b=1;
        for (int j=0; j<h()/2; ++j) {
            for (int i=0; i<w()/2; ++i)   put(coo(i,j),0);
            for (int i=w()/2; i<w(); ++i) put(coo(i,j),1);
        }
        for (int j=h()/2; j<h(); ++j) {
            for (int i=0; i<w()/2; ++i)   put(coo(i,j),2);
            for (int i=w()/2; i<w(); ++i) put(coo(i,j),3);
        }
    }

    void bc_quadripod2 () {
        b=1;
        for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) {
            if (i>j) { if (i+j<w()) put(coo(i,j),0); else put(coo(i,j),1); }
            else { if (i+j<w()) put(coo(i,j),2); else put(coo(i,j),3); }
        }
    }

    void bc_dobrushin (bool r = false) {
        b=1;
        for (int i=0; i<w(); ++i) for (int j=0; j<h()/2; ++j)   put(coo(i,j),0);
        for (int i=0; i<w(); ++i) for (int j=h()/2; j<h(); ++j) put(coo(i,j),1);
        if (r) for (int i=1; i<w()-1; ++i) for (int j=1; j<h()-1; ++j) put(coo(i,j),prng.uniform_int(q));
    }

    void bc_loren () {
        b=1;
        for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j)
            if ((i==0)||(j==0)||(i==w()-1)||(j==h()-1))
                put (coo(i,j),(q*(i+j)/w())%q);
    }

    void bc_loren2 () { assert (q>=6);
        b=1;
        for (int i=0; i<w()/4; ++i)      	put(coo(i,0),0);
        for (int i=w()/4; i<3*w()/4; ++i)	put(coo(i,0),1);
        for (int i=3*w()/4; i<w(); ++i)  	put(coo(i,0),2);
        for (int i=0; i<h()/2; ++i)      	put(coo(0,i),0);
        for (int i=h()/2; i<h(); ++i)    	put(coo(0,i),5);
        for (int i=0; i<h()/2; ++i)      	put(coo(w()-1,i),2);
        for (int i=h()/2; i<h(); ++i)    	put(coo(w()-1,i),3);
        for (int i=0; i<w()/4; ++i)      	put(coo(i,h()-1),5);
        for (int i=w()/4; i<3*w()/4; ++i)	put(coo(i,h()-1),4);
        for (int i=3*w()/4; i<w(); ++i)  	put(coo(i,h()-1),3);
    }

    int q;
    double beta;
    int b;
};

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=500,q=3,b=1");
    Potts P(CLP);
    for (auto & c : P) c=1;
    P.bc_quadripod2();
    P.show();

    Console W;
    W.watch(P.beta,"beta");
    W.manage(P.beta,0.0,4.0);
    W.show();

    while (true) P.up ();
}
