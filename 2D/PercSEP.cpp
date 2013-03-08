
#include "vb/Bitmap.h"
#include "vb/CL_Parser.h"
#include "vb/PRNG.h"

using namespace vb;

class Site {
public:
	Site (int i) : state(i) {};
	operator Color() {
		if (state == 1) return Color(64);
		if (state == 2) return RED;
		return BLACK;
	}
	int state; // 0 closed, 1 empty, 2 occupied
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"n=400,p=.8,l=.3,d=0");
	int n = CLP('n');
	double p = CLP('p'), l = CLP('l'), d = CLP('d');

	Bitmap<Site> P (n,n,"Exclusion in Percolation");
	P.show();

	for (int i=0; i<n; ++i)
		for (int j=0; j<n; ++j)
			if (prng.bernoulli(p))
				P.put(coo(i,j), prng.bernoulli(l) ? 2 : 1);

	coo flow(0);

	for (long t=1 ;; ++t) {
		if ((t%(100*n*n))==0)
			std::cout << t/(100*n*n) << " " << real(flow) << " " << double(real(flow))/t << std::endl;
		coo z (prng.uniform_int(n),prng.uniform_int(n));
		if (P.at(z).state != 2) continue;
		coo s = prng.bernoulli(d) ? 1 : dz[prng.uniform_int(4)];
		coo nz ((real(z+s)+n)%n, (imag(z+s)+n)%n);
		if (P.at(nz).state != 1) continue;
		P.put(z,1); P.put(nz,2);
		flow += s;
	}
}
