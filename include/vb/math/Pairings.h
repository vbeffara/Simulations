#pragma once
#include <vb/math/Permutation.h>

namespace vb {
    class Pairings_Iterator {
    public:
        Pairings_Iterator(size_t n, size_t i, bool d);

        bool         operator!=(const Pairings_Iterator &o) const;
        void         operator++();
        Permutation &operator*();

    private:
        void next();

        std::vector<std::vector<size_t>> todo_c, todo_p;
        Permutation                      current;
        size_t                           n, i;
    };

    class Pairings {
    public:
        Pairings(size_t n);

        size_t size() const;

        Pairings_Iterator begin() const;
        Pairings_Iterator end() const;

        Permutation rrand();

    private:
        size_t n;
    };
} // namespace vb
