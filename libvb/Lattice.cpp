#include <vb/Lattice.h>

namespace vb {
  Lattice & Lattice::bond (int k1, int k2, coo dz) {
    adj[k1].push_back (Lattice_place(dz,k2));
    adj[k2].push_back (Lattice_place(-dz,k1));
    return *this;
  }

  cpx Lattice::shift (int k, int l) const {
    const Lattice_place &m (adj[k][l]);
    return (*this)(m.z,m.k) - (*this)(coo(0,0),k);
  }

  double Lattice::energy () const {
    double t=0;
    for (unsigned k=0; k<n; ++k)
      for (unsigned l=0; l<adj[k].size(); ++l)
        t += norm(shift(k,l));
    return t;
  }

  cpx Lattice::shear () const {
    cpx t=0;
    for (unsigned k=0; k<n; ++k)
      for (unsigned l=0; l<adj[k].size(); ++l)
        t += shift(k,l) * shift(k,l);
    return t;
  }

  double Lattice::relax_once () {
    double diff=0;
    for (unsigned k=1; k<n; ++k) { // Vertex 0 is pinned to ensure uniqueness
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
    for (unsigned k=0; k<n; ++k)
      for (unsigned l=0; l<adj[k].size(); ++l) {
        const Lattice_place &m = adj[k][l];
        cpx u = cpx(m.z.x,m.z.y) + z[m.k] - z[k];
        a += u.imag()*u.imag();
        b += 2*u.real()*u.imag();
        c += u.real()*u.real();
      }
    cpx delta = b*b - 4*a*c;
    cpx t = (-b + sqrt(delta))/(2*a);
    return (t.imag()>0 ? t : conj(t));
  }

  double cost_cp (Lattice const &L) {
    double t=0;
    for (unsigned k=0; k<L.n; ++k)
      for (unsigned l=0; l<L.adj[k].size(); ++l) {
        cpx s = L.shift(k,l);
        double d = sqrt(norm(s));
        double rr = L.r[k] + L.r[L.adj[k][l].k];
        t += (d-rr)*(d-rr);
      }
    return t;
  }

  void Lattice::optimize (double f (Lattice const &), double eps) {
    double cost = f(*this);
    double old_cost = cost + eps + 1;
    double tmp_cost = cost;
    while (old_cost - cost > eps) {
      old_cost = cost;
      double delta = sqrt(cost)/10;

      tau += cpx(delta,0); tmp_cost = f(*this);
      if (tmp_cost < cost) cost = tmp_cost;
      else {
        tau -= cpx(2*delta,0); tmp_cost = f(*this);
        if (tmp_cost < cost) cost = tmp_cost;
        else tau += cpx(delta,0);
      }

      tau += cpx(0,delta); tmp_cost = f(*this);
      if (tmp_cost < cost) cost = tmp_cost;
      else {
        tau -= cpx(0,2*delta); tmp_cost = f(*this);
        if (tmp_cost < cost) cost = tmp_cost;
        else tau += cpx(0,delta);
      }

      for (unsigned i=0; i<n; ++i) {
        if (i>0) {
          z[i] += cpx(delta,0); tmp_cost = f(*this);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            z[i] -= cpx(2*delta,0); tmp_cost = f(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else z[i] += cpx(delta,0);
          }

          z[i] += cpx(0,delta); tmp_cost = f(*this);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            z[i] -= cpx(0,2*delta); tmp_cost = f(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else z[i] += cpx(0,delta);
          }
        }

        r[i] += delta; tmp_cost = f(*this);
        if (tmp_cost < cost) cost = tmp_cost;
        else {
          r[i] -= 2*delta; tmp_cost = f(*this);
          if (tmp_cost < cost) cost = tmp_cost;
          else r[i] += delta;
        }
      }
    }
  }

  Lattice Z2() {
    Lattice Z2(2);
    Z2.bond(0,0,coo(1,0)).bond(0,0,coo(0,1));
    Z2.z[0]=0;
    return Z2;
  }

  Lattice Hx () {
    Lattice H(2,cpx(.5,sqrt(3)/2));
    H.bond(0,1).bond(0,1,coo(-1,0)).bond(0,1,coo(0,-1));
    H.z[0]=0;
    H.z[1]=cpx(1,1)/cpx(3);
    return H;
  }

  Lattice H2 () {
    Lattice H2(4,cpx(0,sqrt(3.0)));
    H2.bond(0,1).bond(1,2).bond(2,3).bond(3,0,coo(0,1)).bond(2,1,coo(1,0)).bond(3,0,coo(1,1));
    H2.z[0]=0;
    H2.z[1]=cpx(0,1.0/3);
    H2.z[2]=cpx(.5,.5);
    H2.z[3]=cpx(.5,5.0/6);
    return H2;
  }

  Lattice T() {
    Lattice T(1,cpx(.5,sqrt(3)/2));
    T.bond(0,0,coo(1,0)).bond(0,0,coo(0,1)).bond(0,0,coo(-1,1));
    T.z[0]=0;
    return T;
  }

  Lattice T2 () {
    Lattice T2(2,cpx(0,sqrt(3.0)));
    T2.bond(0,1).bond(0,0,coo(1,0)).bond(1,0,coo(1,0)).bond(1,0,coo(0,1)).bond(1,0,coo(1,1)).bond(1,1,coo(1,0));
    T2.z[0]=0;
    T2.z[1]=cpx(.5,.5);
    return T2;
  }

  Lattice K () {
    Lattice K(3,cpx(.5,sqrt(3)/2));
    K.bond(0,1).bond(0,2).bond(1,2).bond(1,0,coo(1,0)).bond(2,0,coo(0,1)).bond(1,2,coo(1,-1));
    K.z[0]=0;
    K.z[1]=.5;
    K.z[2]=cpx(0,.5);
    return K;
  }

  Lattice K2 () {
    Lattice K2(6,cpx(0,sqrt(3)));
    K2.bond(0,1).bond(0,2).bond(1,2).bond(2,3).bond(2,4).bond(3,4).bond(4,5)
      .bond(1,0,coo(1,0)).bond(4,3,coo(1,0)).bond(5,3,coo(1,0)).bond(5,1,coo(0,1)).bond(5,0,coo(1,1));
    K2.z[0]=0;
    K2.z[1]=.5;
    K2.z[2]=cpx(.25,.25);
    K2.z[3]=cpx(0,.5);
    K2.z[4]=cpx(.5,.5);
    K2.z[5]=cpx(.75,.75);
    return K2;
  }

  Lattice SO () {
    Lattice SO(4);
    SO.bond(0,1).bond(1,2).bond(2,3).bond(3,0).bond(2,0,coo(0,1)).bond(1,3,coo(1,0));
    SO.z[0]=cpx(.5,.25);
    SO.z[1]=cpx(.75,.5);
    SO.z[2]=cpx(.5,.75);
    SO.z[3]=cpx(.25,.5);
    return SO;
  }

  Lattice Z2C () {
    Lattice Z2C(2);
    Z2C.bond(0,0,coo(1,0)).bond(0,0,coo(0,1)).bond(0,1).bond(0,1,coo(-1,0)).bond(0,1,coo(0,-1)).bond(0,1,coo(-1,-1));
    Z2C.z[0]=0;
    Z2C.z[1]=cpx(.5,.5);
    return Z2C;
  }

  Lattice G67 () {
    Lattice G67(3, cpx(0,sqrt(6.0/7)));
    G67.bond(0,1).bond(0,2).bond(1,2).bond(0,0,coo(1,0)).bond(0,0,coo(0,1)).bond(0,1,coo(-1,0))
      .bond(0,1,coo(0,-1)).bond(0,1,coo(-1,-1)).bond(2,0,coo(0,1));
    G67.z[0]=0;
    G67.z[1]=cpx(.5,.5);
    G67.z[2]=cpx(1.0/6,.5);
    return G67;
  }

  Lattice SV () {
    Lattice SV(4);
    SV.bond(0,1).bond(1,2).bond(2,3).bond(3,0).bond(1,3).bond(1,0,coo(1,0)).bond(2,3,coo(1,0))
      .bond(1,3,coo(1,0)).bond(0,3,coo(0,-1)).bond(1,2,coo(0,-1)).bond(2,0,coo(1,1)).bond(3,1,coo(0,1));
    SV.z[0]=0;
    SV.z[1]=.5;
    SV.z[2]=cpx(.5,.5);
    SV.z[3]=cpx(0,.5);
    return SV;
  }

  Lattice C5 () {
    Lattice C5(5);
    C5.bond(1,0).bond(1,4).bond(0,2).bond(0,3).bond(0,4).bond(2,3).bond(3,4).bond(2,1,coo(1,0)).bond(2,4,coo(1,0))
      .bond(3,4,coo(1,0)).bond(3,1,coo(1,1)).bond(3,0,coo(1,1)).bond(3,2,coo(0,1)).bond(3,0,coo(0,1)).bond(4,0,coo(0,1));
    C5.z[0]=.25;
    C5.z[1]=cpx(0,.125);
    C5.z[2]=cpx(.75,.25);
    C5.z[3]=cpx(.75,.75);
    C5.z[4]=cpx(.25,.5);
    return C5;
  }
}
