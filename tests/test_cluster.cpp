#include <vb/Cluster.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
    H.init ("Once-reinforced random walk, Cluster version", argc,argv, "a=2");
    double a = H['a']; a=1/(1+a);

    Image I (729,729); I.show();

    Cluster W; coo z(0,0); int supsup=0;

    for (unsigned long t=0 ;; t++) {
        W.insert(z); coo nz = z + dz[prng()%4]; if (W.at(nz) || prng.bernoulli(a)) z=nz;
        if (!(t%1000000)) {
			supsup = max (supsup,sup(z));
			W.paint (I);
		}
    }
}
