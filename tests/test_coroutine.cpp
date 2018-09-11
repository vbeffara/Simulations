#include <vb/Hub.h>
#include <vb/Stream_lib.h>

using namespace std;
using namespace vb;

bool good(const Permutation & p) { return p[0] == 2; }
int  sec(const Permutation & p) { return p[2]; }

int main(int argc, char ** argv) {
    H.init("Testing coroutines", argc, argv, "n=5");
    auto S    = permutations(int(H['n']));
    auto SS   = filter(good, S);
    auto SSS  = fmap(sec, SS);
    auto SSSS = take(8, SSS);
    for (auto p : SSSS) H.L->info("{}", p);
}
