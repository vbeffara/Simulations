#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb; using namespace std;

double a = .5;
vector<vector<vector<double>>> W {{{ a, a }, { a, 1 }}, {{ 1, a }, { 1, 1 }}};
vector<vector<double>> hue {{0,.25},{.5,.75}};

double weight (coo z, int d) {
    if (d>=2) { z += dz[d]; d -= 2; }
    return W[z.x%2][z.y%2][d];
}

Color col (coo z, int d) {
    if (d>=2) { z += dz[d]; d -= 2; }
    return HSV (hue[z.x%2][z.y%2],1,1-.8*weight(z,d));
}

class Half { public:
    Half (int _d, coo _z = {0,0}) : d(_d), z(_z) {}
    operator Color () { return active ? col(z,d) : BLACK; }
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
        double w1 = weight (z,d) * weight (zz,(d+2)%4);
        double w2 = weight (z,(d+1)%4) * weight (zz, (d+3)%4);
        if ((w2>=w1) || prng.bernoulli(w2/w1)) { putd (z, (d+1)%4); putd (zz, (d+3)%4); }
    }
    void run () {
        while (visible()) { flip(rand()); step(); }
    };
};

int main (int argc, char ** argv) {
    H.init ("Domino tiling (v2)", argc,argv, "n=200,o=aztec");
    Tiling D(H['n']); D.show(); D.run(); D.pause();
}
