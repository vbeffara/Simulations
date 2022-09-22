#include <vb/Bitmap.h>
#include <vb/Path.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

namespace vb {
    template <> auto to_Color(uint8_t t) -> Color { return Grey(t); }
} // namespace vb

class LERW : private Bitmap<uint8_t> {
public:
    explicit LERW(const Hub &H) : Bitmap<uint8_t>(H.title, {2 * size_t(H['n']), 2 * size_t(H['n'])}) {
        ucoo z = size / 2;
        while (fits(z)) {
            auto d = prng.uniform_int(uint8_t(4));
            put(z, d);
            z += dz[d];
        }
    }

    void output(const std::string &s) override {
        OldPath P(0);
        ucoo    z = size / 2;
        while (fits(z)) {
            auto d = at(z);
            P.push_back(d);
            z += dz[d];
        }
        P.output(s);
    }
};

auto main(int argc, char **argv) -> int {
    Hub const H("Loop-erased random walk", argc, argv, "n=500");
    LERW(H).output(H.title);
}
