#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

namespace vb {
    template <> auto to_Color(size_t t) -> Color { return t == 0 ? BLACK : Indexed(int(t)); }
} // namespace vb

class ACP : public Bitmap<size_t> {
public:
    explicit ACP(const Hub &H) : Bitmap<size_t>(H.title, {size_t(H['n']), size_t(H['n'])}), kid(H['z']), maxage(H['m']) {
        put(size / 2, 1);
        P = {H['d'], H['a']};
        for (size_t ii = 0; ii < size_t(H['m']) - kid; ++ii) P.push_back(double(H['b']) + double(H['r']) * double(ii));
        double s = 0;
        for (double const u : P) s += u;
        for (double &u : P) u /= s;
    }
    void run() {
        auto z = prng.uniform_coo(size);
        if (at(z) == 0) return;
        auto action = prng.discrete(P);
        if (action == 0) {
            put(z, 0);
        } else if ((action == 1) && (at(z) < maxage)) {
            put(z, at(z) + 1);
        } else if (action + kid - 1 <= at(z)) {
            coo const nz = coo(z) + dz[prng.uniform_int(4U)];
            if (atp(nz) == 0) putp(nz, 1);
        }
    }
    size_t         kid, maxage;
    vector<double> P;
};

auto main(int argc, char **argv) -> int {
    Hub const H("Contact process with aging", argc, argv, "n=400,d=.01,a=.1,z=0,m=5,b=.2,r=.1");
    ACP A(H);
    A.show();
    while (true) A.run();
}
