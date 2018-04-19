#include <vb/Cube.h>
#include <vb/Pov.h>

namespace vb {
    Cube::Cube(int x, int y, int z) : Bitmap<Adder>(x + z, y + z), sx(x), sy(y), sz(z), data(sx * sy * sz, 0) {
        for (int x = 0; x < sz; ++x)
            for (int y = 0; y < sz; ++y) at(coo(sx + x, sy + y)) = Adder(((x / 10 + y / 10) % 2) != 0 ? 200 : 150);
        for (int x = 0; x < sx; ++x)
            for (int y = 0; y < sy; ++y) at(coo(x, y)).dim(sz);
        for (int x = 0; x < sx; ++x)
            for (int z = 0; z < sz; ++z) at(coo(x, z + sy)).dim(sy);
        for (int y = 0; y < sy; ++y)
            for (int z = 0; z < sz; ++z) at(coo(z + sx, y)).dim(sx);
    }

    Cube_iterator Cube::begin() { return Cube_iterator(this, {0, 0, 0}); }
    Cube_iterator Cube::end() { return Cube_iterator(this, {0, 0, sz}); }

    void Cube::output_pov(std::string s) {
        Pov_Union squares1, squares2, cube, corner, ground;

        for (int x = 0; x < sx; ++x)
            for (int y = 0; y < sy; ++y)
                for (int z = 0; z < sz; ++z)
                    if (at({x, y, z}) == 255)
                        squares1 << Box({x - .5 * sx, y - .5 * sy, z - .5 * sz}, {x + 1 - .5 * sx, y + 1 - .5 * sy, z + 1 - .5 * sz});
                    else if (at({x, y, z}) == 255 / 2)
                        squares2 << Box({x - .5 * sx, y - .5 * sy, z - .5 * sz}, {x + 1 - .5 * sx, y + 1 - .5 * sy, z + 1 - .5 * sz});

        squares1 << Texture("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");
        squares2 << Texture("pigment { color rgb <.3,.5,.8> transmit .8 } normal { bumps .1 scale .1 } finish { "
                            "reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");

        cube << Frame(tri{-.5 * sx, -.5 * sy, -.5 * sz}, tri{.5 * sx, .5 * sy, .5 * sz}, "pigment { color rgb <.8,0,0> }") << squares1
             << squares2 << "rotate 360*clock*y";

        Pov_Scene SS;
        SS << Camera({1.3 * sx, .9 * sy, -1.5 * sz}, {0, 0, 0}, 60) << Light_Source({50. * sx, 30. * sy, -15. * sz}) << cube;

        if (mirrors) {
            corner << Plane({1, 0, 0}, -.75 * sx) << Plane({0, 0, 1}, .75 * sz)
                   << Texture("pigment { color White } finish { reflection {.5} ambient 0 diffuse 0 }");
            ground << Plane({0, 1, 0}, -.75 * sy) << Texture("pigment { color White } finish { reflection {.1} ambient 0.2 diffuse 0.1 }");
            SS << corner << ground;
        }
        SS.output_pov(s.empty() ? H.title : s);
    }
} // namespace vb
