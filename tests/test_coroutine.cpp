#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/Stream_lib.h>

using namespace std;
using namespace vb;

bool good(const Permutation &p) { return p[0] == 2; }
int  sec(const Permutation &p) { return p[2]; }

int main(int argc, char **argv) {
    Hub  H("Testing coroutines", argc, argv, "n=5");
    auto S    = permutations(int(H['n']));
    auto SS   = filter(good, move(S));
    auto SSS  = fmap(sec, move(SS));
    auto SSSS = take(8, move(SSS));
    for (const auto &p : SSSS) spdlog::info("{}", p);
}
