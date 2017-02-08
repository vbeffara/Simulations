#include <vb/Hub.h>
#include <vb/Minimizer.h>

#ifdef CMAES
#include <cmaes.h>
using namespace libcmaes;
#endif

using namespace std; using namespace vb;

FitFunc f = [](const double *x, const int N) {
  double o = 0;
  for (int i=0; i<N; ++i) { double xi = x[i]-i; o += 1 - cos(xi*xi) + .01*xi*xi; }
  return o;
};

int main (int argc, char ** argv) {
	H.init ("Tests of minimization strategies", argc, argv, "d=10,s=1,l=-1");
	int dim = H['d'];
	double sigma = H['s'];
	int lambda = H['l'];

	vector<double> x0 (dim,0);
	CMAParameters<> cmaparams(x0,sigma,lambda);
	CMASolutions cmasols = cmaes<>(f,cmaparams);

	std::cout << cmasols << std::endl;

	auto C = cmasols.get_best_seen_candidate();
	H.L->info ("Final value : {}", C.get_fvalue());
}
