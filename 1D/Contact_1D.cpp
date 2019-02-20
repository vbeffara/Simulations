#include <vb/Automaton.h>
#include <vb/util/Hub.h>

namespace vb {
    template <> Color to_Color(int t) { return Indexed(t + 1); }
} // namespace vb

class Contact : public vb::Automaton<int> {
public:
    explicit Contact(const vb::Hub &H) : Automaton<int>(H.title, H['n'], 1, H['p']) {
        for (auto &s : (*this))
            if (vb::prng.bernoulli(H['e'])) s = 2;

        add_rule(1, {{}, {{0, 0}}});
        add_rule(H['l'], {{{0, 1}, {1, 0}}, {{1, 1}}});
        add_rule(H['l'], {{{0, 1}, {-1, 0}}, {{-1, 1}}});
        add_rule(H['l'], {{{0, 1}, {1, 2}}, {{1, 1}}});
        add_rule(H['l'], {{{0, 1}, {-1, 2}}, {{-1, 1}}});
        add_rule(H['m'], {{{0, 2}, {1, 0}}, {{1, 2}}});
        add_rule(H['m'], {{{0, 2}, {-1, 0}}, {{-1, 2}}});

        if (H['j']) {
            add_rule(H['m'], {{{0, 2}, {1, 1}, {2, 0}}, {{2, 2}}});
            add_rule(H['m'], {{{0, 2}, {-1, 1}, {-2, 0}}, {{-2, 2}}});
        }
    }
};

int main(int argc, char **argv) {
    vb::Hub H("1D contact process", argc, argv, "n=1000,e=0,l=1.6494,m=100,j,p");
    Contact T(H);
    T.show();
    while (true) T.swipe();
}
