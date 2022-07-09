#include <boost/icl/interval_set.hpp>
#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;
using iset = boost::icl::interval_set<double>;

class fun {
public:
    fun(const std::vector<double> js, const std::vector<double> ds) : jumps(js), shifts(ds){};

    auto index(double x) const -> size_t { return (std::lower_bound(begin(jumps), end(jumps), x) - begin(jumps)) - 1; };

    auto right(size_t i) const -> double { return i < jumps.size() ? jumps[i] : 1.0; }

    auto operator()(double x) const -> double { return x + shifts[index(x)]; };

    void plot() const {
        std::cout << "0.0 " << shifts[0] << '\n';
        for (size_t i = 1; i < jumps.size(); ++i) {
            std::cout << jumps[i] << ' ' << jumps[i] + shifts[i - 1] << '\n' << '\n';
            std::cout << jumps[i] << ' ' << jumps[i] + shifts[i] << '\n';
        };
        std::cout << "1.0 " << 1.0 + shifts.back() << '\n';
    }

    auto range() const -> iset {
        iset out;
        for (size_t i = 0; i < jumps.size(); ++i)
            out.insert(iset::interval_type::right_open(jumps[i] + shifts[i], right(i + 1) + shifts[i]));
        return out;
    }

    std::vector<double> jumps, shifts;
};

// This computes the composition `f2 o f1`
auto compose(const fun &f1, const fun &f2) -> fun {
    std::vector<double> jumps, shifts;
    for (size_t i = 0; i < f1.jumps.size(); ++i) {
        double x = f1.jumps[i];
        double l = f1.shifts[i] + f1.right(i + 1);
        auto   j = f2.index(x + f1.shifts[i]);
        jumps.push_back(x);
        shifts.push_back(f1.shifts[i] + f2.shifts[j]);
        for (size_t k = j + 1; (k < f2.jumps.size()) && (f2.jumps[k] < l); ++k) {
            jumps.push_back(f2.jumps[k] - f1.shifts[i]);
            shifts.push_back(f1.shifts[i] + f2.shifts[k]);
        }
    }
    return {jumps, shifts};
}

auto rand_compose(const vector<fun> fs, const vector<size_t> is) -> fun {
    fun F({0.0}, {0.0});
    for (auto i : is) F = compose(F, fs[i]);
    return F;
}

auto run(double theta, size_t N) {
    vector<fun>    Fs = {fun({0.0, theta}, {0, -theta}), fun({0.0, theta}, {1 - theta, 0})};
    vector<size_t> etas;
    fun            F({0.0}, {0.0});
    for (size_t i = 0; i < N; ++i) {
        F        = compose(Fs[prng.uniform_int(Fs.size())], F);
        auto   R = F.range();
        double l{0.0};
        for (const auto &I : R) l += I.upper() - I.lower();
        std::cout << i << ' ' << l << ' ' << boost::icl::upper(R) - boost::icl::lower(R) << '\n';
    };
    // F.plot();
}

auto main(int argc, char **argv) -> int {
    Hub H("Bernoullicity of [I,T]", argc, argv, "n=1,t=1000,s=.61803398874989484820,v,k=1");
    run(H['s'], H['n']);
}
