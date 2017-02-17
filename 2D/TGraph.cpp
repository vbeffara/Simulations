#include <vb/Array.h>
#include <vb/Figure.h>

using namespace vb; using namespace std;

struct loc { cpx z=0; };

class TGraph : public Array<loc> {
public:
	TGraph (int n, cpx AA, cpx l) : Array<loc> (n,n), A(AA), lambda(l) {
		a=abs(B-A); b=abs(C-B); c=abs(A-C);
		alpha=(B-A)/a; beta=(C-B)/b; gamma=(A-C)/c;
		pa=arg((C-A)/(B-A))/M_PI, pb=arg((A-B)/(C-B))/M_PI, pc=arg((B-C)/(A-C))/M_PI;

		compute(); plot(); F.show(); F.output();
	};

	cpx phiwb (int mw, int nw, int mb, int nb) {
		return real(pow(beta/gamma,-mw)*pow(beta/alpha,-nw)/lambda)
			* alpha*lambda*pow(beta/gamma,mb)*pow(beta/alpha,nb);
	}

	void compute () {
		for (int i=0; i<ww; ++i) {
			for (int j=0; j<hh; ++j) {
				if (j>0) {
					at(coo(i,j)).z = at(coo(i,j-1)).z + c * phiwb (i+1,j-2,i,j-1);
				} else if (i>0) {
					at(coo(i,j)).z = at(coo(i-1,j)).z - a * phiwb (i,j-1,i,j-1);
				}
			}
		}
	}

	void plot () {
		F.contents.clear();
		for (int i=0; i<ww; ++i) {
			for (int j=0; j<hh; ++j) {
				if (i<ww-1) F.add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i+1,j)).z));
				if (j<hh-1) F.add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i,j+1)).z));
				if ((i>0)&&(j<hh-1)) F.add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i-1,j+1)).z));
			}
		}
	}

	cpx A,B=0,C=1, alpha,beta,gamma, lambda;
	double a,b,c, pa,pb,pc;
	Figure F;
};

int main (int argc, char ** argv) {
	H.init ("T-graph for the triangular lattice", argc, argv, "n=20,l");
	int n=H['n'];
	TGraph TG (n, cpx{.4,.6}, cpx{1,0});
	if (H['l']) {
		for (int i=0 ;; ++i) {
			TG.lambda = exp(2.0*M_PI*I*cpx(i,0)/100.0); TG.compute(); TG.plot(); TG.F.update();
			if (i==0) TG.F.output();
		}
	} else TG.F.pause();
}
