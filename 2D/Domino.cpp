
#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

Color halfcolors [12] { BLACK,BLACK,BLACK,BLACK , RED,GREEN,YELLOW,BLUE , YELLOW,BLUE,RED,GREEN };

class Half { public:
    unsigned char d, type;
    Half (unsigned char _d = 0, unsigned char _t = 0) : d(_d), type(_t) {}
    operator Color () { return halfcolors [d+4*type]; }
};

class Tiling : public Bitmap<Half> { public:
    void putd (coo c, unsigned char d) { at(c).d = d; at(c+dz[d]).d = (d+2)%4; step(); }
    void freeze (coo c) { at(c).type = 0; at(c+dz[at(c).d]).type = 0; }

    Tiling (Hub &H) : Bitmap<Half> (H['n'],H['n'],H.title) {
        for (int x=0;x<w();++x) for (int y=0;y<h();++y) { at(coo(x,y)) = Half ( 2*(x%2), 1+((x+y)%2) ); }
        if (H['o'] == "aztec") {
            for (int i=0; i<h()/2; ++i) {
                for (int j=0; j<w()/2-i-1; ++j) { at(coo(i,j)).type=0; at(coo(w()-1-i,j)).type=0; at(coo(i,h()-1-j)).type=0; at(coo(w()-1-i,h()-1-j)).type=0; }
                for (int j=0; j<w(); ++j) { putd(coo(i,j), 1 + 2*((i+j+h()/2+1)%2)); putd(coo(i+w()/2,j), 1 + 2*((i+j+w()/2)%2)); }
            }
        } else if (H['o'] == "hill") {
            for (int y=0; y<h()/2; ++y) for (int x=y; x<w()-y; x+=2)     { putd(coo(x,y),0); putd(coo(x,h()-1-y),0); }
            for (int x=0; x<w()/2; ++x) for (int y=x+1; y<h()-1-x; y+=2) { putd(coo(x,y),1); putd(coo(w()-1-x,y),1); }
        } else {
            for (int x=0; x<w(); x+=2) for (int y=0; y<h(); ++y) putd (coo(x,y), 0);
            int b = H['b']; if (b>0) {
                for (int x=w()/2-b; x<w()/2+b; ++x) putd(coo(x,h()/2), (2+at(coo(x,h()/2)).d)%4);
                at(coo(w()/2-b,h()/2)).type = 0; at(coo(w()/2+b,h()/2)).type = 0;
            }
        }
    }
    
    void flip (coo c) {
        if (at(c).type == 0) return;
        unsigned char d = at(c).d; coo oc = c + dz[d] + dz[(d+1)%4];
        if (!contains(oc)) return; if (at(oc).type == 0) return; if (at(oc).d != ((d+2)%4)) return;
        putd (c,(d+1)%4); putd (oc,(d+3)%4);
    }
};

int main (int argc, char ** argv) {
    Hub H ("Domino tiling",argc,argv,"n=200,o=aztec|hill|flat,b=0,f=0");
    Tiling T(H); T.show(); T.pause();
    unsigned f = T.w()*T.h()*double(H['f']);
    for (unsigned t=1 ;; ++t) { T.flip(T.rand()); if (t==f) T.freeze (coo(T.w()/2,T.h()/2)); }
}
