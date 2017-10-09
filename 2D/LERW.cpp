#include <vb/Bitmap.h>
#include <vb/Path.h>

using namespace vb;

class Site { public:
    Site (char s = 0) : s(s) {}
    operator int() { return s; }
    operator Color() { return Grey(s); }
    int s;
};

class LERW : private Bitmap<Site> { public:
    LERW (Hub &H) : Bitmap<Site> (2*int(H['n']),2*int(H['n'])) {
        coo z(w()/2,h()/2); while (contains(z)) { int d = prng()&3; put(z,d); z += dz[d]; }
    }

    void output (const std::string &s = "") override {
        OldPath P (0);
        coo z (w()/2,h()/2); while (contains(z)) { int d = at(z); P.push_back(d); z += dz[d]; }
        P.output(s);
    }
};

int main (int argc, char ** argv) {
    H.init ("Loop-erased random walk",argc,argv,"n=500");
    LERW(H).output();
}
