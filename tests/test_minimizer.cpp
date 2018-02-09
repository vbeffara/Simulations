#include <vb/Minimizer.h>
#include <vb/config.h>
#include <cmaes.h>
#include <gsl/gsl>

using namespace std;
using namespace vb;
using namespace libcmaes;

int main(int argc, char ** argv) {
    H.init("Tests of minimization strategies", argc, argv, "d=10,s=1,l=-1,t,a=acmaes,m");
    int    dim = H['d'], lambda = H['l'];
    double sigma = H['s'];

    FitFunc f = [](const double * xx, const int N) {
        gsl::span<const double> x(xx, N);
        double                  o = 0;
        for (int i = 0; i < N; ++i) {
            double xi = x[i] - i;
            o += 1 - cos(xi * xi) + .01 * xi * xi;
        }
        return o;
    };

    GradFunc df = [](const double * xx, const int N) {
        gsl::span<const double> x(xx, N);
        dVec                    grad(N);
        for (int i = 0; i < N; i++) {
            double xi = x[i] - i;
            grad(i)   = xi * (2 * sin(xi * xi) + .02);
        }
        return grad;
    };

    ProgressFunc<CMAParameters<>, CMASolutions> pf = [](const CMAParameters<> &, const CMASolutions & cmasols) {
        H.L->trace("Current best : {}", cmasols.get_best_seen_candidate().get_fvalue());
        return 0;
    };

    vector<double>  x0(dim, 0);
    CMAParameters<> cmaparams(x0, sigma, lambda);
    cmaparams.set_str_algo(H['a']);
    cmaparams.set_mt_feval(H['m']);

    if (H['t']) spdlog::set_level(spdlog::level::trace);
    auto cmasols = cmaes<>(f, cmaparams, pf, df);

    H.L->info("Final value : {}", cmasols.get_best_seen_candidate().get_fvalue());
    std::cout << cmasols << std::endl;
}
