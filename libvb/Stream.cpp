#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    Stream<std::vector<size_t>> partitions(size_t n, size_t m) {
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

    Stream<std::vector<size_t>> tuples(size_t k, size_t n) {
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

    Stream<std::vector<size_t>> cycles(size_t k, size_t n) {
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
