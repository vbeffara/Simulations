#include <vb/Hub.h>
#include <vb/Image.h>

using namespace vb;

int main(int argc, char ** argv) {
    H.init ("Brownian motion", argc,argv, "n=500"); int n = H['n'];

    Image img (2*n,2*n);

    for (Color & c : img) c = 255;
    coo z(n,n); while (img.contains(z)) { img.put(z,0); z += dz[prng()%4]; }
    img.output();
}
