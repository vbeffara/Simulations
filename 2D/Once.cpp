#include <vb/CL_Parser.h>
#include <vb/CoarseImage.h>
#include <vb/fmt.h>

using namespace vb;

int main(int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=1000,a=2");
    int n = CLP('n');
    double a = CLP('a'); a=1/(1+a);

    int L = (int) pow((double)n,.33);
    CoarseImage img(n,n, str(fmt("A vertex once-reinforced random walk (size=%d, a=%f)") % n % (1/a-1)), L);
    img.show();

    coo z(n/2,n/2);
    while (true) {
        img.put (z,1);
        coo nz = z + dz[prng()%4]; if (!img.contains(nz)) break;
        if (img.at(nz) || prng.bernoulli(a)) z=nz;
    }
}
