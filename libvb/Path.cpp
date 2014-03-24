/// @file
/// Implementation of the vb::Path class.

#include <vb/Path.h>
#include <vb/TriMatrix.h>
#include <vb/Figure.h>
#include <vb/coo.h>

namespace vb {
    OldPath::OldPath (int l, const std::string & title_, bool rel) : std::vector<char> (l), title(title_), relative(rel) { };

    bool OldPath::self_avoiding () {
        TriMatrix<char> T;
        int l=0;
        coo z(0);

        T.put(z,1);
        for (char i : *this) {
            l = (relative ? l+i : i) % 4; z += dz[l];
            if (T.at(z)) return false; else T.put(z,1);
        }
        return true;
    }

    void OldPath::output (const std::string &s) const { output_pdf (s); }

    void OldPath::output_pdf (const std::string &s) const {
        int l=0; cpx z(0); std::vector<cpx> p(1);
        for (char i : *this) { l = (relative ? l+i : i) % 4; z += dzc[l]; p.push_back(z); }
        Figure F; F.title = title; F.add (new Path(p, Pen(0,.2))); F.output_pdf(s);
    }
}
