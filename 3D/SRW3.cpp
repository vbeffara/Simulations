#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class SRW3 : public Cube {
public:
    explicit SRW3(const Hub &H, int n_) : Cube(H.title, {n_, n_, n_}), n(n_), pos({n / 2, n / 2, n / 2}) {}
    void step() {
        pos = pos + dz3[prng.uniform_int(6)];
        putp(pos, 255);
    }

    int  n;
    coo3 pos;
};

int main(int argc, char **argv) {
    Hub  H("Simple random walk in dimension 3", argc, argv, "n=100");
    SRW3 C(H, H['n']);
    for (int t = 0; t < C.n * C.n; ++t) C.step();
    C.output_pov(H.title);
}
