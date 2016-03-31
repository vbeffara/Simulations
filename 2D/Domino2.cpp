#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb; using namespace std;

vector<vector<vector<double>>> W;

double weight (coo z, int d) {
    static int l1 = W.size(), l2 = W[0].size();
    if (d>=2) { z += dz[d]; d -= 2; }
    return W [z.x % l1] [z.y % l2] [d];
}

Color col (coo z, int d) {
    static int l1 = W.size(), l2 = W[0].size();
    static double minw = 1, maxw = -1;
    if (maxw == -1) {
        for (int i=0; i<l1; ++i) for (int j=0; j<l2; ++j) for (int k=0; k<2; ++k) {
            minw = std::min(minw,W[i][j][k]); maxw = std::max(maxw,W[i][j][k]);
        }
        if (maxw == minw) maxw = minw + 1;
    }
    if (d>=2) { z += dz[d]; d -= 2; }
    double hh = 0.5 * d + (0.5/l1) * (z.x%l1) + (0.5/l1/l2) * (z.y%l2);
    return HSV (hh,1,1-.7*(weight(z,d)-minw)/(maxw-minw));
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
    H.init ("Domino tiling (v2)", argc,argv, "n=200,a=.5");
    W = vector<vector<vector<double>>> (6, vector<vector<double>> (2, vector<double> (2,1)));
    double a = H['a']; W[0][0][1]=a; W[3][1][1]=a;
    Tiling D(H['n']); D.show(); D.run(); D.pause();
}
