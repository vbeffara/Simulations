#include <ranges>
#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>

using namespace vb;
namespace rv = std::views;

auto good(const Permutation &p) -> bool { return p[0] == 2; }
auto sec(const Permutation &p) { return p[2]; }

auto main(int argc, char **argv) -> int {
    Hub const H("Testing coroutines", argc, argv, "n=5");
    for (const auto &p : permutations(H['n']) | rv::filter(good) | rv::transform(sec) | rv::take(8)) spdlog::info("{}", p);
}
