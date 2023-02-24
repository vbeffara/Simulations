#include <boost/icl/interval_set.hpp>
#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>
#include <vector>

using iset = boost::icl::interval_set<double>;
using intv = iset::interval_type;

// This represents the map x \mapsto x + d on the interval [a,b]
struct piece {
    double a, b, d;
};

class function : public std::vector<piece> {
public:
    void add(const piece &p) {
        if ((!empty()) && (p.a == back().b) && (p.d == back().d))
            back().b = p.b;
        else
            push_back(p);
    }

    void print() const {
        for (const auto &p : *this) { fmt::print("[{},{}] + {}\n", p.a, p.b, p.d); }
    }

    auto split(const piece &p) -> function {
        function g;
        for (auto q : *this) {
            double a = std::max(p.a, q.a - p.d), b = std::min(p.b, q.b - p.d);
            if (a < b) g.add({a, b, p.d + q.d});
        };
        return g;
    }

    // Compose inside
    auto compose(const function &f1) -> function {
        function g;
        for (const auto &p : f1) {
            auto pf2 = split(p);
            for (const auto &q : pf2) g.add(q);
        }
        return g;
    }

    [[nodiscard]] auto range() const -> iset {
        iset out;
        for (const auto &p : *this) out.insert(intv::right_open(p.a + p.d, p.b + p.d));
        return out;
    }

    void plot() const {
        for (const auto &p : *this) { fmt::print("{} {}\n{} {}\n\n", p.a, p.a + p.d, p.b, p.b + p.d); }
    }
};

auto measure(const iset &I) -> double {
    double out = 0;
    for (const auto &i : I) out += upper(i) - lower(i);
    return out;
}

auto hole(const iset &I) -> double {
    iset delta;
    delta.insert(intv::right_open(-1.0, 2.0));
    delta -= I;

    double hole_of_zero = 0, biggest_hole = 0;
    for (const auto &i : delta) {
        if (lower(i) < -.5) {
            hole_of_zero += upper(i);
            continue;
        }
        if (upper(i) > 1.5) {
            hole_of_zero += 1 - lower(i);
            continue;
        }
        biggest_hole = std::max(biggest_hole, upper(i) - lower(i));
    };
    return std::max(hole_of_zero, biggest_hole);
}

auto main(int argc, char **argv) -> int {
    vb::Hub const H("Bernoullicity of [I,T]", argc, argv, "n=1000,i");

    double                theta = .61803398874989484820;
    function const        f1{{{0.0, theta, 0.0}, {theta, 1.0, -theta}}};
    function const        f2{{{0.0, theta, 1 - theta}, {theta, 1.0, 0.0}}};
    std::vector<function> ff{f1, f2};

    function f{{{0.0, 1.0, 0.0}}};
    double   avg = 1;
    for (int i = 0; i < int(H['n']); ++i) {
        if (bool(H['i']))
            f = f.compose(ff[vb::prng.uniform_int(ff.size())]);
        else
            f = ff[vb::prng.uniform_int(ff.size())].compose(f);
        auto R = f.range();
        avg    = .999 * avg + .001 * (1 - hole(R));
        fmt::print("{} {} {}\n", measure(R), 1 - hole(R), avg);
    }
    // for (const auto &p : f) fmt::print("{} {}\n", p.a + p.d, p.b + p.d);
    // f.print();
}