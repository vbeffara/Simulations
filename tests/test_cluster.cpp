#include <vb/Cluster.h>

using namespace vb; using namespace std;

class Cluster2 : public Cluster { public:
	int dist_to_box (coo z) { return max (0L, min ({z.x-(ul.x+w-1), z.y-(ul.y+w-1), ul.x-z.x, ul.y-z.y})); }
	int dist_to_out (coo z) { return max (0L, min ({z.x-ul.x+1, z.y-ul.y+1, (ul.x+w)-z.x, (ul.y+w)-z.y})); }

	int inrad_go (coo z) {
		if (np == w*w) return dist_to_out (z);
		if (w == bs) return 1;
		int ww=w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
		return ((Cluster2*)(&sub[i+3*j])) -> inrad_go(z);
	}
	int inrad (coo z) { if ((dist_to_box(z) > 0) || (!at(z))) return 0; else return inrad_go (z); }
};

int main (int argc, char ** argv) {
    H.init ("Once-reinforced random walk, Cluster version", argc,argv, "l=200,a=10,v,s=0");
    int l = H['l'];
    double a = H['a']; a = 1 / (1+a);
    int s = H['s']; if (s) prng.seed(s);
    Image I (729,729); I.show();
    if (H['v']) I.snapshot_setup("ORRW_cluster",60);

    Cluster2 W; coo z(0,0); W.insert(z); long supsup=0;

    for (unsigned long t=0 ; supsup < l; t++) {
		int ir = W.inrad_go(z), d = 1 + ir/2;
        coo nz = z + dz[prng.uniform_int(4)] * d;
        bool there = (d>1) ? true : W.at(nz);
        if (there || prng.bernoulli(a)) {
			z = nz; if (!there) W.insert(z);
			supsup = max (supsup,sup(z));
		}
        if (!(t%1000000)) {
			cerr << "\r" << t << " particles.";
			W.paint (I);
		}
    }
}
