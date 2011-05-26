
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

  Lattice::Lattice (int _n, cpx _tau) : n(_n), adj(_n), tau(_tau), z(_n), r(_n) { }

  cpx Lattice::actual (cpx xy) const {
    return xy.real() + tau*xy.imag();
  }

  cpx Lattice::operator() (int x, int y, int k) const {
    return actual (cpx(x,y)+z[k]);
  }

  cpx Lattice::operator() (const Lattice_vertex &v) const {
    return (*this)(v.x,v.y,v.k);
  }

  Lattice & Lattice::bond (int k1, int k2, int dx, int dy) {
    adj[k1].push_back (Lattice_move(k2,dx,dy));
    adj[k2].push_back (Lattice_move(k1,-dx,-dy));
    return *this;
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

  cpx Lattice::shear () const {
    cpx t=0;
    for (int k=0; k<n; ++k)
      for (int l=0; l<adj[k].size(); ++l)
        t += shift(k,l) * shift(k,l);
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

  void Lattice::relax (double eps) {
    while (relax_once()>eps) {}
  }

  cpx Lattice::tau_rw () const {
    double a=0, b=0, c=0;
    for (int k=0; k<n; ++k)
      for (int l=0; l<adj[k].size(); ++l) {
        const Lattice_move &m = adj[k][l];
        cpx u = cpx(m.dx,m.dy) + z[m.k] - z[k];
        a += u.imag()*u.imag();
        b += 2*u.real()*u.imag();
        c += u.real()*u.real();
      }
    cpx delta = b*b - 4*a*c;
    cpx t = (-b + sqrt(delta))/(2*a);
    return (t.imag()>0 ? t : conj(t));
  }

  double Lattice::cost_cp () const {
    double t=0;
    for (int k=0; k<n; ++k)
      for (int l=0; l<adj[k].size(); ++l) {
        cpx s = shift(k,l);
        double d = sqrt(norm(s));
        double rr = r[k] + r[adj[k][l].k];
        t += (d-rr)*(d-rr);
      }
    return t;
  }

  void Lattice::optimize (LatticeCostFunction f, double eps) {
    double cost = eval(f);
    double old_cost = cost + eps + 1;
    double tmp_cost = cost;
    while (old_cost - cost > eps) {
      old_cost = cost;
      double delta = sqrt(cost)/10;

      tau += cpx(delta,0); tmp_cost = eval(f);
      if (tmp_cost < cost) cost = tmp_cost;
      else {
        tau -= cpx(2*delta,0); tmp_cost = eval(f);
        if (tmp_cost < cost) cost = tmp_cost;
        else tau += cpx(delta,0);
      }

      tau += cpx(0,delta); tmp_cost = eval(f);
      if (tmp_cost < cost) cost = tmp_cost;
      else {
        tau -= cpx(0,2*delta); tmp_cost = eval(f);
        if (tmp_cost < cost) cost = tmp_cost;
        else tau += cpx(0,delta);
      }

      for (int i=0; i<n; ++i) {
        if (i>0) {
          z[i] += cpx(delta,0); tmp_cost = eval(f);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            z[i] -= cpx(2*delta,0); tmp_cost = eval(f);
            if (tmp_cost < cost) cost = tmp_cost;
            else z[i] += cpx(delta,0);
          }

          z[i] += cpx(0,delta); tmp_cost = eval(f);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            z[i] -= cpx(0,2*delta); tmp_cost = eval(f);
            if (tmp_cost < cost) cost = tmp_cost;
            else z[i] += cpx(0,delta);
          }
        }

        r[i] += delta; tmp_cost = eval(f);
        if (tmp_cost < cost) cost = tmp_cost;
        else {
          r[i] -= 2*delta; tmp_cost = eval(f);
          if (tmp_cost < cost) cost = tmp_cost;
          else r[i] += delta;
        }
      }
    }
  }
}
