#include <fmt/ostream.h>
#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
    Cube::Cube(const std::string &s, coo3 sz) : Bitmap<Adder>(s, {sz.x + sz.z, sz.y + sz.z}), size(sz), data(size.x * size.y * size.z, 0) {
        for (int x = 0; x < size.z; ++x)
            for (int y = 0; y < size.z; ++y) Bitmap<Adder>::at({size.x + x, size.y + y}) = Adder(((x / 10 + y / 10) % 2) != 0 ? 200 : 150);
        for (int x = 0; x < size.x; ++x)
            for (int y = 0; y < size.y; ++y) Bitmap<Adder>::at({x, y}).dim(size.z);
        for (int x = 0; x < size.x; ++x)
            for (int z = 0; z < size.z; ++z) Bitmap<Adder>::at({x, z + size.y}).dim(size.y);
        for (int y = 0; y < size.y; ++y)
            for (int z = 0; z < size.z; ++z) Bitmap<Adder>::at({z + size.x, y}).dim(size.x);
    }

    void Cube::put(const coo3 &c, uint8_t t) {
        uint8_t d = data[index(c)];
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

    Cube_iterator Cube::begin() { return Cube_iterator(this, {0, 0, 0}); }
    Cube_iterator Cube::end() { return Cube_iterator(this, {0, 0, size.z}); }

    void Cube::output_pov(const std::string &s) {
        Pov_Union squares1, squares2, cube, corner, ground;

        for (int x = 0; x < size.x; ++x)
            for (int y = 0; y < size.y; ++y)
                for (int z = 0; z < size.z; ++z)
                    if (at({x, y, z}) == 255)
                        squares1 << Box({x - .5 * size.x, y - .5 * size.y, z - .5 * size.z},
                                        {x + 1 - .5 * size.x, y + 1 - .5 * size.y, z + 1 - .5 * size.z});
                    else if (at({x, y, z}) == 255 / 2)
                        squares2 << Box({x - .5 * size.x, y - .5 * size.y, z - .5 * size.z},
                                        {x + 1 - .5 * size.x, y + 1 - .5 * size.y, z + 1 - .5 * size.z});

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
