#include <vb/Bitmap.h>
#include <vb/Path.h>

using namespace vb;

namespace vb {
    template<> Color to_Color (int t) { return Grey(t); }
}

class LERW : private Bitmap<int> { public:
    explicit LERW (const Hub &H) : Bitmap<int> (2*int(H['n']),2*int(H['n'])) {
        coo z(w()/2,h()/2); while (contains(z)) { int d = prng()&3u; put(z,d); z += dz[d]; }
    }

    using Bitmap<int>::output;
    void output (const std::string &s) override {
        OldPath P (0);
        coo z (w()/2,h()/2); while (contains(z)) { int d = at(z); P.push_back(d); z += dz[d]; }
        P.output(s);
    }
};

int main (int argc, char ** argv) {
    H.init ("Loop-erased random walk",argc,argv,"n=500");
    LERW(H).output();
}
