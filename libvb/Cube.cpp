#include <fmt/ostream.h>
#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
    Cube::Cube(const std::string &s, coo3 sz)
        // TODO: ucoo3
        : Bitmap<Adder>(s, {size_t(sz.x + sz.z), size_t(sz.y + sz.z)}), size(sz), data(size_t(size.x * size.y * size.z), 0) {
        for (int64_t x = 0; x < size.z; ++x)
            for (int64_t y = 0; y < size.z; ++y)
                Bitmap<Adder>::at(ucoo(coo{size.x + x, size.y + y})) = Adder(((x / 10 + y / 10) % 2) != 0 ? 200 : 150);
        for (int64_t x = 0; x < size.x; ++x)
            for (int64_t y = 0; y < size.y; ++y) Bitmap<Adder>::at(ucoo(coo{x, y})).dim(size_t(size.z));
        for (int64_t x = 0; x < size.x; ++x)
            for (int64_t z = 0; z < size.z; ++z) Bitmap<Adder>::at(ucoo(coo{x, z + size.y})).dim(size_t(size.y));
        for (int64_t y = 0; y < size.y; ++y)
            for (int64_t z = 0; z < size.z; ++z) Bitmap<Adder>::at(ucoo(coo{z + size.x, y})).dim(size_t(size.x));
    }

    void Cube::put(const coo3 &c, uint8_t t) {
        uint8_t d = data[index(c)];
        if (t != d) {
            Bitmap<Adder>::at(ucoo(coo{c.x, c.y})) -= d;
            Bitmap<Adder>::at(ucoo(coo{c.x, c.z + size.y})) -= d;
            Bitmap<Adder>::at(ucoo(coo{c.z + size.x, c.y})) -= d;
            Bitmap<Adder>::at(ucoo(coo{c.x, c.y})) += t;
            Bitmap<Adder>::at(ucoo(coo{c.x, c.z + size.y})) += t;
            Bitmap<Adder>::at(ucoo(coo{c.z + size.x, c.y})) += t;
            data[index(c)] = t;
        }
        step();
    }

    Cube_iterator Cube::begin() { return Cube_iterator(this, {0, 0, 0}); }
    Cube_iterator Cube::end() { return Cube_iterator(this, {0, 0, size.z}); }

    void Cube::output_pov(const std::string &s) {
        Pov_Union squares1, squares2, cube, corner, ground;

        for (int64_t x = 0; x < size.x; ++x)
            for (int64_t y = 0; y < size.y; ++y)
                for (int64_t z = 0; z < size.z; ++z)
                    if (at({x, y, z}) == 255)
                        squares1 << Box({x - .5 * size.x, y - .5 * size.y, z - .5 * size.z},
                                        {double(x) + 1 - .5 * size.x, double(y) + 1 - .5 * size.y, double(z) + 1 - .5 * size.z});
                    else if (at({x, y, z}) == 255 / 2)
                        squares2 << Box({x - .5 * size.x, y - .5 * size.y, z - .5 * size.z},
                                        {double(x) + 1 - .5 * size.x, double(y) + 1 - .5 * size.y, double(z) + 1 - .5 * size.z});

        squares1 << Texture("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");
        squares2 << Texture("pigment { color rgb <.3,.5,.8> transmit .8 } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

        cube << fmt::format("{}", (bunch)Frame(tri{-.5 * size.x, -.5 * size.y, -.5 * size.z}, tri{.5 * size.x, .5 * size.y, .5 * size.z},
                                               "pigment { color rgb <.8,0,0> }"))
             << fmt::format("{}", (bunch)squares1) << fmt::format("{}", (bunch)squares2) << "rotate 360*clock*y";

        Pov_Scene SS;
        SS << Camera({1.3 * size.x, .9 * size.y, -1.5 * size.z}, {0, 0, 0}, 60) << Light_Source({50. * size.x, 30. * size.y, -15. * size.z})
           << fmt::format("{}", (bunch)cube);

        if (mirrors) {
            corner << Plane({1, 0, 0}, -.75 * size.x) << Plane({0, 0, 1}, .75 * size.z)
                   << Texture("pigment { color White } finish { reflection {.5} ambient 0 diffuse 0 }");
            ground << Plane({0, 1, 0}, -.75 * size.y)
                   << Texture("pigment { color White } finish { reflection {.1} ambient 0.2 diffuse 0.1 }");
            SS << fmt::format("{}", (bunch)corner) << fmt::format("{}", (bunch)ground);
        }
        SS.output_pov(s);
    }
} // namespace vb
