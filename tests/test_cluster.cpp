#include <vb/Cluster.h>
#include <vb/Image.h>

using namespace vb; using namespace std;

void paint (Cluster & C, Image & I, coo ul, coo br) {
	if (C.np == 0) { for (int x=ul.x; x<br.x; ++x) for (int y=ul.y; y<br.y; ++y) I.put(coo(x,y),BLACK); }
	else if (C.np == C.w*C.w) {
		for (int x=ul.x; x<br.x; ++x) for (int y=ul.y; y<br.y; ++y)
			I.put (coo(x,y), (x==ul.x)||(x==br.x-1)||(y==ul.y)||(y==br.y-1) ? RED : BLUE); }
	else if (br == ul + coo(1,1)) { I.put (ul, (255 * C.np) / (C.w*C.w)); }
	else if (C.sub.size()) {
		int ww = br.x-ul.x, hh = br.y-ul.y;
		for (int x=0; x<3; ++x) for (int y=0; y<3; ++y)
			paint (C.sub[x+3*y],I,ul+coo(x*ww/3,y*hh/3),ul+coo((x+1)*ww/3,(y+1)*hh/3));
	} else {
		assert (C.tile.size() == C.w*C.w);
		int ww = br.x-ul.x, hh = br.y-ul.y;
		for (int x=0; x<C.w; ++x) for (int y=0; y<C.w; ++y)
			I.put (ul+coo(x*ww/C.w,y*hh/C.w), C.tile[x+C.w*y] ? WHITE : BLACK);
	}
}

int main (int argc, char ** argv) {
    H.init ("Once-reinforced random walk, Cluster version", argc,argv, "a=2");
    double a = H['a']; a=1/(1+a);

    Image I (729,729); I.show();

    Cluster::bs = 128; Cluster W; coo z(0,0); int supsup=0;

    for (unsigned long t=0 ;; t++) {
        W.insert(z); coo nz = z + dz[prng()%4]; if (W.at(nz) || prng.bernoulli(a)) z=nz;
        if (!(t%1000000)) {
			supsup = max (supsup,sup(z));
			// cout << t << " " << supsup << " " << double(W.np)/(supsup*supsup) << endl;
			paint (W,I,coo(0,0),coo(729,729));
		}
    }
}
