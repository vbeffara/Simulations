#pragma once /// \file
#include <vb/PRNG.h>
#include <vb/coo.h>
#include <vb/math.h>
#include <range/v3/all.hpp>

namespace vb {
    template <typename T> class Array_iterator;

    template <typename T> class Array {
    public:
        Array(int64_t w, int64_t h, T d) : ww(w), hh(h), data(w * h, d) {}
        Array(int64_t w, int64_t h) : ww(w), hh(h), data(w * h) {}
        Array() = default;

        explicit Array(const std::vector<std::vector<T>> & l) : ww(l.size()), hh(l[0].size()), data(ww * hh) {
            for (int64_t i = 0; i < ww; ++i)
                for (int64_t j = 0; j < hh; ++j) put(coo(i, j), l[i][j]);
        }

        void resize(int64_t w, int64_t h) {
            ww = w;
            hh = h;
            data.resize(ww * hh);
        }
        void resize(int64_t w, int64_t h, T t) {
            ww = w;
            hh = h;
            data.resize(ww * hh, t);
        }

        T &       at(const coo & z) { return data[z.x + ww * z.y]; }
        T const & at(const coo & z) const { return data[z.x + ww * z.y]; }

        T &       operator[](const coo & z) { return at(z); }
        T const & operator[](const coo & z) const { return at(z); }

        T & atp(const coo & z) {
            int64_t x = pmod(z.x, ww), y = pmod(z.y, hh);
            return at(coo(x, y));
        }
        T const & atp(const coo & z) const {
            int64_t x = pmod(z.x, ww), y = pmod(z.y, hh);
            return at(coo(x, y));
        }

        void put(const coo & z, T const & c) { at(z) = c; }
        void putp(const coo & z, T const & c) { atp(z) = c; }

        bool contains(const coo & z, int64_t b = 0) const { return (z.x >= b) && (z.y >= b) && (z.x < ww - b) && (z.y < hh - b); }
        coo  rand(int64_t b = 0) const { return coo(b + prng.uniform_int(ww - 2 * b), b + prng.uniform_int(hh - 2 * b)); }

        auto coos() const {
            using ranges::view::cartesian_product;
            using ranges::view::ints;
            using ranges::view::transform;
            return cartesian_product(ints(0LL, ww), ints(0LL, hh)) | transform([](auto xy) { return std::make_from_tuple<coo>(xy); });
        }

        int64_t ww = 0, hh = 0;

    private:
        std::vector<T> data;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Array") {
        Array<int> A(23, 45, 1);
        CHECK(A.contains({6, 3}));
        CHECK(!A.contains({23, 1}));
        CHECK(!A.contains({5, 45}));

        A.put({1, 1}, 2);
        CHECK(A.at({3, 4}) == 1);
        CHECK(A.at({1, 1}) == 2);

        A.putp({5 * 23 + 4, 7 * 45 + 10}, 0);
        CHECK(A.atp({23 + 4, -45 + 10}) == 0);

        A.at(A.rand()) += 10;
        int s = 0;
        for (auto z : A.coos()) s += A[z];
        CHECK(s == 23 * 45 + 10);
    }
#endif
} // namespace vb
