#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class SRW3 : public Cube {
public:
    explicit SRW3(const Hub &H, size_t n) : Cube(H.title, {n, n, n}), n(n), pos({int64_t(n) / 2, int64_t(n) / 2, int64_t(n) / 2}) {}
    void step() {
        pos = pos + dz3[prng.uniform_int(6)];
        putp(pos, 255);
    }

    size_t n;
    coo3   pos;
};

int main(int argc, char **argv) {
    Hub  H("Simple random walk in dimension 3", argc, argv, "n=100");
    SRW3 C(H, H['n']);
    for (size_t t = 0; t < C.n * C.n; ++t) C.step();
    C.output_pov(H.title);
}
