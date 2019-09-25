#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    auto partitions(size_t n, size_t m) -> Stream<std::vector<size_t>> {
        return Stream<std::vector<size_t>>([n, m](Sink<std::vector<size_t>> &yield) {
            for (size_t i = m; i < n; ++i) {
                for (const auto &p : partitions(n - i, i)) {
                    std::vector<size_t> out({i});
                    for (auto j : p) out.push_back(j);
                    yield(out);
                }
            }
            if (n >= m) yield({n});
        });
    }

    auto tuples(size_t k, size_t n) -> Stream<std::vector<size_t>> {
        return Stream<std::vector<size_t>>([k, n](Sink<std::vector<size_t>> &yield) {
            if (k == 0) {
                yield({});
                return;
            }
            for (size_t i = 0; i < n; ++i) {
                for (const auto &c : tuples(k - 1, n - 1)) {
                    std::vector<size_t> out({i});
                    for (auto j : c) out.push_back(j < i ? j : j + 1);
                    yield(out);
                }
            }
        });
    }

    auto cycles(size_t k, size_t n) -> Stream<std::vector<size_t>> {
        return Stream<std::vector<size_t>>([k, n](Sink<std::vector<size_t>> &yield) {
            for (size_t i = 0; i < n - k + 1; ++i) {
                for (const auto &c : tuples(k - 1, n - i - 1)) {
                    std::vector<size_t> out(1, i);
                    for (auto j : c) out.push_back(i + j + 1);
                    yield(out);
                }
            }
        });
    }
} // namespace vb
