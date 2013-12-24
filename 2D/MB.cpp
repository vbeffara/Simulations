#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/fmt.h>

using namespace vb;

int main(int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=500"); int n = CLP('n');

    Image img (2*n,2*n, str(fmt("A Brownian path of size %d")%n));

    for (Color & c : img) c = 255;
    coo z(n,n); while (img.contains(z)) { img.put(z,0); z += dz[prng()%4]; }
    img.output();
}
