#pragma once
#include <vb/math/Permutation.h>

namespace vb {
    class Pairings_Iterator {
    public:
        Pairings_Iterator(size_t n_, size_t i_, bool d);

        auto operator!=(const Pairings_Iterator &o) const -> bool;
        void operator++();
        auto operator*() -> Permutation &;

    private:
        void next();

        std::vector<std::vector<size_t>> todo_c, todo_p;
        Permutation                      current;
        size_t                           n, i;
    };

    class Pairings {
    public:
        explicit Pairings(size_t n_);

        [[nodiscard]] auto size() const -> size_t;

        [[nodiscard]] auto begin() const -> Pairings_Iterator;
        [[nodiscard]] auto end() const -> Pairings_Iterator;

        [[nodiscard]] auto rrand() const -> Permutation;

    private:
        size_t n;
    };
} // namespace vb
