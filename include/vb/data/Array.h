#pragma once
#include <tbb/parallel_for.h>
#include <vb/util/PRNG.h>

namespace vb {
    template <typename T> class Array {
    public:
        Array(ucoo sz, T d) : size(sz), data(sz.x * sz.y, d) {}
        explicit Array(ucoo sz) : size(sz), data(sz.x * sz.y) {}

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

        [[nodiscard]] auto index(const ucoo &z) const -> size_t { return z.x + size.x * z.y; }

        auto at(const ucoo &z) -> T & { return data[index(z)]; }
        auto at(const ucoo &z) const -> T const & { return data[index(z)]; }

        auto operator[](const ucoo &z) -> T & { return at(z); }
        auto operator[](const ucoo &z) const -> T const & { return at(z); }

        auto atp(const coo &z) -> T & { return at(wrap(z, size)); }
        auto atp(const coo &z) const -> T const & { return at(wrap(z, size)); }

        void put(const ucoo &z, T const &c) { at(z) = c; }
        void putp(const coo &z, T const &c) { atp(z) = c; }

        template <typename U> auto fits(const coo_2d<U> &z, size_t b = 0) const -> bool {
            return (z.x >= U(b)) && (z.y >= U(b)) && (z.x < U(size.x - b)) && (z.y < U(size.y - b));
        }

        template <typename F> void parallel_do (F &&f) {
            tbb::parallel_for(0UL, size.y, [sx = size.x, &f](size_t y) {
                for (size_t x = 0; x < sx; ++x) f(ucoo(x, y));
            });
        }

        ucoo size;

    private:
        std::vector<T> data;
    };
} // namespace vb
