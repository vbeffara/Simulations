#include <vb/Cluster.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
    H.init ("Once-reinforced random walk, Cluster version", argc,argv, "l=150");
    int l = H['l'];
    Image I (729,729); I.show();

    Cluster W; coo z(0,0); W.insert(z); int supsup=0;

    for (unsigned long t=0 ; supsup < l; t++) {
        z = z + dz[prng()%4]; if (!W.at(z)) { W.insert(z); z -= z/10; }
        if (!(t%1000000)) {
			supsup = max (supsup,sup(z));
			W.paint (I);
		}
    }
}
