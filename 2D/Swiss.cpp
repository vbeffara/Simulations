#include <vb/Bitmap.h>

#define EAST  0
#define NORTH 1
#define WEST  2
#define SOUTH 3

#define C_EAST  vb::Color(0,255,0)
#define C_NORTH vb::Color(255,0,0)
#define C_WEST  vb::Color(0,0,255)
#define C_SOUTH vb::Color(255,255,0)

namespace vb {
    template<> Color to_Color (uint8_t t) {
        static const std::vector<Color> colors { C_EAST, C_NORTH, C_WEST, C_SOUTH };
        return colors[t];
    }
}

class World : public vb::Bitmap<uint8_t> {
public:
    int c;
    double p,q;

    explicit World (const vb::Hub &H) : Bitmap<uint8_t> (H['n'],H['n']), c(H['c']), p(H['p']), q(H['q']) {
        int mid = (w()+h())/2;
        for (int x=0; x<w(); ++x) {
            for (int y=0; y<h(); ++y) {
                if (y>x) { if (x+y<mid) at(vb::coo(x,y)) = EAST;  else at(vb::coo(x,y)) = SOUTH; }
                else     { if (x+y<mid) at(vb::coo(x,y)) = NORTH; else at(vb::coo(x,y)) = WEST;  }
                if (vb::prng.bernoulli(p)) at(vb::coo(x,y)) = vb::prng.uniform_int(4);
            }
        }

        for (int x=c; x<w()-c; ++x) for (int y=c; y<h()-c; ++y) at(vb::coo(x,y)) = vb::prng.uniform_int(4);
    }

    void run () {
        for (vb::coo z (w()/2,h()/2);;) {
            std::vector<int> nb (4,0);
            nb[atp(z+vb::coo{1,0})] += 1; nb[atp(z-vb::coo{1,0})] += 1; nb[atp(z+vb::coo{0,1})] += 1; nb[atp(z-vb::coo{0,1})] += 1;
            int max = 0; for (int i : nb) if (i>max) max=i;

            int d = vb::prng.uniform_int(4);
            if (vb::prng.bernoulli(1-q))
                while (nb[d]<max) d = vb::prng.uniform_int(4);

            putp(z,d); z += vb::dz[d];
        }
    }
};

int main (int argc, char ** argv) {
    vb::H.init ("The Swiss Journalist",argc,argv,"n=600,c=0,p=.8,q=.35");
    World w(vb::H); w.show(); w.run();
}
