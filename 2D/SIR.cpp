#include <vb/Hub.h>
#include <vb/Image.h>

using namespace vb;

const Color none(0), prey(GREEN), pred(RED);

class SIR : public Image { public:
    SIR (int n, double l_) : Image (n,n), l (l_) {
        int n0 = H['d'] ? 0 : int(H['n'])/2;
        for (int i=n0-10; i<n0+10; ++i) for (int j=n0-10; j<n0+10; ++j)
            if (contains(coo(i,j))) { put(coo(i,j),prey); ++ np; }
        put (coo(n0,n0),pred); -- np;
        show();
    }

    void step () {
        coo z = rand(); if (at(z) != prey) return;
        coo nz = z + dz[prng.uniform_int(H['d'] ? 2 : 4)]; if (!contains(nz)) return;
        if ((at(nz) == none) && ((l>1) || prng.bernoulli(l)))   { put(nz,prey); ++ np; }
        if ((at(nz) == pred) && ((l<1) || prng.bernoulli(1/l))) { put(z,pred);  -- np; }
    }

    int np = 0;
    double l;
};

int main (int argc, char ** argv) {
    H.init ("SIR process on the lattice", argc, argv, "n=600,l=.5,d");

    SIR img (H['n'],H['l']);
    while (img.np>0) img.step();
    img.output();
}
