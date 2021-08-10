#include <algorithm>
#include <vb/math/Permutation.h>
#include <vb/util/Stream.h>

namespace vb {
    Permutation::Permutation(Cycles &c) {
        size_t sz = 0;
        for (const auto &v : c) sz += v.size();
        resize(sz);
        for (auto v : c) {
            for (unsigned i = 0; i + 1 < v.size(); ++i) at(v[i]) = v[i + 1];
            at(v.back()) = v[0];
        }
    }

    auto Permutation::cycles() const -> Cycles {
        Cycles           c;
        std::vector<int> done(size(), 0);
        for (size_t i = 0; i < size(); ++i) {
            if (done[i] != 0) continue;
            std::vector<size_t> v(1, i);
            done[i] = 1;
            for (auto j = at(i); done[j] == 0; j = at(j)) {
                v.push_back(j);
                done[j] = 1;
            }
            c.push_back(v);
        }
        return c;
    }

    auto Permutation::signature() const -> std::vector<size_t> {
        vector<size_t> output;
        for (const auto &c : cycles()) output.push_back(c.size());
        sort(output.begin(), output.end());
        return output;
    }

    auto Permutation::passport() const -> Passport {
        auto     s = signature();
        Passport out;
        size_t   l = 0, c = 0;
        if (!s.empty())
            for (auto i = s.size() - 1; i <= s.size(); --i) {
                if (s[i] == l)
                    ++c;
                else {
                    if (c > 0) out.push_back({l, c});
                    l = s[i];
                    c = 1;
                }
            }
        out.push_back({l, c});
        return out;
    }

    auto Permutation::is_identity() const -> bool {
        for (size_t i = 0; i < size(); ++i)
            if (at(i) != i) return false;
        return true;
    }

    auto Permutation::inverse() const -> Permutation {
        Permutation s;
        s.resize(size());
        for (size_t i = 0; i < size(); ++i) s[at(i)] = i;
        return s;
    }

    auto Permutation::operator*(const Permutation &o) const -> Permutation {
        Permutation s;
        s.resize(size());
        for (size_t i = 0; i < size(); ++i) s[i] = o[at(i)];
        return s;
    }

    auto Permutation::conjugate(const Permutation &s) const -> Permutation {
        Permutation out(size());
        for (size_t i = 0; i < size(); ++i) out[s[i]] = s[at(i)];
        return out;
    }

    auto Transposition(size_t n, size_t i, size_t j) -> Permutation {
        Permutation p(n);
        p[i] = j;
        p[j] = i;
        return p;
    }

    auto connected(const Permutation &s, const Permutation &a) -> bool {
        auto                n = s.size();
        std::vector<size_t> l(n);
        for (size_t i = 0; i < n; ++i) l[i] = i;
        bool dirty = true;
        while (dirty) {
            dirty = false;
            for (size_t i = 0; i < n; ++i)
                if (l[s[i]] < l[i]) {
                    l[i]  = l[s[i]];
                    dirty = true;
                }
            for (size_t i = 0; i < n; ++i)
                if (l[a[i]] < l[i]) {
                    l[i]  = l[a[i]];
                    dirty = true;
                }
        }
        return std::all_of(begin(l), end(l), [](auto v) { return v == 0; });
    }

    auto permutations(size_t n) -> Stream<Permutation> {
        Permutation p(n);
        do co_yield(Permutation(p));
        while (next_permutation(p.begin(), p.end()));
    }

    auto permutations(const std::vector<size_t> &s) -> Stream<Permutation> {
        size_t n = 0;
        for (auto i : s) n += i;
        if (n == 0) {
            co_yield(Permutation{{}});
        } else {
            size_t L = 0;
            for (size_t i = 0; i < s.size(); ++i)
                if (s[i] > L) {
                    L       = s[i];
                    auto ns = s;
                    ns[i]   = 0;
                    for (const auto &c : tuples(L - 1, n - 1)) {
                        std::vector<size_t> cc({0});
                        for (auto i : c) cc.push_back(i + 1);
                        std::vector<size_t> missed(n);
                        for (size_t i = 0; i < n; ++i) missed[i] = i;
                        for (auto i : cc) missed[i] = 0;
                        for (size_t i = 0, j = 0; j < n; ++j)
                            if (missed[j] != 0) missed[i++] = missed[j];
                        for (const auto &p : permutations(ns)) {
                            auto out = p.cycles();
                            for (auto &c : out)
                                for (auto &i : c) i = missed[i];
                            out.push_back(cc);
                            co_yield(out);
                        }
                    }
                }
        }
    }
} // namespace vb
