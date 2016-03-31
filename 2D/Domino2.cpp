#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb; using namespace std;

vector<vector<Color>> C {{ BLUE, GREEN, YELLOW, RED }, { YELLOW, RED, BLUE, GREEN }};

class Half { public:
    Half (int _d, coo _z = {0,0}) : d(_d), z(_z) {}
    operator Color () {
        if (!active) return BLACK;
        int parity = (z.x+z.y)%2;
        return C[parity][d];
    }
    int d;
    coo z;
    bool active = false;
};

class Tiling : public Bitmap<Half> { public:
    Tiling (int n) : Bitmap<Half> (2*n,2*n) {
        for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) at(coo(x,y)).z = coo(x,y);
        for (int i=0; i<n; ++i) for (int j=n-1-i; j<n+i; j+=2) putd (coo(i,j),1);
        for (int i=n; i<2*n; ++i) for (int j=i-n; j<3*n-i-1; j+=2) putd (coo(i,j),1);
    };
    void putd (coo z, int d) { atp(z).d = d; atp(z).active = true; atp(z+dz[d]).d = (d+2)%4; atp(z+dz[d]).active = true; }
    void flip (coo z) {
        if (!at(z).active) return;
        int d = atp(z).d; coo zz = z + dz[d] + dz[(d+1)%4];
        if ((!at(zz).active) || (at(zz).d != (d+2)%4)) return;
        putd (z, (d+1)%4); putd (zz, (d+3)%4);
    }
    void run () {
        while (visible()) { flip(rand()); step(); }
    };
};

int main (int argc, char ** argv) {
    H.init ("Domino tiling (v2)", argc,argv, "n=100,o=aztec");
    Tiling D(H['n']); D.show(); D.run(); D.pause();
}
