#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb; using namespace std;

vector<vector<vector<double>>> W, hues;

double weight (coo z, int d);

class Domino { public:
    Domino (coo z_ = {0,0}, int d_ = 0) : z(z_), d(d_) { if (d>=2) { z += dz[d]; d -= 2; } }
    Domino (int) {}

    double weight () const { return W [z.x % W.size()] [z.y % W[0].size()] [d]; }

    operator Color () {
        if (!active) return BLACK;
        static int l1 = W.size(), l2 = W[0].size();
        static double minw = 1, maxw = -1;
        if (maxw == -1) {
            for (int i=0; i<l1; ++i) for (int j=0; j<l2; ++j) for (int k=0; k<2; ++k) {
                minw = std::min(minw,W[i][j][k]); maxw = std::max(maxw,W[i][j][k]);
            }
            if (maxw == minw) maxw = minw + 1;
        }
        double hh = hues [z.x%l1] [z.y%l2] [d];
        return HSV (hh,1,1-.5*(::weight(z,d)-minw)/(maxw-minw));
    }

    coo z;
    int d;
    bool active = false;
};

double weight (coo z, int d) { return Domino{z,d}.weight(); }

Color col (coo z, int d) { return Domino{z,d}; }

class Tiling : public Bitmap<Domino> { public:
    Tiling (int n) : Bitmap<Domino> (2*n,2*n) {
        for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) at(coo(x,y)).z = coo(x,y);
        for (int i=0; i<n; ++i) for (int j=n-1-i; j<n+i; j+=2) putd (coo(i,j),1);
        for (int i=n; i<2*n; ++i) for (int j=i-n; j<3*n-i-1; j+=2) putd (coo(i,j),1);
    };

    void putd (coo z, int d) {
        Domino D {z,d}; D.active = true; atp(D.z) = D; atp(D.z + dz[D.d]) = D;
        // atp(z).d = d; atp(z).active = true; atp(z+dz[d]).d = (d+2)%4; atp(z+dz[d]).active = true;
    }

    void flip (coo z) {
        Domino d1 = atp(z); if (!d1.active) return; z = d1.z;
        int d = d1.d; coo zz = z + dz[(d+1)%4];
        Domino d2 = atp(zz); if ((!d2.active) || (d2.z != zz) || (d2.d != d)) return;
        double w1 = weight (z,d) * weight (zz,d);
        double w2 = weight (z,(d+1)%4) * weight (z + dz[d], (d+1)%4);
        if ((w2>=w1) || prng.bernoulli(w2/w1)) { putd (z, (d+1)%4); putd (z + dz[d], (d+1)%4); }
    }
    void run () {
        while (visible()) { flip(rand()); step(); }
    };

    static vector<vector<vector<double>>> W;
};

int main (int argc, char ** argv) {
    H.init ("Domino tiling (v2)", argc,argv, "n=100,a=.5");
    W = vector<vector<vector<double>>> (6, vector<vector<double>> (2, vector<double> (2,1)));
    hues = vector<vector<vector<double>>> (6, vector<vector<double>> (2, vector<double> (2,0)));
    double a = H['a']; W[0][0][1]=a; W[3][1][1]=a;
    Tiling D(H['n']);
    Console C; for (auto & x : hues) for (auto & y : x) for (auto & z : y) { z = prng.uniform_real(); C.manage (z,0.0,1.0); }
    C.show(); D.show(); D.run(); D.pause();
}
