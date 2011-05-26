
#include <vb/Lattice.h>

namespace vb {
  Lattice_vertex & Lattice_vertex::operator+= (const Lattice_move &m) {
    x+=m.dx; y+=m.dy; k=m.k;
    return *this;
  }

  Lattice_vertex Lattice_vertex::operator+ (const Lattice_move &m) const {
    Lattice_vertex tmp (x,y,k);
    tmp += m;
    return tmp;
  }

  Lattice::Lattice (int _n, cpx _tau) : n(_n), adj(_n), tau(_tau), z(_n) { }

  cpx Lattice::operator() (int x, int y, int k) const {
    const cpx &t = z[k];
    return cpx(x,0)+t.real() + (cpx(y,0)+t.imag()) * tau;
  }

  cpx Lattice::operator() (const Lattice_vertex &v) const {
    return (*this)(v.x,v.y,v.k);
  }

  void Lattice::bond (int k1, int k2, int dx, int dy) {
    adj[k1].push_back (Lattice_move(k2,dx,dy));
    adj[k2].push_back (Lattice_move(k1,-dx,-dy));
  }

  cpx Lattice::shift (int k, int l) const {
    const Lattice_move &m (adj[k][l]);
    return (*this)(m.dx,m.dy,m.k) - (*this)(0,0,k);
  }

  double Lattice::energy () const {
    double t=0;
    for (int k=0; k<n; ++k)
      for (int l=0; l<adj[k].size(); ++l)
        t += norm(shift(k,l));
    return t;
  }

  double Lattice::relax_once () {
    double diff=0;
    for (int k=1; k<n; ++k) { // Vertex 0 is pinned to ensure uniqueness
      if (int d = adj[k].size()) {
        cpx s(0);
        for (int l=0; l<d; ++l) s += shift(k,l);
        s /= d;
        diff += abs(s);
        z[k] += s;
      }
    }
    return diff;
  }
}
