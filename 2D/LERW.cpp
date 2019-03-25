#include <vb/Bitmap.h>
#include <vb/Path.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

namespace vb {
    template <> Color to_Color(uint8_t t) { return Grey(t); }
} // namespace vb

class LERW : private Bitmap<uint8_t> {
public:
    explicit LERW(const Hub &H) : Bitmap<uint8_t>(H.title, {2 * size_t(H['n']), 2 * size_t(H['n'])}) {
        auto z = coo(ucoo{w() / 2, h() / 2});
        // TODO: contains for ucoo
        while (contains(z)) {
            auto d = prng.uniform_int(uint8_t(4));
            put(ucoo(z), d);
            z += dz[d];
        }
    }

    void output(const std::string &s) override {
        OldPath P(0);
        auto    z = coo(ucoo{w() / 2, h() / 2});
        while (contains(z)) {
            auto d = at(ucoo(z));
            P.push_back(d);
            z += dz[d];
        }
        P.output(s);
    }
};

int main(int argc, char **argv) {
    Hub H("Loop-erased random walk", argc, argv, "n=500");
    LERW(H).output(H.title);
}
