#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class SRW3 : public Cube {
public:
    explicit SRW3(const string &title, size_t n) : Cube(title, {n, n, n}), n(n), pos({int64_t(n) / 2, int64_t(n) / 2, int64_t(n) / 2}) {}
    void step() {
        pos = pos + dz3[prng.uniform_int(6)];
        putp(pos, 255);
    }

    size_t n;
    coo3   pos;
};

int main(int argc, char **argv) {
    CLP  clp(argc, argv, "Simple random walk in dimension 3");
    auto n = clp.param("n", size_t(100), "Box size");
    clp.finalize();

    SRW3 C(clp.title, n);
    for (size_t t = 0; t < C.n * C.n; ++t) C.step();
    C.output_pov(clp.title);
}
