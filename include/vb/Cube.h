#pragma once
#include <vb/Bitmap.h>

namespace vb {
    struct Adder {
        explicit Adder(int _s = 0) : s(_s) {}
        void     dim(int _n) { n = _n; }
        int      operator+=(int t) { return (s += t); }
        int      operator-=(int t) { return (s -= t); }
        explicit operator Color() { return Grey(s / n); }

        int s, n = 1;
    };

    struct Cube_iterator;

    class Cube : public Bitmap<Adder> {
    public:
        Cube(const Hub &H, coo3 sz);

        int64_t index(coo3 c) { return c.x + size.x * c.y + size.x * size.y * c.z; }
        coo3    wrap(coo3 c) { return {pmod(c.x, size.x), pmod(c.y, size.y), pmod(c.z, size.z)}; }

        uint8_t &at(const coo3 &c) { return data[index(c)]; }
        uint8_t &atp(const coo3 &c) { return at(wrap(c)); }
        void     put(const coo3 &c, uint8_t t);
        void     putp(const coo3 &c, uint8_t t) { put(wrap(c), t); }

        void output_pov(const Hub &H, std::string s = "");

        using iterator = Cube_iterator;
        iterator begin();
        iterator end();

        coo3                 size;
        std::vector<uint8_t> data;
        bool                 mirrors = true;
    };

    struct Cube_iterator : public coo3 {
        Cube *c;
        Cube_iterator(Cube *cc, coo3 xyz) : coo3(xyz), c(cc) {}
        bool operator!=(const Cube_iterator &o) { return (c != o.c) || coo3::operator!=(o); }
        void operator++() {
            x++;
            if (x == c->size.x) {
                x = 0;
                y++;
            }
            if (y == c->size.y) {
                y = 0;
                z++;
            }
        }
        const uint8_t &operator*() { return c->at(*this); }
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Cube") {
        char *argv[] = {(char *)"test_cube"};
        Hub   H("Testing Cube", 1, argv);
        Cube  C(H, {100, 100, 100});
        C.putp(prng.uniform_coo3(C.size), 1);
        int s = 0;
        for (auto v : C) s += v;
        CHECK(s == 1);
        CHECK(Color(C.Bitmap<Adder>::at({0, 0})).a == 255);
        C.output_pov(H);
    }
#endif
} // namespace vb
