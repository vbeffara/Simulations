#include <vb/Bitmap.h>
#include <vb/Hub.h>
#include <vb/Path.h>

using namespace vb;

class LERW : private Bitmap<char> {
public:
    LERW (Hub &H) : Bitmap<char> (2*int(H['n']),2*int(H['n']),H.title) {
        coo z(w()/2,h()/2); while (contains(z)) { int d = prng()&3; put(z,d); z += dz[d]; }
    }

    void output (const std::string &s = "") {
        OldPath P (0,title);
        coo z (w()/2,h()/2); while (contains(z)) { int d = at(z); P.push_back(d); z += dz[d]; }
        P.output(s);
    }
};

int main (int argc, char ** argv) {
    Hub H ("Loop-erased random walk",argc,argv,"n=500");
    LERW(H).output();
}
