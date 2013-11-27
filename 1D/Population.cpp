#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <cmath>
using namespace std; using namespace vb;

double a0 = 10, aex1 = 20, aex2 = 20, sigmab0 = .5, sigmabex1 = .1, sigmabex2 = .1, Ac = 100, sigmac = .1;

double b (double x) { return a0 * exp (-x*x/(2*sigmab0*sigmab0)) + aex1 * exp (-(x-2)*(x-2)/(2*sigmabex1*sigmabex1)) + aex2 * exp (-(x+2)*(x+2)/(2*sigmabex2*sigmabex2)); }
double c (double x, double y) { return .1 * max (-.3*(x-y)*(x-y)*(x-y)*(x-y) + (x-y)*(x-y) + 3, 0.0); }

double rescaling (double x, int size) { return 6.0*(x-1.0)/(size-1.0) - 3.0; }

double NewNu (vector<double> & nu, int K, double p, int size) {
	vector<double> B   (size+1,0); double BB = 0;
	vector<double> De  (size+1,0); double DD = 0;
	vector<double> M   (size+1,0); double MM = 0;
  
	for (int x=1; x<=size; x++) {
		double s=0; for (int y=1; y<=size; ++y) { s += c(rescaling(x,size),rescaling(y,size))*nu[y]; }
		B[x]  = nu[x] * (1-p) * b(rescaling(x,size));
		M[x]  = nu[x] * p * b(rescaling(x,size));
		De[x] = nu[x] * s;
		BB += B[x]; DD += De[x]; MM += M[x]; 
	}
	double u = prng.uniform_real();
  
	if (u < BB/(BB+DD+MM)) {
		int z=0; double v = prng.uniform_real()*BB;
		while (v>0) { z++; v -= B[z]; }
		nu[z] += 1.0/K;
    } else if (u < (BB+DD)/(BB+DD+MM)) {
		int z=0; double v = prng.uniform_real()*DD;
		while (v>0) { z++; v -= De[z]; }
		nu[z] -= 1.0/K; if (nu[z]<.5/K) nu[z]=0;
    } else { 
		int z=0; double v = prng.uniform_real()*MM;
		while (v>0) { z++; v -= M[z]; }
		int dz = 2*prng.bernoulli()-1;
		if ((z+dz>=1) && (z+dz<=size)) nu[z+dz] += 1.0/K;
	}
	return K*(BB+DD+MM);
}

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"s=4,K=100,p=.05,n=7,i=1000000"); // n impair ! the function rescaling maps {1, ..., size} to {-3, ..., 3} 7+(2^k*)6 is optimal rescaling
	prng.seed (int(CLP('s')));
	double K=CLP('K'), p=CLP('p');
	int size=CLP('n'), shift=(size+1)/2;

	vector<double> nu (size+1,0); nu[shift] = 3;

	double t=0;
	int iterations = CLP('i');

	vector<vector<double>> nut (1,nu);
	ProgressBar PB (iterations);

	for (int k=1; k<=iterations; ++k) {
		PB.set(k);
		double Tot = NewNu (nu,K,p,size);
		// nut.push_back(nu);
		t += prng.exponential (Tot);
		if (!(k%(iterations/1000))) {cout << k << " " << t; for (int i=1; i<=size; ++i) cout << " " << int(.2+nu[i]*K); cout << endl; }
	}
}
