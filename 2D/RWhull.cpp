#include <vb/CL_Parser.h>
#include <vb/Bitmap.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

vector<int> bridge (int n, bool p=false) {
    vector<int> v (n,-1);
    for (int i=0; i<n/2; ++i) v[i]=1;
    for (int i=n-1; i>=0; --i) swap (v[i], v[prng.uniform_int(i+1)]);

    if (!p) return v;

    int s=0, ms=0, mi=0;
    for (int i=0; i<n; ++i) {
        s += v[i];
        if (s<ms) { ms=s; mi=i; }
    }

    vector<int> w;
    for (int i=mi+1; i<n; ++i) w.push_back (v[i]);
    for (int i=0; i<=mi; ++i)  w.push_back (v[i]);
    return w;
}

int m=1;

class Stat {
public:
    Stat (int ss) : s(ss) {}
    int operator+= (int k) {
        s += k;
        m = max(m,s);
        return s;
    }
    operator Color() {
        if (s==0) return 255;
        if (s==1) return Color(255,0,0);
        return 255 - (50 + (s*205)/m);
    }
    int s;
};

class Snake : public Bitmap<Stat> {
public:
    Snake (int n_) : Bitmap<Stat> (6*n_,6*n_,"The path of a snake"), n(n_) {
        p.push_back(coo(3*n,3*n));
        put (p.back(),1); show();
    };
    void grow (int d) {
        p.push_back (p.back() + dz[d]);
        at(p.back()) += 1; step();
    }
    void shrink() { p.pop_back(); }

    int n;
    vector<coo> p;
};

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=50,i,v");
    int n=CLP('n'), l=n*n*n*n;
    bool inf=CLP('i'), vid=CLP('v');

    Snake S(n);

    if (inf) {
        if (vid) S.snapshot_setup ("RWSH",1.0);
        while (abs(S.p.back() - coo(3*n,3*n)) < 3*n-1) {
            if ((S.p.size()==1) || (prng.bernoulli(.5))) {
                S.grow(prng.uniform_int(4));
            } else S.shrink();
        }
    } else {
        vector<int> b = bridge (l,true);
        for (int i=0; i<l; ++i) {
            if (b[i]==1) {
                S.grow(prng.uniform_int(4));
            } else S.shrink();
        }
    }

    S.pause();
}
