#include <vb/Cluster.h>

using namespace vb; using namespace std;

class Cluster2 : public Cluster { public:
	int dist_to_box (coo z) { return max ({z.x-ul.x-w, z.y-ul.y-w, ul.x-z.x, ul.y-z.y, 0}); }
	int dist_to_out (coo z) { return min ({z.x-ul.x, z.y-ul.y, ul.x+w-z.x, ul.y+w-z.y}); }

	int inrad_go (coo z) {
		assert (fits(z));
		if (np == w*w) return dist_to_out (z);
		if (w == bs) return 1;
		int ww=w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
		return ((Cluster2*)(&sub[i+3*j])) -> inrad_go(z);
	}
	int inrad (coo z) { if ((dist_to_box(z) > 0) || (!at(z))) return 0; else return inrad_go (z); }
};

int main (int argc, char ** argv) {
    H.init ("Internal DLA, Cluster version", argc,argv, "l=150");
    int l = H['l'];
    Image I (729,729); I.show();

    Cluster2 W; coo z(0,0); W.insert(z); int supsup=0;

    for (unsigned long t=0 ; supsup < l; t++) {
		int ir = W.inrad_go(z), d = 1 + ir/2;
        z = z + dz[prng()%4] * d;
        if (!W.at(z)) { W.insert(z); z -= z/10; }
        if (!(t%1000000)) {
			supsup = max (supsup,sup(z));
			W.paint (I);
			// cerr << z << "\t" << W.inrad(z) << endl;
		}
    }
}
