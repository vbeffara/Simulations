#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
    class Adder {
    public:
        int s;
        int n = 1;
        explicit Adder(int _s = 0) : s(_s) {}
        void dim(int _n) { n = _n; }
        int  operator+=(int t) {
            s += t;
            return s;
        }
        int operator-=(int t) {
            s -= t;
            return s;
        }
        explicit operator Color() { return Grey(s / n); }
    };

    class Cube_iterator;

    class Cube : public Bitmap<Adder> {
    public:
        Cube(int x, int y, int z);

        int  index(coo3 c) { return c.x + sx * c.y + sx * sy * c.z; }
        coo3 rand(int b = 0) {
            return {b + prng.uniform_int(sx - 2 * b), b + prng.uniform_int(sy - 2 * b), b + prng.uniform_int(sz - 2 * b)};
        }
        coo3 wrap(coo3 c) {
            int xx = ((c.x % sx) + sx) % sx, yy = ((c.y % sy) + sy) % sy, zz = ((c.z % sz) + sz) % sz;
            return {xx, yy, zz};
        }

        void putp(const coo3 & c, uint8_t t) { put(wrap(c), t); }
        bool done(const coo3 & c) { return (c.z == sz); }

        uint8_t & at(const coo3 & c) { return data[index(c)]; }
        uint8_t & atp(const coo3 & c) { return at(wrap(c)); }

        using iterator = Cube_iterator;
        iterator begin();
        iterator end();

        void put(const coo3 & c, uint8_t t) {
            uint8_t d = data[index(c)];
            if (t != d) {
                Bitmap<Adder>::at({c.x, c.y}) -= d;
                Bitmap<Adder>::at({c.x, c.z + sy}) -= d;
                Bitmap<Adder>::at({c.z + sx, c.y}) -= d;
                Bitmap<Adder>::at({c.x, c.y}) += t;
                Bitmap<Adder>::at({c.x, c.z + sy}) += t;
                Bitmap<Adder>::at({c.z + sx, c.y}) += t;
                data[index(c)] = t;
            }
            step();
        }

        void output_pov(std::string s = "");

        int                  sx, sy, sz;
        std::vector<uint8_t> data;
        bool                 mirrors = true;
    };

    class Cube_iterator : public coo3 {
    public:
        Cube * c;
        Cube_iterator(Cube * cc, coo3 xyz) : coo3(xyz), c(cc) {}
        bool operator!=(const Cube_iterator & o) { return (c != o.c) || coo3::operator!=(o); }
        void operator++() {
            x++;
            if (x == c->sx) {
                x = 0;
                y++;
            }
            if (y == c->sy) {
                y = 0;
                z++;
            }
        }
        const uint8_t & operator*() { return c->at(*this); }
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Cube") {
        Cube C(100, 100, 100);
        C.putp(C.rand(), 1);
        int s = 0;
        for (auto v : C) s += v;
        CHECK(s == 1);
        CHECK(Color(C.Bitmap<Adder>::at({0, 0})).a == 255);
        C.output_pov();
    }
#endif
} // namespace vb
