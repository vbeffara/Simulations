#pragma once
#include <vb/util/PRNG.h>

namespace vb {
    template <typename T> class Array {
    public:
        Array(ucoo sz, T d) : size(sz), data((unsigned long)(sz.x * sz.y), d) {}
        Array(ucoo sz) : size(sz), data((unsigned long)(sz.x * sz.y)) {}

        explicit Array(const std::vector<std::vector<T>> &l) : size(ucoo{l.size(), l[0].size()}), data(size.x * size.y) {
            for (const ucoo z : coo_range(size)) put(z, l[z.x][z.y]);
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

        bool contains(const coo &z, int64_t b = 0) const {
            return (z.x >= b) && (z.y >= b) && (z.x < int64_t(size.x) - b) && (z.y < int64_t(size.y) - b);
        }

        ucoo size;

    private:
        std::vector<T> data;
    };
} // namespace vb

#ifdef UNIT_TESTS
namespace vb {
    TEST_CASE("vb::Array") {
        Array<int> A({23, 45}, 1);
        CHECK(A.contains({6, 3}));
        CHECK(!A.contains({23, 1}));
        CHECK(!A.contains({5, 45}));

        A.put({1, 1}, 2);
        CHECK(A.at({3, 4}) == 1);
        CHECK(A.at({1, 1}) == 2);

        A.putp({5 * 23 + 4, 7 * 45 + 10}, 0);
        CHECK(A.atp({23 + 4, -45 + 10}) == 0);

        A.at(prng.uniform_coo(A.size)) += 10;
        int s = 0;
        for (auto z : coo_range(A.size)) s += A[z];
        CHECK(s == 23 * 45 + 10);
    }
} // namespace vb
#endif
