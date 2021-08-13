#pragma once
#include <vb/Bitmap.h>

namespace vb {
    struct Adder {
        explicit Adder(size_t s = 0) : s(s) {}
        void     dim(size_t _n) { n = _n; }
        auto     operator+=(size_t t) -> size_t { return (s += t); }
        auto     operator-=(size_t t) -> size_t { return (s -= t); }
        explicit operator Color() const { return Grey(uint8_t(s / n)); }

        size_t s, n = 1;
    };

    struct Cube_iterator;

    class Cube : public Bitmap<Adder> {
    public:
        Cube(const std::string &s, ucoo3 sz);

        auto index(ucoo3 c) const -> size_t { return c.x + size.x * c.y + size.x * size.y * c.z; }
        auto wrap(coo3 c) const -> ucoo3 { return {pmod(c.x, size.x), pmod(c.y, size.y), pmod(c.z, size.z)}; }

        auto at(const ucoo3 &c) -> uint8_t & { return data[index(c)]; }
        auto atp(const coo3 &c) -> uint8_t & { return at(wrap(c)); }
        void put(const ucoo3 &c, uint8_t t);
        void putp(const coo3 &c, uint8_t t) { put(wrap(c), t); }

        void output_pov(const std::string &s);

        using iterator = Cube_iterator;
        auto begin() -> iterator;
        auto end() -> iterator;

        ucoo3                size;
        std::vector<uint8_t> data;
        bool                 mirrors = true;
    };

    struct Cube_iterator : public ucoo3 {
        Cube *c;
        Cube_iterator(Cube *cc, ucoo3 xyz) : ucoo3(xyz), c(cc) {}
        auto operator!=(const Cube_iterator &o) -> bool { return (c != o.c) || ucoo3::operator!=(o); }
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
        auto operator*() -> const uint8_t & { return c->at(*this); }
    };
} // namespace vb
