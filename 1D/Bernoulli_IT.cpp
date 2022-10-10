#include <boost/icl/interval_set.hpp>
#include <spdlog/spdlog.h>
#include <utility>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;
using iset = boost::icl::interval_set<double>;
using intv = iset::interval_type;

/* A function that is piecewise of the form $x \mapsto x + d$. The function is
codes as a sequence `jumps` of points in the definition interval and a sequence
`shifts` of reals: the meaning being that on the interval between `jumps[i]` and
`jumps[i+1]`, the function is of the form `x + shifts[i]. */

class fun {
public:
    fun(std::vector<double> js, std::vector<double> ds) : jumps(std::move(js)), shifts(std::move(ds)){};

    [[nodiscard]] auto index(double x) const -> size_t { return (std::lower_bound(begin(jumps), end(jumps), x) - begin(jumps)) - 1; };

    [[nodiscard]] auto jump(size_t i) const -> double { return i < jumps.size() ? jumps[i] : 1.0; }

    [[nodiscard]] auto shift(size_t i) const -> double { return i < shifts.size() ? shifts[i] : 0.0; }

    void plot() const {
        for (size_t i = 0; i < jumps.size(); ++i) {
            fmt::print("{} {}\n", jump(i), jump(i) + shift(i));
            fmt::print("{} {}\n\n", jump(i + 1), jump(i + 1) + shift(i));
        }
    }

    [[nodiscard]] auto range(size_t i) const -> intv { return intv::right_open(jump(i) + shift(i), jump(i + 1) + shift(i)); }

    [[nodiscard]] auto range() const -> iset {
        iset out;
        for (size_t i = 0; i < jumps.size(); ++i) out.insert(range(i));
        return out;
    }

    [[nodiscard]] auto compose(const fun &o) const -> fun {
        std::vector<double> nj, ns;
        for (size_t i = 0; i < jumps.size(); ++i) {
            auto I = range(i);
            auto j = o.index(I.lower());
            nj.push_back(jump(i));
            ns.push_back(shift(i) + o.shift(j));
            for (size_t k = j + 1; o.jump(k) < I.upper(); ++k) {
                nj.push_back(o.jump(k) - shift(i));
                ns.push_back(shift(i) + o.shift(k));
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
        // fmt::print("{} {} {}\n", i, l, upper(R) - lower(R));
    };
    F.plot();
}

auto main(int argc, char **argv) -> int {
    Hub const H("Bernoullicity of [I,T]", argc, argv, "n=1,t=1000,s=.61803398874989484820,v,k=1");
    run(H['s'], H['n']);
}
