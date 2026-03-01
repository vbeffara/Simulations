#include <vb/Automaton.h>
#include <vb/util/CLP.h>

namespace vb {
    template <> auto to_Color(int t) -> Color { return Indexed(t + 1); }
} // namespace vb

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "1D contact process");
    auto    n = clp.param("n", size_t(1000), "Grid size");
    auto    e = clp.param("e", 0.0, "Initial infection probability");
    auto    l = clp.param("l", 1.6494, "Lambda parameter");
    auto    m = clp.param("m", 100.0, "Mu parameter");
    auto    j = clp.flag("j", "Enable jump rules");
    auto    p = clp.flag("p", "Pause on start");
    clp.finalize();

    vb::Automaton<int> T(clp.title, n, 1, p);
    for (auto &s : T)
        if (vb::prng.bernoulli(e)) s = 2;

    T.add_rule(1, {{}, {{0, 0}}});
    T.add_rule(l, {{{0, 1}, {1, 0}}, {{1, 1}}});
    T.add_rule(l, {{{0, 1}, {-1, 0}}, {{-1, 1}}});
    T.add_rule(l, {{{0, 1}, {1, 2}}, {{1, 1}}});
    T.add_rule(l, {{{0, 1}, {-1, 2}}, {{-1, 1}}});
    T.add_rule(m, {{{0, 2}, {1, 0}}, {{1, 2}}});
    T.add_rule(m, {{{0, 2}, {-1, 0}}, {{-1, 2}}});

    if (j) {
        T.add_rule(m, {{{0, 2}, {1, 1}, {2, 0}}, {{2, 2}}});
        T.add_rule(m, {{{0, 2}, {-1, 1}, {-2, 0}}, {{-2, 2}}});
    }

    T.show();
    while (true) T.swipe();
}
