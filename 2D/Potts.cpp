#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> auto to_Color(unsigned t) -> Color {
        static const vector<Color> C{RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN};
        if (t < 6) return C[size_t(t)];
        return Indexed(int(t));
    }
} // namespace vb

class Potts : public Bitmap<unsigned> {
public:
    Potts(const Hub &H, size_t n, unsigned q_, double beta_) : Bitmap<unsigned>(H.title, {n, n}), q(q_), beta(beta_) {
        bcs["perio"] = [] {};
        bcs["free"]  = [this] {
            b = 1;
            for (auto z : coo_range(size)) put(z, unsigned(-1));
        };
        bcs["wired"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii) {
                put({ii, 0}, 1);
                put({ii, size.y - 1}, 1);
            }
            for (size_t ii = 0; ii < size.y; ++ii) {
                put({0, ii}, 1);
                put({size.x - 1, ii}, 1);
            }
        };
        bcs["tripod"] = [this] {
            b = 1;
            for (size_t j = 0; j < size.y - size.x / 2; ++j) {
                for (size_t ii = 0; ii < size.x / 2; ++ii) put({ii, j}, 0);
                for (size_t ii = size.x / 2; ii < size.x; ++ii) put({ii, j}, 2);
            }
            for (size_t j = size.y - size.x / 2; j < size.y; ++j) {
                for (size_t ii = 0; ii < size.y - j; ++ii) put({ii, j}, 0);
                for (size_t ii = size.y - j; ii < size.x - size.y + j; ++ii) put({ii, j}, 1);
                for (size_t ii = size.x - size.y + j; ii < size.x; ++ii) put({ii, j}, 2);
            }
        };
        bcs["quadripod"] = [this] {
            b = 1;
            for (size_t j = 0; j < size.y / 2; ++j) {
                for (size_t ii = 0; ii < size.x / 2; ++ii) put({ii, j}, 0);
                for (size_t ii = size.x / 2; ii < size.x; ++ii) put({ii, j}, 1);
            }
            for (size_t j = size.y / 2; j < size.y; ++j) {
                for (size_t ii = 0; ii < size.x / 2; ++ii) put({ii, j}, 2);
                for (size_t ii = size.x / 2; ii < size.x; ++ii) put({ii, j}, 3);
            }
        };
        bcs["quadripod2"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y; ++j) {
                    if (ii > j) {
                        if (ii + j < size.x)
                            put({ii, j}, 0);
                        else
                            put({ii, j}, 1);
                    } else {
                        if (ii + j < size.x)
                            put({ii, j}, 2);
                        else
                            put({ii, j}, 3);
                    }
                }
        };
        bcs["dobrushin"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y / 2; ++j) put({ii, j}, 0);
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = size.y / 2; j < size.y; ++j) put({ii, j}, 1);
        };
        bcs["loren"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y; ++j)
                    if ((ii == 0) || (j == 0) || (ii == size.x - 1) || (j == size.y - 1))
                        put({ii, j}, unsigned((q * (ii + j) / size.x) % q));
        };
        bcs["loren2"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x / 4; ++ii) put({ii, 0}, 0);
            for (size_t ii = size.x / 4; ii < 3 * size.x / 4; ++ii) put({ii, 0}, 1);
            for (size_t ii = 3 * size.x / 4; ii < size.x; ++ii) put({ii, 0}, 2);
            for (size_t ii = 0; ii < size.y / 2; ++ii) put({0, ii}, 0);
            for (size_t ii = size.y / 2; ii < size.y; ++ii) put({0, ii}, 5);
            for (size_t ii = 0; ii < size.y / 2; ++ii) put({size.x - 1, ii}, 2);
            for (size_t ii = size.y / 2; ii < size.y; ++ii) put({size.x - 1, ii}, 3);
            for (size_t ii = 0; ii < size.x / 4; ++ii) put({ii, size.y - 1}, 5);
            for (size_t ii = size.x / 4; ii < 3 * size.x / 4; ++ii) put({ii, size.y - 1}, 4);
            for (size_t ii = 3 * size.x / 4; ii < size.x; ++ii) put({ii, size.y - 1}, 3);
        };
        bcs["123"] = [this] {
            b          = 1;
            unsigned c = 0;
            for (size_t ii = 0; ii < size.x - 1; ++ii) {
                put({ii, 0}, c);
                c = (c + 1) % q;
            }
            for (size_t ii = 0; ii < size.y - 1; ++ii) {
                put({size.x - 1, ii}, c);
                c = (c + 1) % q;
            }
            for (size_t ii = 0; ii < size.x - 1; ++ii) {
                put({size.x - 1 - ii, size.y - 1}, c);
                c = (c + 1) % q;
            }
            for (size_t ii = 0; ii < size.y - 1; ++ii) {
                put({0, size.y - 1 - ii}, c);
                c = (c + 1) % q;
            }
        };
        bcs["12123333"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y; ++j) put({ii, j}, (j > size.y / 2) ? ((ii + j) % 2) : 2);
        };
        bcs["1231234444"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y; ++j) put({ii, j}, (j > size.y / 2) ? unsigned((ii + j) % 3) : 3);
        };
        bcs["mostlyfree"] = [this] {
            b = 1;
            for (size_t ii = 0; ii < size.x; ++ii)
                for (size_t j = 0; j < size.y; ++j) put({ii, j}, q);
        };

        for (size_t ii = 0; ii < size.x; ++ii)
            for (size_t j = 0; j < size.y; ++j) put({ii, j}, prng.uniform_int(q));
        beta *= log(1 + sqrt(double(q)));
        bcs[H['c']]();
        show();
    }

    [[nodiscard]] static auto HH(unsigned i, unsigned j) -> unsigned { return i == j ? 0 : 1; }
    [[nodiscard]] auto        HH(const coo &z, unsigned ii) const -> unsigned {
        return HH(ii, atp(z + dz[0])) + HH(ii, atp(z + dz[1])) + HH(ii, atp(z + dz[2])) + HH(ii, atp(z + dz[3]));
    }

    void up() { up(prng.uniform_coo(coo(size))); }
    void up(coo z) {
        auto   ii = prng.uniform_int(q);
        double const dH = HH(z, ii) - HH(z, atp(z));
        // TODO: at(ucoo()), put(ucoo()) and so on
        if ((dH <= 0) || prng.bernoulli(exp(-beta * dH))) put(ucoo(z), ii);
    }

    unsigned                      q, b = 0;
    double                        beta;
    map<string, function<void()>> bcs;
};

auto main(int argc, char **argv) -> int {
    Hub const H("Potts model", argc, argv, "n=500,q=3,b=1,c=free");
    Potts P(H, H['n'], H['q'], H['b']);
    while (true) P.up();
}
