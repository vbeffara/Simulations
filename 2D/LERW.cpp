#include <vb/CL_Parser.h>
#include <vb/Bitmap.h>
#include <vb/Path.h>
#include <vb/fmt.h>

using namespace vb;

class LERW : public Bitmap<char> {
public:
    LERW (int n) : Bitmap<char> (2*n,2*n,str(fmt("A loop-erased walk of size %d")%n)) {
        coo z(n,n); while (contains(z)) { int d = prng()&3; put(z,d); z += dz[d]; }
    }

    void output (const std::string &s = "") {
        OldPath P (0,title);
        coo z (w()/2,h()/2); while (contains(z)) { int d = at(z); P.push_back(d); z += dz[d]; }
        P.output(s);
    }
};

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=500");
    LERW(CLP('n')).output();
}
