
#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

Color halfcolors [12] { BLACK,BLACK,BLACK,BLACK , RED,GREEN,YELLOW,BLUE , YELLOW,BLUE,RED,GREEN };

class Half { public:
    unsigned char d, type;
    Half (unsigned char _d = 0) : d(_d), type(0) {}
    void operator= (unsigned char _d) { d=_d; }
    operator Color () { return halfcolors [d+4*type]; }
};

class Tiling : public Bitmap<Half> { public:
    void putd (coo c, unsigned char d) { at(c).d = d; step(); }

    Tiling (Hub &H) : Bitmap<Half> (H['n'],H['n'],H.title) {
        for (int x=0;x<w();++x) for (int y=0;y<h();++y) { at(coo(x,y)).type = 1+((x+y)%2); }
        if (H['o'] == "aztec") {
            for (int i=0; i<h()/2; ++i) {
                for (int j=0; j<w()/2-i-1; ++j) { at(coo(i,j)).type=0; at(coo(w()-1-i,j)).type=0; at(coo(i,h()-1-j)).type=0; at(coo(w()-1-i,h()-1-j)).type=0; }
                for (int j=0; j<w(); ++j) { putd(coo(i,j), 1 + 2*((i+j+h()/2+1)%2)); putd(coo(i+w()/2,j), 1 + 2*((i+j+w()/2)%2)); }
            }
        } else {
            for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) putd (coo(x,y), 2*(x%2));
        }
    }
    
    void flip (coo c) {
        if (at(c).type == 0) return;
        step(); unsigned char d = at(c).d;
        coo oc = c + dz[d] + dz[(d+1)%4]; if (!contains(oc)) return; if (at(oc).d != ((d+2)%4)) return;
        putd (c,(d+1)%4); putd (c+dz[d],(d+1)%4); putd (c+dz[(d+1)%4],(d+3)%4); putd (oc,(d+3)%4); 
    }
};

int main (int argc, char ** argv) {
    Hub H ("Domino tiling",argc,argv,"n=200,o=aztec");
    Tiling T(H); T.show(); T.pause();
    while (true) T.flip(T.rand());
}
