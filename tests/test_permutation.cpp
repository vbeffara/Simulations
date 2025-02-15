#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>
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

  Hub const H("Streams and permutations", argc, argv, "n=10");
  for (size_t k = 1; k <= size_t(H['n']); ++k) {
    size_t perm_count = 0, part_count = 0;
    for (auto &&p : permutations(k)) ++perm_count;
    for (auto &&p : partitions(k)) ++part_count;
    spdlog::info("n: {:>2}  permutations: {:>7}  partitions: {:>2}", k, perm_count, part_count);
  }
}
