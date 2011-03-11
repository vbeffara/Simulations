/// @file
/// Implementation of the vb::Path class.

#include <vb/Path.h>
#include <vb/TriMatrix.h>
#include <vb/Figure.h>

static const int dx[4] = { 1, 0,-1, 0 };
static const int dy[4] = { 0, 1, 0,-1 };

namespace vb {
  OldPath::OldPath (int l, const std::string & title_, bool rel)
    : std::vector<char> (l), title(title_), relative(rel) { };

  bool OldPath::self_avoiding () {
    TriMatrix<char> T;
    int l=0, x=0, y=0;

    T.put(x,y,1);
    for (unsigned i=0; i<size(); ++i) {
      l = (relative ? l+at(i) : at(i)) % 4;
      x += dx[l]; y += dy[l];
      if (T.get(x,y)) return false; else T.put(x,y,1);
    }
    return true;
  }

  void OldPath::output (const std::string &s) const { output_pdf (s); }

  void OldPath::output_pdf (const std::string &s) const {
    Figure F;

    int l=0, x=0, y=0;
    for (unsigned i=0; i<size(); ++i) {
      l = (relative ? l+at(i) : at(i)) % 4;
      F.add (new Segment ( cpx(x,y), cpx(x+dx[l],y+dy[l]) ));
      x += dx[l]; y += dy[l];
    }

    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;

    F.output_pdf(os.str());
  }
}
