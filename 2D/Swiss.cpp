#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

constexpr unsigned EAST  = 0;
constexpr unsigned NORTH = 1;
constexpr unsigned WEST  = 2;
constexpr unsigned SOUTH = 3;

namespace vb {
    constexpr Color C_EAST{0, 255, 0};
    constexpr Color C_NORTH{255, 0, 0};
    constexpr Color C_WEST{0, 0, 255};
    constexpr Color C_SOUTH{255, 255, 0};

    template <> auto to_Color(uint8_t t) -> Color {
        static const std::vector<Color> colors{C_EAST, C_NORTH, C_WEST, C_SOUTH};
        return colors[t];
    }
} // namespace vb

using namespace vb;

class World : public vb::Bitmap<uint8_t> {
public:
    size_t c;
    double p, q;

    explicit World(const vb::Hub &H) : Bitmap<uint8_t>(H.title, {H['n'], H['n']}), c(H['c']), p(H['p']), q(H['q']) {
        auto mid = (size.x + size.y) / 2;
        for (const auto &z : coo_range(size)) {
            if (z.y > z.x) {
                if (z.x + z.y < mid)
                    at(z) = EAST;
                else
                    at(z) = SOUTH;
            } else {
                if (z.x + z.y < mid)
                    at(z) = NORTH;
                else
                    at(z) = WEST;
            }
            if (vb::prng.bernoulli(p)) at(z) = vb::prng.uniform_int(uint8_t(4));
        }

        for (const auto &z : coo_range(ucoo{c, c}, size - ucoo{c, c})) at(z) = vb::prng.uniform_int(uint8_t(4));
    }

    void run() {
        for (vb::coo z = vb::coo(size / 2);;) {
            std::vector<int> nb(4, 0);
            nb[atp(z + vb::coo{1, 0})] += 1;
            nb[atp(z - vb::coo{1, 0})] += 1;
            nb[atp(z + vb::coo{0, 1})] += 1;
            nb[atp(z - vb::coo{0, 1})] += 1;
            int max = 0;
            for (int i : nb)
                if (i > max) max = i;

            auto d = vb::prng.uniform_int(uint8_t(4));
            if (vb::prng.bernoulli(1 - q))
                while (nb[d] < max) d = vb::prng.uniform_int(uint8_t(4));

            putp(z, d);
            z += vb::dz[d];
        }
    }
};

auto main(int argc, char **argv) -> int {
    vb::Hub H("The Swiss Journalist", argc, argv, "n=600,c=0,p=.8,q=.35");
    World   w(H);
    w.show();
    w.run();
}
