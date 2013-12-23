#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <vb/Figure.h>
#include <vb/Console.h>
using namespace std; using namespace vb;

double a0 = 10, aex1 = 20, aex2 = 20, sigmab0 = .5, sigmabex1 = .1, sigmabex2 = .1, Ac = .3, sigmac = .1, slope = 0;

double g        	(double x, double sigma)	{ return exp(-x*x/(2*sigma*sigma)); }
double b        	(double x)              	{ return a0 * g(x,sigmab0) + aex1 * g(x-2,sigmabex1) + aex2 * g(x+2,sigmabex2) + slope*x; }
double c        	(double x, double y)    	{ return .1 * max (-Ac*(x-y)*(x-y)*(x-y)*(x-y) + (x-y)*(x-y) + 3, 0.0); }
double rescaling	(double x, int size)    	{ return 6.0*(x-1.0)/(size-1.0) - 3.0; }

double NewNu (vector<double> & nu, double K, double p, int size) {
	vector<double> B(size+1,0), De(size+1,0), M(size+1,0); double BB=0, DD=0, MM=0;
  
	for (int x=1; x<=size; x++) {
		double s=0; for (int y=1; y<=size; ++y) { s += c(rescaling(x,size),rescaling(y,size))*nu[y]; }
		B[x]  = nu[x] * (1-p) * b(rescaling(x,size));	BB += B[x];
		M[x]  = nu[x] * p * b(rescaling(x,size));    	MM += M[x];
		De[x] = nu[x] * s;                           	DD += De[x];
	}

    double u = prng.uniform_real()*(BB+DD+MM);  
    if (u < BB) {        	int z=0; double v = prng.uniform_real()*BB; while (v>0) { z++; v -= B[z]; } nu[z] += 1/K; }
    else if (u < BB+DD) {	int z=0; double v = prng.uniform_real()*DD; while (v>0) { z++; v -= De[z]; } nu[z] -= 1/K; if (nu[z]<.5/K) nu[z]=0; }
    else {               	int z=0; double v = prng.uniform_real()*MM; while (v>0) { z++; v -= M[z]; }
                         	int dz = 2*prng.bernoulli()-1; if ((z>0) && (z+dz>=1) && (z+dz<=size)) nu[z+dz] += 1/K; }

    return prng.exponential (K*(BB+DD+MM));
}

int main (int argc, char ** argv) {
	// n impair ! the function rescaling maps {1, ..., size} to {-3, ..., 3}; 7+(2^k*)6 is optimal rescaling
	CL_Parser CLP (argc,argv,"s=4,K=100,p=.05,n=7,i=1000000");
	prng.seed (int(CLP('s')));
	double K=CLP('K'), p=CLP('p');
	int size=CLP('n'), shift=(size+1)/2, iterations = CLP('i');

	vector<double> nu (size+1,0); nu[shift] = 3;

	vector<cpx> pos (size,0);
	for (int i=0; i<size; ++i) pos[i] = cpx(i,nu[i+1]);

	Console W;
	W.watch (slope, "Slope"); W.manage (slope,-100.0,100.0);
	W.watch (Ac, "Ac"); W.manage (Ac,.2,.4);
	W.show();

	Figure F (false);
	F.add (new Path(pos));
	F.show();

	Figure G (false);
	vector<vector<cpx>> graph (size+1);

	ProgressBar PB (iterations); double t=0;

	for (int k=1; k<=iterations; ++k) {
		PB.set(k);
		t += NewNu (nu,K,p,size);
		if (!(k%(iterations/1000))) {
			cout << k << " " << t; for (int i=1; i<=size; ++i) cout << " " << int(.2+nu[i]*K); cout << endl; 
			G.contents.clear(); 
			for (int i=0; i<size; ++i) { graph[i].push_back (cpx(1000.0*k/iterations,nu[i+1])); G.add (new Path(graph[i],Pen(Indexed(i)))); }
			if (! G.visible()) G.show(); G.step();
		}
		for (int i=0; i<size; ++i) pos[i] = cpx(i,nu[i+1]);
		F.contents.clear(); F.add (new Path(pos));
		F.step();	
	}
}
