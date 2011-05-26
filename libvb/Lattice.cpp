
#include <vb/Lattice.h>

namespace vb {
  Lattice::Lattice (int _n, cpx _tau) : n(_n), adj(_n), tau(_tau), z(_n) { }

  void Lattice::bond (int k1, int k2, int dx, int dy) {
    adj[k1].push_back (Lattice_move(k2,dx,dy));
    adj[k2].push_back (Lattice_move(k1,-dx,-dy));
  }

  Lattice_vertex & Lattice_vertex::operator= (const Lattice_vertex &v) {
    assert (&L == &(v.L));
    x=v.x; y=v.y; k=v.k;
    return *this;
  }

  Lattice_vertex & Lattice_vertex::operator+= (const Lattice_move &m) {
    x+=m.dx; y+=m.dy; k=m.k;
    return *this;
  }

  Lattice_vertex Lattice_vertex::operator+ (const Lattice_move &m) const {
    Lattice_vertex tmp (L,x,y,k);
    tmp += m;
    return tmp;
  }

  Lattice_vertex::operator cpx () {
    return cpx(x,0) + cpx(y,0)*L.tau + L.z[k];
  }
}
