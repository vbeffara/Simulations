#include <vb/Hub.h>
#include <vb/Minimizer.h>

using namespace std; using namespace vb;

#ifdef CMAES
#include <cmaes.h>
using namespace libcmaes;

FitFunc f = [](const double *x, const int N) {
  double o = 0;
  for (int i=0; i<N; ++i) { double xi = x[i]-i; o += 1 - cos(xi*xi) + .01*xi*xi; }
  return o;
};

GradFunc df = [](const double *x, const int N) {
  dVec grad(N);
  for (int i=0; i<N; i++) { double xi = x[i]-i; grad(i) = xi * (2 * sin (xi*xi) + .02); }
  return grad;
};

ProgressFunc <CMAParameters<>,CMASolutions> pf = [](const CMAParameters<> &cmaparams, const CMASolutions &cmasols) {
	H.L->trace ("Current best : {}", cmasols.get_best_seen_candidate().get_fvalue());
	return 0;
};

int main (int argc, char ** argv) {
	H.init ("Tests of minimization strategies", argc, argv, "d=10,s=1,l=-1");
	int dim = H['d'], lambda = H['l'];
	double sigma = H['s'];

	vector<double> x0 (dim,0);
	CMAParameters<> cmaparams(x0,sigma,lambda);
	cmaparams.set_algo (aCMAES);

	spdlog::set_level(spdlog::level::trace);
	auto cmasols = cmaes<>(f,cmaparams,pf,df);

	H.L->info ("Final value : {}", cmasols.get_best_seen_candidate().get_fvalue());
	std::cout << cmasols << std::endl;
}

#else
int main () { cerr << "Nothing to see here, move along.\n"; }
#endif
