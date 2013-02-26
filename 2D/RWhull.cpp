#include <vb/CL_Parser.h>
#include <vb/Bitmap.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

vector<int> bridge (int n, bool p=false) {
    vector<int> v (n,0);
    for (int i=0; i<n/2; ++i) v[i]=1;
    for (int i=n-1; i>=0; --i) {
        int j = prng.uniform_int(i+1);
        if (i!=j) swap(v[i],v[j]);
    }

    if (!p) return v;

    int s=0,ms=0,mi=0;
    for (int i=0;i<n;++i) {
        s += 2*v[i]-1;
        if (s<ms) { ms=s; mi=i; }
    }

    vector<int> w(n,0);
    for (int i=mi+1; i<n; ++i) w[i-mi-1] = v[i];
    for (int i=0; i<=mi; ++i) w[n-mi-1+i] = v[i];

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

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=50,i,v");
    int n=CLP('n');
    int l=n*n*n*n;
    bool inf=CLP('i');
    bool vid=CLP('v');

    Bitmap<Stat> img(6*n,6*n,"Random Walk Snake hull");

    vector<coo> p;
    p.push_back(coo(3*n,3*n));
    img.at(p.back()) = 1;
    img.show();

    if (inf) {
        if (vid) img.snapshot_setup ("RWSH",1.0);
        while (abs(p.back() - coo(3*n,3*n)) < 3*n-1) {
            if ((p.size()==1) || (prng.bernoulli(.5))) {
                p.push_back (p.back() + dz[prng.uniform_int(4)]);
                img.at(p.back()) += 1; img.step();
            } else {
                p.pop_back();
            }
        }
    } else {
        vector<int> b = bridge (l,true);
        for (int i=0; i<l; ++i) {
            if (b[i]==1) {
                p.push_back (p.back() + dz[prng.uniform_int(4)]);
                img.at(p.back()) += 1;
            } else {
                p.pop_back();
            }
        }
    }

    img.pause();
}
