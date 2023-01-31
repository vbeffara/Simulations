#include <boost/icl/interval_set.hpp>
#include <spdlog/spdlog.h>
#include <vb/util/PRNG.h>
#include <vector>

using iset = boost::icl::interval_set<double>;
using intv = iset::interval_type;

// This represents the map x \mapsto x + d on [a,b]
struct piece {
    double a, b, d;
};

using function = std::vector<piece>;

function split(piece p, function f) {
    function g;
    for (auto q : f) {
        // The interval [aa,bb] is what is sent into q by x \mapsto x + d
        double aa = q.a - p.d;
        double bb = q.b - p.d;
        // If [aa,bb] intersects p, this gives a piece of the output
        double aaa = std::max(p.a, aa);
        double bbb = std::min(p.b, bb);
        if (aaa < bbb) { g.push_back({aaa, bbb, p.d + q.d}); }
    };
    return g;
}

function compose(function f1, function f2) {
    function g;
    for (const auto &p : f1) {
        auto pf2 = split(p, f2);
        for (const auto &q : pf2) g.push_back(q);
    }
    return g;
}

iset range(const function &f) {
    iset out;
    for (const auto &p : f) out.insert(intv::right_open(p.a + p.d, p.b + p.d));
    return out;
}

void plot(const function &f) {
    for (const auto &p : f) { fmt::print("{} {}\n{} {}\n\n", p.a, p.a + p.d, p.b, p.b + p.d); }
}

double measure(const iset &I) {
    double out = 0;
    for (const auto &i : I) out += upper(i) - lower(i);
    return out;
}

double hole(const iset &I) {
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

int main() {
    double                theta = .61803398874989484820;
    function              f1{{{0.0, theta, 0.0}, {theta, 1.0, -theta}}};
    function              f2{{{0.0, theta, 1 - theta}, {theta, 1.0, 0.0}}};
    std::vector<function> ff{f1, f2};

    function f{{0.0, 1.0, 0.0}};
    for (int i = 0; i < 2000; ++i) {
        f      = compose(f, ff[vb::prng.uniform_int(ff.size())]);
        auto R = range(f);
        fmt::print("{} {} {}\n", measure(R), 1 - hole(R), upper(R) - lower(R));
    }
    // for (const auto &p : f) fmt::print("{} {}\n", p.a + p.d, p.b + p.d);
}