#include <ranges>
#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/CLP.h>

using namespace vb;
namespace rv = std::ranges::views;

auto good(const Permutation &p) -> bool { return p[0] == 2; }
auto sec(const Permutation &p) { return p[2]; }

auto main(int argc, char **argv) -> int {
  CLP clp(argc, argv, "Testing coroutines");
  auto n = clp.param("n", 5, "Permutation size");
  clp.finalize();
  //   for (const auto &p : permutations(n) | rv::filter(good) | rv::transform(sec) | rv::take(8)) spdlog::info("{}", p);
  spdlog::error("This test is disabled");
}
