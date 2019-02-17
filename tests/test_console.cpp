#include <vb/Console.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Testing Console", argc, argv);
    double x = 1.1;

    Console W;
    W.watch(x, "x");
    W.show();

    for (int64_t i = 0; i < 1e8; ++i) {
        x = prng.uniform_real();
        W.step();
    } // Usually W.step() would be implicit

    W.lambda<double>([&x] { return 2 * x; }, "Double of x");

    for (int64_t i = 0; i < 1e8; ++i) {
        x = prng.uniform_real();
        W.step();
    }
}
