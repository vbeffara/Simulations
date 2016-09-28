#include <vb/Image.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	H.init ("Divide-and-color model", argc,argv, "n=300,p=.3,q=.4");
	int n = H['n'];
	double p = H['p'], q = H['q'];

	// Connectivity (&1 -> to the right, &2 -> downwards)
	vector<char> connect (n*n); for (auto & c : connect) c = prng.bernoulli(p) + 2*prng.bernoulli(p);

	H.L->info ("Computing connected components ...");

	vector<long> cluster (n*n); for (int i=0; i<n*n; ++i) cluster[i] = i;

	bool dirty = true;
	while (dirty) {
		dirty = false;
		for (int x=0;x<n;++x) {
			for (int y=0;y<n;++y) {
				if ((x>0) && (connect[(x-1)+n*y] & 1) && (cluster[x+n*y] > cluster[(x-1)+n*y])) {
					dirty = true; cluster[x+n*y] = cluster[(x-1)+n*y];
				}
				if ((x<n-1) && (connect[x+n*y] & 1) && (cluster[x+n*y] > cluster[(x+1)+n*y])) {
					dirty = true; cluster[x+n*y] = cluster[(x+1)+n*y];
				}
				if ((y>0) && (connect[x+n*(y-1)] & 2) && (cluster[x+n*y] > cluster[x+n*(y-1)])) {
					dirty = true; cluster[x+n*y] = cluster[x+n*(y-1)];
				}
				if ((y<n-1) && (connect[x+n*y] & 2) && (cluster[x+n*y] > cluster[x+n*(y+1)])) {
					dirty = true; cluster[x+n*y] = cluster[x+n*(y+1)];
				}
			}
		}
	}

	H.L->info (" ... Done.");

	vector<char> color (n*n); for (auto & c : color) c = 255 * prng.bernoulli(q);

	Image img (n,n);

	for (int x=0; x<n; ++x)
		for (int y=0; y<n; ++y)
			img.put(coo(x,y),color[cluster[x+n*y]]);
	img.show(); img.pause();
}
