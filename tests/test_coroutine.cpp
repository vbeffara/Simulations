#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>

using namespace std;
using namespace vb;

auto good(const Permutation &p) -> bool { return p[0] == 2; }
auto sec(const Permutation &p) { return p[2]; }

auto main(int argc, char **argv) -> int {
    Hub  H("Testing coroutines", argc, argv, "n=5");
    auto S    = permutations(H['n']);
    auto SS   = filter(good, move(S));
    auto SSS  = fmap(sec, move(SS));
    auto SSSS = SSS | ranges::views::take(8);
    for (const auto &p : SSSS) spdlog::info("{}", p);
}
