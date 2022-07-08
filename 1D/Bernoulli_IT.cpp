#include <spdlog/spdlog.h>
#include <tbb/parallel_for.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class fun {
public:
    fun(){};
    fun(std::initializer_list<double> js, std::initializer_list<double> ds) : jumps(js), shifts(ds){};

    auto index(double x) const -> size_t { return (std::lower_bound(begin(jumps), end(jumps), x) - begin(jumps)) - 1; };
    auto operator()(double x) -> double { return x + shifts[index(x)]; };
    void plot() {
        std::cout << "0.0 " << shifts[0] << '\n';
        for (size_t i = 1; i < jumps.size(); ++i) {
            std::cout << jumps[i] << ' ' << jumps[i] + shifts[i - 1] << '\n' << '\n';
            std::cout << jumps[i] << ' ' << jumps[i] + shifts[i] << '\n';
        };
        std::cout << "1.0 " << 1.0 + shifts.back() << '\n';
    }

    std::vector<double> jumps, shifts;
};

auto compose(const fun &f1, const fun &f2) -> fun {
    fun ans;
    for (size_t i = 0; i < f1.jumps.size(); ++i) {
        double x = f1.jumps[i];
        double y = x + f1.shifts[i];
        double l = f1.shifts[i] + (i == f1.jumps.size() - 1 ? 1.0 : f1.jumps[i + 1]);
        auto   j = f2.index(y);
        ans.jumps.push_back(x);
        ans.shifts.push_back(f1.shifts[i] + f2.shifts[j]);
        for (size_t k = j + 1; (k < f2.jumps.size()) && (f2.jumps[k] < l); ++k) {
            ans.jumps.push_back(f2.jumps[k] - f1.shifts[i]);
            ans.shifts.push_back(f1.shifts[i] + f2.shifts[k]);
        }
    }
    return ans;
}

auto indexed_compose(const vector<function<double(double)>> &fs, const vector<size_t> &etas, double x) -> double {
    for (auto eta : etas) x = fs[eta](x);
    return x;
}

auto run(size_t N, size_t T, double theta, bool v) -> double {
    vector<function<double(double)>> funs = {[theta](double x) { return (x < theta) ? x : x - theta; },
                                             [theta](double x) { return (x < theta) ? x + 1 - theta : x; }};

    vector<size_t> etas;
    for (size_t i = 0; i < N; ++i) etas.push_back(prng.uniform_int(funs.size()));

    vector<fun> Fs = {fun({0.0, theta}, {0, -theta}), fun({0.0, theta}, {1 - theta, 0})};

    vector<double> fs(T);
    tbb::parallel_for(size_t(0), T, [&](size_t i) { fs[i] = indexed_compose(funs, etas, double(i) / double(T)); });

    fun F({0.0}, {0.0});
    for (size_t i = 0; i < N; ++i) F = compose(F, Fs[etas[i]]);
    F.plot();

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
