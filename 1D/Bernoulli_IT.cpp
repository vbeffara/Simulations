#include <boost/icl/interval_set.hpp>
#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;
using iset = boost::icl::interval_set<double>;
using intv = iset::interval_type;

class fun {
public:
    fun(const std::vector<double> js, const std::vector<double> ds) : jumps(js), shifts(ds){};

    auto index(double x) const -> size_t { return (std::lower_bound(begin(jumps), end(jumps), x) - begin(jumps)) - 1; };

    auto jump(size_t i) const -> double { return i < jumps.size() ? jumps[i] : 1.0; }

    void plot() const {
        for (size_t i = 0; i < jumps.size(); ++i) {
            fmt::print("{} {}\n", jump(i), jump(i) + shifts[i]);
            fmt::print("{} {}\n\n", jump(i + 1), jump(i + 1) + shifts[i]);
        }
    }

    auto range(size_t i) const -> intv { return intv::right_open(jumps[i] + shifts[i], jump(i + 1) + shifts[i]); }

    auto range() const -> iset {
        iset out;
        for (size_t i = 0; i < jumps.size(); ++i) out.insert(range(i));
        return out;
    }

    auto compose(const fun &o) const -> fun {
        std::vector<double> nj, ns;
        for (size_t i = 0; i < jumps.size(); ++i) {
            auto I = range(i);
            auto j = o.index(I.lower());
            nj.push_back(jumps[i]);
            ns.push_back(shifts[i] + o.shifts[j]);
            for (size_t k = j + 1; o.jump(k) < I.upper(); ++k) {
                nj.push_back(o.jumps[k] - shifts[i]);
                ns.push_back(shifts[i] + o.shifts[k]);
            }
        }
        return {nj, ns};
    }

private:
    std::vector<double> jumps, shifts;
};

auto run(double theta, size_t N) {
    vector<fun> Fs = {fun({0.0, theta}, {0, -theta}), fun({0.0, theta}, {1 - theta, 0})};
    fun         F({0.0}, {0.0});
    for (size_t i = 0; i < N; ++i) {
        // F        = F.compose(Fs[prng.uniform_int(Fs.size())]);
        F        = Fs[prng.uniform_int(Fs.size())].compose(F);
        auto   R = F.range();
        double l = std::accumulate(begin(R), end(R), 0.0, [](double s, const intv &I) { return s + upper(I) - lower(I); });
        fmt::print("{} {} {}\n", i, l, upper(R) - lower(R));
    };
    // F.plot();
}

auto main(int argc, char **argv) -> int {
    Hub H("Bernoullicity of [I,T]", argc, argv, "n=1,t=1000,s=.61803398874989484820,v,k=1");
    run(H['s'], H['n']);
}
