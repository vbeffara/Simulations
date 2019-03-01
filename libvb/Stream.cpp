#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    Stream<std::vector<int>> partitions(int n, int m) {
        return Stream<std::vector<int>>([n, m](Sink<std::vector<int>> &yield) {
            for (int i = m; i < n; ++i) {
                for (const auto &p : partitions(n - i, i)) {
                    std::vector<int> out({i});
                    for (auto j : p) out.push_back(j);
                    yield(out);
                }
            }
            if (n >= m) yield({n});
        });
    }

    Stream<std::vector<int>> tuples(int k, int n) {
        return Stream<std::vector<int>>([k, n](Sink<std::vector<int>> &yield) {
            if (k == 0) {
                yield({});
                return;
            }
            for (int i = 0; i < n; ++i) {
                for (const auto &c : tuples(k - 1, n - 1)) {
                    std::vector<int> out({i});
                    for (auto j : c) out.push_back(j < i ? j : j + 1);
                    yield(out);
                }
            }
        });
    }

    Stream<std::vector<int>> cycles(int k, int n) {
        return Stream<std::vector<int>>([k, n](Sink<std::vector<int>> &yield) {
            for (int i = 0; i < n - k + 1; ++i) {
                for (const auto &c : tuples(k - 1, n - i - 1)) {
                    std::vector<int> out(1, i);
                    for (auto j : c) out.push_back(i + j + 1);
                    yield(out);
                }
            }
        });
    }
} // namespace vb
