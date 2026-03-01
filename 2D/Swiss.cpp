#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
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

    explicit World(const std::string &title, size_t n, size_t cc, double pp, double qq) : Bitmap<uint8_t>(title, {n, n}), c(cc), p(pp), q(qq) {
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
            for (int const ii : nb)
                if (ii > max) max = ii;

            auto d = vb::prng.uniform_int(uint8_t(4));
            if (vb::prng.bernoulli(1 - q))
                while (nb[d] < max) d = vb::prng.uniform_int(uint8_t(4));

            putp(z, d);
            z += vb::dz[d];
        }
    }
};

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "The Swiss Journalist");
    auto n = clp.param("n", size_t(600), "Grid size");
    auto c = clp.param("c", size_t(0), "Center region size");
    auto p = clp.param("p", 0.8, "Initial randomness probability");
    auto q = clp.param("q", 0.35, "Random walk probability");
    clp.finalize();

    World w(clp.title, n, c, p, q);
    w.show();
    w.run();
}
