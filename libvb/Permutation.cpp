#include <algorithm>
#include <vb/math/Permutation.h>

namespace vb {
    Permutation::Permutation(Cycles &c) {
        int sz = 0;
        for (const auto &v : c) sz += v.size();
        resize(sz);
        for (auto v : c) {
            for (unsigned i = 0; i + 1 < v.size(); ++i) at(v[i]) = v[i + 1];
            at(v.back()) = v[0];
        }
    }

    Cycles Permutation::cycles() const {
        Cycles           c;
        std::vector<int> done(size(), 0);
        for (unsigned i = 0; i < size(); ++i) {
            if (done[i] != 0) continue;
            std::vector<unsigned> v(1, i);
            done[i] = 1;
            for (int j = at(i); done[j] == 0; j = at(j)) {
                v.push_back(j);
                done[j] = 1;
            }
            c.push_back(v);
        }
        return c;
    }

    std::vector<int> Permutation::signature() const {
        vector<int> output;
        for (const auto &c : cycles()) output.push_back(c.size());
        sort(output.begin(), output.end());
        return output;
    }

    Passport Permutation::passport() const {
        std::vector<int> s = signature();
        Passport         out;
        int              l = 0, c = 0;
        for (int i = s.size() - 1; i >= 0; --i) {
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

    bool Permutation::is_identity() const {
        for (unsigned i = 0; i < size(); ++i)
            if (at(i) != i) return false;
        return true;
    }

    Permutation Permutation::inverse() const {
        Permutation s;
        s.resize(size());
        for (unsigned i = 0; i < size(); ++i) s[at(i)] = i;
        return s;
    }

    Permutation Permutation::operator*(const Permutation &o) const {
        Permutation s;
        s.resize(size());
        for (unsigned i = 0; i < size(); ++i) s[i] = o[at(i)];
        return s;
    }

    Permutation Permutation::conjugate(const Permutation &s) const {
        Permutation out(size());
        for (unsigned i = 0; i < size(); ++i) out[s[i]] = s[at(i)];
        return out;
    }

    Permutation Transposition(int n, int i, int j) {
        Permutation p(n);
        p[i] = j;
        p[j] = i;
        return p;
    }

    bool connected(const Permutation &s, const Permutation &a) {
        int              n = s.size();
        std::vector<int> l(n);
        for (int i = 0; i < n; ++i) l[i] = i;
        bool dirty = true;
        while (dirty) {
            dirty = false;
            for (int i = 0; i < n; ++i)
                if (l[s[i]] < l[i]) {
                    l[i]  = l[s[i]];
                    dirty = true;
                }
            for (int i = 0; i < n; ++i)
                if (l[a[i]] < l[i]) {
                    l[i]  = l[a[i]];
                    dirty = true;
                }
        }
        for (int i = 0; i < n; ++i)
            if (l[i] > 0) return false;
        return true;
    }
} // namespace vb
