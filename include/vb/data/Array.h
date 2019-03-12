#pragma once
#include <vb/math/math.h>
#include <vb/util/coo.h>

namespace vb {
    template <typename T> class Array {
    public:
        Array(coo sz, T d) : size(sz), data((unsigned long)(sz.x * sz.y), d) {}
        Array(coo sz) : size(sz), data((unsigned long)(sz.x * sz.y)) {}

        explicit Array(const std::vector<std::vector<T>> &l)
            : size({static_cast<int64_t>(l.size()), static_cast<int64_t>(l[0].size())}), data(size.x * size.y) {
            for (const auto &z : coo_range(size)) put(z, l[z.x][z.y]);
        }

        void resize(coo sz) {
            size = sz;
            data.resize(size.x * size.y);
        }
        void resize(coo sz, T t) {
            size = sz;
            data.resize(size.x * size.y, t);
        }

        size_t index(const coo &z) const { return size_t(z.x + size.x * z.y); }

        T &      at(const coo &z) { return data[index(z)]; }
        T const &at(const coo &z) const { return data[index(z)]; }

        T &      operator[](const coo &z) { return at(z); }
        T const &operator[](const coo &z) const { return at(z); }

        T &atp(const coo &z) {
            int64_t x = pmod(z.x, size.x), y = pmod(z.y, size.y);
            return at({x, y});
        }
        T const &atp(const coo &z) const {
            int64_t x = pmod(z.x, size.x), y = pmod(z.y, size.y);
            return at({x, y});
        }

        void put(const coo &z, T const &c) { at(z) = c; }
        void putp(const coo &z, T const &c) { atp(z) = c; }

        bool contains(const coo &z, int64_t b = 0) const { return (z.x >= b) && (z.y >= b) && (z.x < size.x - b) && (z.y < size.y - b); }

        coo size;

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
