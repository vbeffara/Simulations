#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>

using namespace std;
using namespace vb;
namespace rv = ranges::views;

auto good(const Permutation &p) -> bool { return p[0] == 2; }
auto sec(const Permutation &p) { return p[2]; }

auto main(int argc, char **argv) -> int {
    Hub  H("Testing coroutines", argc, argv, "n=5");
    ranges::for_each(permutations(H['n']) | rv::filter(good) | rv::transform(sec) | rv::take(8), [](auto p) { spdlog::info("{}", p); });
}
