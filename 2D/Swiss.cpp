#include <vb/Hub.h>
#include <vb/Bitmap.h>

using namespace vb;
using namespace std;

#define EAST  0
#define NORTH 1
#define WEST  2
#define SOUTH 3

#define C_EAST  Color(0,255,0)
#define C_NORTH Color(255,0,0)
#define C_WEST  Color(0,0,255)
#define C_SOUTH Color(255,255,0)

Color colors[4] = { C_EAST, C_NORTH, C_WEST, C_SOUTH };

class Site {
public:
    unsigned char s;
    Site (unsigned char _s) : s(_s) { };
    void operator= (unsigned char _s) { s=_s; }
    operator int () { return s; }
    operator Color () { return colors[s]; }
};

class World : public Bitmap<Site> {
public:
    int c;
    double p,q;

    World (Hub &H) : Bitmap<Site> (H('n'),H('n'),H.title), c(H('c')), p(H('p')), q(H('q')) {
        int mid = (w()+h())/2;
        for (int x=0; x<w(); ++x) {
            for (int y=0; y<h(); ++y) {
                if (y>x) { if (x+y<mid) at(coo(x,y)) = EAST;  else at(coo(x,y)) = SOUTH; }
                else     { if (x+y<mid) at(coo(x,y)) = NORTH; else at(coo(x,y)) = WEST;  }
                if (prng.bernoulli(p)) at(coo(x,y)) = prng.uniform_int(4);
            }
        }

        for (int x=c; x<w()-c; ++x) for (int y=c; y<h()-c; ++y) at(coo(x,y)) = prng.uniform_int(4);
    }

    void run () {
        for (coo z (w()/2,h()/2);;) {
            int nb[4] { 0,0,0,0 };
            nb[atp(z+1)] += 1; nb[atp(z-1)] += 1; nb[atp(z+J)] += 1; nb[atp(z-J)] += 1;
            int max = 0; for (int i : nb) if (i>max) max=i;

            int d = prng.uniform_int(4);
            if (prng.bernoulli(1-q))
                while (nb[d]<max) d = prng.uniform_int(4);

            putp(z,d); z += dz[d];
        }
    }
};

int main (int argc, char ** argv) {
    Hub H ("The Swiss Journalist",argc,argv,"n=600,c=0,p=.8,q=.35");
    World w(H); w.show(); w.pause(); w.run();
}
