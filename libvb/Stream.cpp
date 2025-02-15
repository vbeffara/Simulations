#include <vb/util/Stream.h>
#include <vector>

namespace vb {
  auto partitions(size_t n, size_t m) -> Stream2<std::vector<size_t>> {
    for (size_t i = m; i < n; ++i) {
      for (const auto &p : partitions(n - i, i)) {
        std::vector<size_t> out({i});
        for (auto j : p) out.push_back(j);
        co_yield (out);
      }
    }
    if (n >= m) co_yield (std::vector<size_t>{n});
  }

  auto tuples(size_t k, size_t n) -> Stream2<std::vector<size_t>> {
    if (k == 0) {
      co_yield (std::vector<size_t>{});
    } else {
      for (size_t i = 0; i < n; ++i) {
        for (const auto &c : tuples(k - 1, n - 1)) {
          std::vector<size_t> out({i});
          for (auto j : c) out.push_back(j < i ? j : j + 1);
          co_yield (out);
        }
      }
    }
  }

  auto cycles(size_t k, size_t n) -> Stream2<std::vector<size_t>> {
    for (size_t i = 0; i < n - k + 1; ++i) {
      for (const auto &c : tuples(k - 1, n - i - 1)) {
        std::vector<size_t> out(1, i);
        for (auto j : c) out.push_back(i + j + 1);
        co_yield (out);
      }
    }
  }
} // namespace vb
