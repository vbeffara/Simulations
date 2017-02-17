#include <vb/Array.h>
#include <vb/Figure.h>

using namespace vb; using namespace std;

struct loc {
	cpx z=0;
};

int main (int argc, char ** argv) {
	H.init ("T-graph for the triangular lattice", argc, argv, "n=20");
	int n=H['n'];

	cpx A(.4,.6), B(0,0), C(1,0);
	double a=abs(B-A), b=abs(C-B), c=abs(A-C);
	cpx alpha=(B-A)/a, beta=(C-B)/b, gamma=(A-C)/c;
	double pa=arg((C-A)/(B-A))/M_PI, pb=arg((A-B)/(C-B))/M_PI, pc=arg((B-C)/(A-C))/M_PI;
	cpx lambda(1,0);

	auto phiwb = [&] (int mw, int nw, int mb, int nb) {
		return real(pow(beta/gamma,-mw)*pow(beta/alpha,-nw)/lambda)
			* alpha*lambda*pow(beta/gamma,mb)*pow(beta/alpha,nb);
	};

	cerr << a * phiwb(0,0,0,0) + b * phiwb(0,0,0,1) + c * phiwb(0,0,-1,1) << endl;

	Array<loc> TG (n,n);

	for (int i=0; i<n; ++i) {
		for (int j=0; j<n; ++j) {
			if (j>0) {
				TG[coo(i,j)].z = TG[coo(i,j-1)].z + phiwb (i+1,j-2,i,j-1);
			} else if (i>0) {
				TG[coo(i,j)].z = TG[coo(i-1,j)].z - phiwb (i,j-1,i,j-1);
			} else {
			}
		}
	}

	Figure F;
	for (int i=0; i<n; ++i) {
		for (int j=0; j<n; ++j) {
			if (i<n-1) F.add (make_unique<Segment> (TG[coo(i,j)].z, TG[coo(i+1,j)].z));
			if (j<n-1) F.add (make_unique<Segment> (TG[coo(i,j)].z, TG[coo(i,j+1)].z));
			if ((i>0)&&(j<n-1)) F.add (make_unique<Segment> (TG[coo(i,j)].z, TG[coo(i-1,j+1)].z));
		}
	}
	F.show(); F.pause();
}
