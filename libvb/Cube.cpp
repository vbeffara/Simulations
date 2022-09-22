#include <fmt/ostream.h>
#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
    Cube::Cube(const std::string &s, ucoo3 sz) : Bitmap<Adder>(s, {sz.x + sz.z, sz.y + sz.z}), size(sz), data(size.x * size.y * size.z, 0) {
        for (const auto &z : coo_range(ucoo{size.z, size.z}))
            Bitmap<Adder>::at({size.x + z.x, size.y + z.y}) = Adder(((z.x / 10 + z.y / 10) % 2) != 0 ? 200 : 150);
        for (const auto &z : coo_range(ucoo{size.x, size.y})) Bitmap<Adder>::at(z).dim(size.z);
        for (const auto &z : coo_range(ucoo{size.x, size.z})) Bitmap<Adder>::at({z.x, z.y + size.y}).dim(size.y);
        for (const auto &z : coo_range(ucoo{size.y, size.z})) Bitmap<Adder>::at({z.x + size.x, z.y}).dim(size.x);
    }

    void Cube::put(const ucoo3 &c, uint8_t t) {
        uint8_t const d = data[index(c)];
        if (t != d) {
            Bitmap<Adder>::at({c.x, c.y}) -= d;
            Bitmap<Adder>::at({c.x, c.z + size.y}) -= d;
            Bitmap<Adder>::at({c.z + size.x, c.y}) -= d;
            Bitmap<Adder>::at({c.x, c.y}) += t;
            Bitmap<Adder>::at({c.x, c.z + size.y}) += t;
            Bitmap<Adder>::at({c.z + size.x, c.y}) += t;
            data[index(c)] = t;
        }
        step();
    }

    auto Cube::begin() -> Cube_iterator { return Cube_iterator(this, {0, 0, 0}); }
    auto Cube::end() -> Cube_iterator { return Cube_iterator(this, {0, 0, size.z}); }

    void Cube::output_pov(const std::string &s) {
        Pov_Union squares1, squares2, cube, corner, ground;

        for (size_t x = 0; x < size.x; ++x)
            for (size_t y = 0; y < size.y; ++y)
                for (size_t z = 0; z < size.z; ++z)
                    if (at({x, y, z}) == 255)
                        squares1 << Box({double(x) - .5 * double(size.x), double(y) - .5 * double(size.y), double(z) - .5 * double(size.z)},
                                        {double(x) + 1 - .5 * double(size.x), double(y) + 1 - .5 * double(size.y),
                                         double(z) + 1 - .5 * double(size.z)});
                    else if (at({x, y, z}) == 255 / 2)
                        squares2 << Box({double(x) - .5 * double(size.x), double(y) - .5 * double(size.y), double(z) - .5 * double(size.z)},
                                        {double(x) + 1 - .5 * double(size.x), double(y) + 1 - .5 * double(size.y),
                                         double(z) + 1 - .5 * double(size.z)});

        squares1 << Texture("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");
        squares2 << Texture("pigment { color rgb <.3,.5,.8> transmit .8 } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

        cube << fmt::format("{}", static_cast<bunch>(Frame(tri{-.5 * double(size.x), -.5 * double(size.y), -.5 * double(size.z)},
                                                           tri{.5 * double(size.x), .5 * double(size.y), .5 * double(size.z)},
                                                           "pigment { color rgb <.8,0,0> }")))
             << fmt::format("{}", static_cast<bunch>(squares1)) << fmt::format("{}", static_cast<bunch>(squares2)) << "rotate 360*clock*y";

        Pov_Scene SS;
        SS << Camera({1.3 * double(size.x), .9 * double(size.y), -1.5 * double(size.z)}, {0, 0, 0}, 60)
           << Light_Source({50. * double(size.x), 30. * double(size.y), -15. * double(size.z)})
           << fmt::format("{}", static_cast<bunch>(cube));

        if (mirrors) {
            corner << Plane({1, 0, 0}, -.75 * double(size.x)) << Plane({0, 0, 1}, .75 * double(size.z))
                   << Texture("pigment { color White } finish { reflection {.5} ambient 0 diffuse 0 }");
            ground << Plane({0, 1, 0}, -.75 * double(size.y))
                   << Texture("pigment { color White } finish { reflection {.1} ambient 0.2 diffuse 0.1 }");
            SS << fmt::format("{}", static_cast<bunch>(corner)) << fmt::format("{}", static_cast<bunch>(ground));
        }
        SS.output_pov(s);
    }
} // namespace vb
