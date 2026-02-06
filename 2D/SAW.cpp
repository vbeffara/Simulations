#include <vb/Path.h>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Self-avoiding walk");
    auto n = clp.param("n", 500, "Path length");
    auto t = clp.param("t", 5000, "Number of iterations");
    clp.finalize();

    OldPath     P(size_t(n), true);
    ProgressBar PB(static_cast<size_t>(t));

    for (size_t i = 0; i < size_t(t);) {
        PB.set(i);
        auto j = prng.uniform_int(size_t(n));
        auto k = uint8_t(1) + prng.uniform_int(uint8_t(3));
        auto l = P[j];
        P[j]   = static_cast<unsigned char>((P[j] + k) % 4);
        if (P.self_avoiding())
            i++;
        else
            P[j] = l;
    }

    P.output(clp.title);
}
