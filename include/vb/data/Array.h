#pragma once
#include <vb/util/PRNG.h>

namespace vb {
    template <typename T> class Array {
    public:
        Array(ucoo sz, T d) : size(sz), data((unsigned long)(sz.x * sz.y), d) {}
        Array(ucoo sz) : size(sz), data((unsigned long)(sz.x * sz.y)) {}

        explicit Array(const std::vector<std::vector<T>> &l) : size(ucoo{l.size(), l[0].size()}), data(size.x * size.y) {
            for (auto z : coo_range(size)) put(z, l[z.x][z.y]);
        }

        void resize(ucoo sz) {
            size = sz;
            data.resize(size.x * size.y);
        }
        void resize(ucoo sz, T t) {
            size = sz;
            data.resize(size.x * size.y, t);
        }

        size_t index(const ucoo &z) const { return z.x + size.x * z.y; }

        T &      at(const ucoo &z) { return data[index(z)]; }
        T const &at(const ucoo &z) const { return data[index(z)]; }

        T &      operator[](const ucoo &z) { return at(z); }
        T const &operator[](const ucoo &z) const { return at(z); }

        T &      atp(const coo &z) { return at(wrap(z, size)); }
        T const &atp(const coo &z) const { return at(wrap(z, size)); }

        void put(const ucoo &z, T const &c) { at(z) = c; }
        void putp(const coo &z, T const &c) { atp(z) = c; }

        template <typename U> bool fits(const coo_2d<U> &z, size_t b = 0) const {
            return (z.x >= U(b)) && (z.y >= U(b)) && (z.x < U(size.x - b)) && (z.y < U(size.y - b));
        }

        ucoo size;

    private:
        std::vector<T> data;
    };
} // namespace vb
