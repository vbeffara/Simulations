#include <vb/Array.h>
#include <vb/Console.h>
#include <vb/Figure.h>

using namespace vb; using namespace std;

struct loc { cpx z=0; };

class TGraph : public Array<loc> {
public:
	TGraph (int n, cpx AA, cpx l) : Array<loc> (n,n), A(AA), lambda(l) {
		compute(); plot(); F.show(); F.output();
	};

	cpx phiwb (int mw, int nw, int mb, int nb) {
		return real(pow(beta/gamma,-mw)*pow(beta/alpha,-nw)/lambda)
			* alpha*lambda*pow(beta/gamma,mb)*pow(beta/alpha,nb);
	}

	void compute () {
		a=abs(B-A); b=abs(C-B); c=abs(A-C);
		alpha=(B-A)/a; beta=(C-B)/b; gamma=(A-C)/c;
		pa=arg((C-A)/(B-A))/M_PI, pb=arg((A-B)/(C-B))/M_PI, pc=arg((B-C)/(A-C))/M_PI;

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

	void loop () { for (int i=0 ;; ++i) { lambda = exp(2.0*M_PI*I*cpx(i,0)/100.0); compute(); plot(); F.update(); } }

	void interact () {
		Console C;
		double theta=arg(lambda)/(2.0*M_PI);
		C.manage<double> (theta,0,1,"arg(λ) / 2π",[this,&theta]{ lambda = exp(2.0*M_PI*I*theta); compute(); plot(); F.update(); });
		C.watch(lambda,"λ");
		C.manage<cpx> (A,{-1,0},{2,2},"point A",[this]{ compute(); plot(); F.update(); });
		C.show(); C.pause();
	}

	cpx A,B=0,C=1, alpha=1,beta=1,gamma=1, lambda;
	double a=1,b=1,c=1, pa=1,pb=1,pc=1;
	Figure F;
};

int main (int argc, char ** argv) {
	H.init ("T-graph for the triangular lattice", argc, argv, "n=20,l");
	int n=H['n'];
	TGraph TG (n, cpx{.4,.6}, cpx{1,0});
	if (H['l']) TG.loop(); else TG.interact();
}
