
#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

Color V1(100,255,100), V2(0,0,100), V3(0,100,0), V4(150,150,255), H1(255,100,100), H2(100,100,0), H3(255,255,0), H4(100,0,0);
vector<Color> dicolors   { BLACK,BLACK,BLACK,BLACK , H1,V1,H2,V2           , H3,V2,H4,V1           , H4,V3,H3,V4           , H2,V4,H1,V3 };
vector<Color> halfcolors { BLACK,BLACK,BLACK,BLACK , RED,GREEN,YELLOW,BLUE , YELLOW,BLUE,RED,GREEN , RED,GREEN,YELLOW,BLUE , YELLOW,BLUE,RED,GREEN };

class Half { public:
    unsigned char d, type;
    Half (unsigned char _d = 0, unsigned char _t = 0) : d(_d), type(_t) {}
    operator Color () { return halfcolors [d+4*type]; }
};

class Tiling : public Bitmap<Half> { public:
    void putd (coo c, unsigned char d) { at(c).d = d; at(c+dz[d]).d = (d+2)%4; step(); }
    void freeze (coo c) { at(c).type = 0; at(c+dz[at(c).d]).type = 0; }

    Tiling (Hub &H) : Bitmap<Half> (H['n'],H['n'],H.title), r(H['r']), rr {r,r*r,1,r} {
        for (int x=0;x<w();++x) for (int y=0;y<h();++y) { at(coo(x,y)) = Half ( 2*(x%2), 1 + ((x+y)%2) + 2*(x%2) ); }
        if (H['o'] == "aztec") {
            for (int i=0; i<h()/2; ++i) {
                for (int j=0; j<w()/2-i-1; ++j) { at(coo(i,j)).type=0; at(coo(w()-1-i,j)).type=0; at(coo(i,h()-1-j)).type=0; at(coo(w()-1-i,h()-1-j)).type=0; }
                for (int j=0; j<w(); ++j) { putd(coo(i,j), 1 + 2*((i+j+h()/2+1)%2)); putd(coo(i+w()/2,j), 1 + 2*((i+j+w()/2)%2)); }
            }
        } else if (H['o'] == "hill") {
            for (int y=0; y<h()/2; ++y) for (int x=y; x<w()-y; x+=2)     { putd(coo(x,y),0); putd(coo(x,h()-1-y),0); }
            for (int x=0; x<w()/2; ++x) for (int y=x+1; y<h()-1-x; y+=2) { putd(coo(x,y),1); putd(coo(w()-1-x,y),1); }
        } else if (H['o'] == "hole") {
            for (int y=0; y<h()/2; ++y) for (int x=y; x<w()-y-1; x+=2)   { putd(coo(x,y),0); putd(coo(x+1,h()-1-y),0); }
            for (int x=0; x<w()/2; ++x) for (int y=x+1; y<h()-1-x; y+=2) { putd(coo(x,y),1); putd(coo(w()-1-x,y-1),1); }
            coo mid (w()/2,h()/2); at(mid).type = 0; putd (mid+coo(-1,-1),1); putd (mid+coo(-1,1),0); putd (mid+coo(1,1),3); putd (mid+coo(1,-1),2);
            at(mid).type = 0; putd (mid+coo(-2,-2),1); putd (mid+coo(-2,0),1); putd (mid+coo(-2,2),0); putd (mid+coo(0,2),0);
            at(mid).type = 0; putd (mid+coo(2,2),3); putd (mid+coo(2,0),3); putd (mid+coo(2,-2),2); putd (mid+coo(0,-2),2);
        } else {
            for (int x=0; x<w(); x+=2) for (int y=0; y<h(); ++y) putd (coo(x,y), 0);
            int b = H['b']; if (b>0) {
                for (int x=w()/2-b; x<w()/2+b; ++x) putd(coo(x,h()/2), (2+at(coo(x,h()/2)).d)%4);
                at(coo(w()/2-b,h()/2)).type = 0; at(coo(w()/2+b,h()/2)).type = 0;
            }
        }
    }

    int flip (coo c) {
        if (at(c).type == 0) return 0;
        unsigned char d = at(c).d; coo oc = c + dz[d] + dz[(d+1)%4];
        if (!contains(oc)) return 0; if (at(oc).type == 0) return 0; if (at(oc).d != ((d+2)%4)) return 0;
        vector<double> rr {r,r*r,1,r};
        if (prng.bernoulli(1-rr[(d+at(c).type)%4])) return 0;
        putd (c,(d+1)%4); putd (oc,(d+3)%4); return 1;
    }

    double r; vector<double> rr;
};

int main (int argc, char ** argv) {
    H.init ("Domino tiling",argc,argv,"n=200,o=aztec|hill|hole|flat,b=0,f=0,r=1,d"); Tiling T(H);
    if (H['d']) halfcolors = dicolors;
    Console C; C.watch(T.r,"r"); C.manage(T.r,0.0,1.0); C.show();
    T.show(); T.pause();
    unsigned f = T.w()*T.h()*double(H['f']);
    for (unsigned long long t=1 ;; ++t) { T.flip(T.rand()); if (t==f) T.freeze (coo(T.w()/2,T.h()/2)); }
}
