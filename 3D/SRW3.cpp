#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class SRW3 : public Cube {
public:
    explicit SRW3(const string &title_, size_t n_)
        : Cube(title_, {n_, n_, n_}), n(n_), pos({int64_t(n) / 2, int64_t(n) / 2, int64_t(n) / 2}) {}
    void step() {
        pos = pos + dz3[prng.uniform_int(6U)];
        putp(pos, 255);
    }

    size_t n;
    coo3   pos;
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Simple random walk in dimension 3");
    auto n = clp.param("n", size_t(100), "Box size");
    clp.finalize();

    SRW3 C(clp.title, n);
    for (size_t t = 0; t < C.n * C.n; ++t) C.step();
    C.output_pov(clp.title);
}
