#include <vb/Image.h>

using namespace vb;

int main(int argc, char ** argv) {
    H.init ("Discrete-time contact process on Z^2",argc,argv,"p=.4,n=400");

    double p = H['p'];
    int n = H['n'];

    Image img (n,n);

    for (int x=0; x<n; ++x) for (int y=0; y<n/2; ++y) img.put(coo(x,y),WHITE);
    img.show();

    for (int i=0; i<3*n; i++) {
        int total=0;
        for (int x=1; x<n-1; x++) {
            for (int y=1; y<n-1; y++) {
	            if ((x+y+i)%2 == 0) {
                    bool nb = false;
                    nb |= (img.at(coo(x-1,y  )) == WHITE);
                    nb |= (img.at(coo(x+1,y  )) == WHITE);
                    nb |= (img.at(coo(x  ,y-1)) == WHITE);
                    nb |= (img.at(coo(x  ,y+1)) == WHITE);
                    if (nb && prng.bernoulli(p)) {
	                    img.put(coo(x,y),WHITE);
	                    total++;
                    } else {
	                    img.put(coo(x,y),BLACK);
                    }
                }
            }
        }
        if (total==0) break;
    }
}
