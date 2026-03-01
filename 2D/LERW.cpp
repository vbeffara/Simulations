#include <vb/Bitmap.h>
#include <vb/Path.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

namespace vb {
    template <> auto to_Color(uint8_t t) -> Color { return Grey(t); }
} // namespace vb

class LERW : private Bitmap<uint8_t> {
public:
    LERW(const std::string &title, size_t n) : Bitmap<uint8_t>(title, {2 * n, 2 * n}) {
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
    CLP  clp(argc, argv, "Loop-erased random walk");
    auto n = clp.param("n", 500, "half grid size");
    clp.finalize();

    LERW(clp.title, n).output(clp.title);
}
