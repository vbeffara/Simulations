#include <gsl/gsl>
#include <vb/Figure.h>
#include <vb/Path.h>
#include <vb/data/TriMatrix.h>

namespace vb {
    OldPath::OldPath(int l, bool rel) : std::vector<char>(l), relative(rel) {}

    bool OldPath::self_avoiding() {
        TriMatrix<char> T;
        int             l = 0;
        coo             z{0, 0};

        T.put(z, 1);
        for (char i : *this) {
            l = (relative ? l + i : i) % 4;
            z += dz[l];
            if (T.at(z) != 0) return false;
            T.put(z, 1);
        }
        return true;
    }

    void OldPath::output(const std::string &s) const { output_pdf(s); }

    void OldPath::output_pdf(const std::string &s) const {
        static const cpx dzc[4] = {cpx(1, 0), cpx(0, 1), cpx(-1, 0), cpx(0, -1)};
        int              l      = 0;
        cpx              z(0);
        std::vector<cpx> p(1);
        for (char i : *this) {
            l = (relative ? l + i : i) % 4;
            z += gsl::at(dzc, l);
            p.push_back(z);
        }
        Figure F{s};
        F.add(std::make_unique<Path>(p, Pen(BLACK, .2)));
        F.output_pdf(s);
    }
} // namespace vb
