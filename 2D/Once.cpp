#include <vb/CL_Parser.h>
#include <vb/CoarseImage.h>

using namespace vb;

int main(int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=1000,a=2");
    int n = CLP('n');
    double a = CLP('a'); a=1/(1+a);
    int L = pow((double)n,.33);

    CoarseImage img(n,n,L); img.show();

    for (coo z(n/2,n/2);;) {
        img.put (z,1);
        coo nz = z + dz[prng()%4]; if (!img.contains(nz)) break;
        if (img.at(nz) || prng.bernoulli(a)) z=nz;
    }
}
