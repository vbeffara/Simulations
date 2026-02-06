#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/CLP.h>
#include <vb/util/Stream.h>

using namespace vb;
using namespace std;

auto f() -> Stream2<int> {
  int i = 0;
  while (true) co_yield i++;
}

auto main(int argc, char **argv) -> int {
  for (const int i : f()) {
    if (i > 10) break;
    spdlog::info("i: {}", i);
  }

  CLP clp(argc, argv, "Streams and permutations");
  auto n = clp.param("n", size_t(10), "Max permutation size");
  clp.finalize();
  for (size_t k = 1; k <= n; ++k) {
    size_t perm_count = 0, part_count = 0;
    for (auto &&p : permutations(k)) ++perm_count;
    for (auto &&p : partitions(k)) ++part_count;
    spdlog::info("n: {:>2}  permutations: {:>7}  partitions: {:>2}", k, perm_count, part_count);
  }
}
