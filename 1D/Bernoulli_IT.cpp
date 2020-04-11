#include <spdlog/spdlog.h>
#include <tbb/parallel_for.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto indexed_compose(const vector<function<double(double)>> &fs, const vector<size_t> &etas, double x) -> double {
    for (auto eta : etas) x = fs[eta](x);
    return x;
}

auto run(size_t N, size_t T, double theta, bool v) -> double {
    vector<function<double(double)>> funs = {[theta](double x) { return (x < theta) ? x : x - theta; },
                                             [theta](double x) { return (x < theta) ? x + 1 - theta : x; }};

    vector<size_t> etas;
    for (size_t i = 0; i < N; ++i) etas.push_back(prng.uniform_int(funs.size()));

    vector<double> fs(T);
    tbb::parallel_for(size_t(0), T, [&](size_t i) { fs[i] = indexed_compose(funs, etas, double(i) / double(T)); });

    if (v)
        for (size_t i = 0; i < T; ++i) cout << double(i) / double(T) << ' ' << fs[i] << '\n';

    double s1 = 0.0, s2 = 0.0;
    for (auto u : fs) s1 += cos(2 * M_PI * u);
    for (auto u : fs) s2 += cos(2 * M_PI * u) * cos(2 * M_PI * u);
    return s2 / double(T) - s1 * s1 / double(T * T);
}

auto main(int argc, char **argv) -> int {
    Hub H("Bernoullicity of [I,T]", argc, argv, "n=1,t=1000,s=.61803398874989484820,v,k=1");

    double sum = 0;
    for (int k = 0; k < int(H['k']); ++k) sum += run(H['n'], H['t'], H['s'], H['v']);
    spdlog::info("Average variance over {} runs of {} compositions: {}", int(H['k']), int(H['n']), sum / int(H['k']));
}
