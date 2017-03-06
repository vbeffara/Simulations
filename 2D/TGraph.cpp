#include <vb/Array.h>
#include <vb/Console.h>
#include <vb/Figure.h>

using namespace vb; using namespace std;

struct loc { cpx z=0; };

class TGraph : public Figure, Array<loc> {
public:
	TGraph (int n, cpx A, double t) : Array<loc> (n,n), A(A), theta(t) { compute(); plot(); show(); output(); };

	cpx phiwb (int mw, int nw, int mb, int nb) {
		return real(pow(beta/gamma,-mw)*pow(beta/alpha,-nw)/lambda)
			* alpha*lambda*pow(beta/gamma,mb)*pow(beta/alpha,nb);
	}

	void compute () {
		a=abs(B-A); b=abs(C-B); c=abs(A-C);
		alpha=(B-A)/a; beta=(C-B)/b; gamma=(A-C)/c;
		pa=arg((C-A)/(B-A))/M_PI; pb=arg((A-B)/(C-B))/M_PI; pc=arg((B-C)/(A-C))/M_PI;
		lambda=exp(2.0*M_PI*I*theta);

		for (int i=0; i<ww; ++i) {
			for (int j=0; j<hh; ++j) {
				if (j>0) at(coo(i,j)).z = at(coo(i,j-1)).z + c * phiwb (i+1,j-2,i,j-1);
				else if (i>0) at(coo(i,j)).z = at(coo(i-1,j)).z - a * phiwb (i,j-1,i,j-1);
			}
		}
	}

	void plot () {
		contents.clear();
		for (int i=0; i<ww; ++i) {
			for (int j=0; j<hh; ++j) {
				if (i<ww-1) add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i+1,j)).z));
				if (j<hh-1) add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i,j+1)).z));
				if ((i>0)&&(j<hh-1)) add (make_unique<Segment> (at(coo(i,j)).z, at(coo(i-1,j+1)).z));
			}
		}
	}

	void loop () { while (true) { theta = Duration(now()-start).count()/3; compute(); plot(); update(); } }

	void interact () {
		Console C;
		C.manage (theta,0.0,1.0,"arg(λ) / 2π",[this]{ compute(); plot(); update(); });
		C.watch (lambda,"λ");
		C.manage (A,{-1,0},{2,2},"point A",[this]{ compute(); plot(); update(); });
		C.show(); C.pause();
	}

	cpx A,B=0,C=1, alpha=1,beta=1,gamma=1, lambda;
	double theta, a=1,b=1,c=1, pa=1,pb=1,pc=1;
};

int main (int argc, char ** argv) {
	H.init ("T-graph for the triangular lattice", argc, argv, "n=20,l,x=.4,y=.6,t");
	TGraph TG (H['n'], cpx{H['x'],H['y']}, H['t']);
	if (H['l']) TG.loop(); else TG.interact();
}
