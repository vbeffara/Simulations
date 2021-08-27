#include <vb/math/Pairings.h>
#include <vb/math/math.h>
#include <vb/util/PRNG.h>

namespace vb {
    void Pairings_Iterator::next() {
        while (!todo_c.empty()) {
            auto c = todo_c.back();
            todo_c.pop_back();
            auto p = todo_p.back();
            todo_p.pop_back();
            if (p.empty()) {
                for (size_t ii = 0; ii < n / 2; ++ii) {
                    current[c[2 * ii]]     = c[2 * ii + 1];
                    current[c[2 * ii + 1]] = c[2 * ii];
                }
                return;
            }
            auto ii = p[0];
            for (size_t k = 1; k < p.size(); ++k) {
                auto o  = p[k];
                auto cc = c;
                cc.push_back(ii);
                cc.push_back(o);
                std::vector<size_t> pp;
                for (auto j : p)
                    if ((j != ii) && (j != o)) pp.push_back(j);
                todo_c.push_back(cc);
                todo_p.push_back(pp);
            }
        }
    }

    Pairings_Iterator::Pairings_Iterator(size_t n_, size_t i_, bool d) : current(n_), n(n_), i(i_) {
        if (d) {
            std::vector<size_t> all;
            all.reserve(n);
            for (size_t ii = 0; ii < n; ++ii) all.push_back(ii);
            std::vector<size_t> c;
            todo_c.push_back(c);
            todo_p.push_back(all);
            next();
        }
    }

    void Pairings_Iterator::operator++() {
        ++i;
        next();
    }

    auto Pairings_Iterator::operator!=(const Pairings_Iterator &o) const -> bool { return (n != o.n) || (i != o.i); }

    auto Pairings_Iterator::operator*() -> Permutation & { return current; }

    Pairings::Pairings(size_t n_) : n(n_) {}

    auto Pairings::size() const -> size_t {
        size_t nn = n;
        return fact(nn) / fact(nn / 2) / (size_t(1) << (n / 2));
    }

    auto Pairings::begin() const -> Pairings_Iterator { return Pairings_Iterator(n, 0, true); }

    auto Pairings::end() const -> Pairings_Iterator { return Pairings_Iterator(n, size(), false); }

    auto Pairings::rrand() const -> Permutation {
        Permutation out(n);
        for (size_t i = 0; i < n; ++i) {
            if (out[i] < i) continue;
            while (true) {
                out[i] = i + 1 + prng.uniform_int(n - i - 1);
                if (out[out[i]] == out[i]) break;
            }
            out[out[i]] = i;
        }
        return out;
    }
} // namespace vb
