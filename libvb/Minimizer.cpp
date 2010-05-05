/// @file
/// Implementation of minimizer algorithms.

#include <vb/Minimizer.h>

namespace vb {
  Minimizer::Minimizer (unsigned int n_,
                        double fg_ (const Vector &, Vector &, void *),
                        void *context_)
    : n(n_), f(NULL), g(NULL), fg(fg_), context(context_) {
      init();
    }

  Minimizer::Minimizer (unsigned int n_,
                        double f_ (const Vector &, void *),
                        Vector g_ (const Vector &, void *),
                        void *context_)
    : n(n_), f(f_), g(g_), fg(NULL), context(context_) {
      init();
    };

  double Minimizer::compute (const Vector &x_) {
    if ((!x_.empty()) && (&x != &x_)) x=x_;

    if (fg) {
      fx = fg (x,gx,context);
    } else {
      fx = f(x,context);
      gx = g(x,context);
    }

    return fx;
  }

  void Minimizer::init () {
    os = NULL;
    er = 1.0;
    ler = 0;

    x = Vector (n);
    gx = Vector (n);
    old_x = Vector (n);
    old_gx = Vector (n);
  }

  void Minimizer::line_search (const Vector &d) {
    old_x.swap(x); old_fx=fx; old_gx.swap(gx);

    double qq_0 = .8 * scalar_product (old_gx,d);
    double dir = (qq_0>0 ? -1 : 1);
    double t_l = 0.0, t_r = 0.0, t = dir;
    double y;

    bool refining = false;

    while (true) {
      // Compute old_x+t*d in-place :
      x.assign (d.begin(), d.end());
      x *= t; x += old_x;

      compute();

      y = old_fx + .3 * t * qq_0;

      if ((fx<=y) && (dir*scalar_product (gx,d) >= dir*qq_0)) break;
      if (fx>y) { t_r=t; refining = true; } else t_l = t;
      if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
      if (t-t_l+1.0 == 1.0) break;
    }

    if (os) {
      double tmp = scalar_product (gx,gx);
      while (tmp<er) {
        er /= 10.0;
        ++ler;
        if (ler%10) *os << '.'; else *os << '*';
      }
    }
  }

  double Minimizer::minimize_grad (const Vector &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    while (fx < old_fx) {
      line_search (Vector(gx));
    }
    return fx;
  }

  double Minimizer::minimize_bfgs (const Vector &x0, const Vector &W0) {
    compute (x0);

    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector dx,dg,Wdg;
    double dgdx,u;

    Vector diag = W0;
    if (diag.size() == 0) diag = Vector (x0.size(), double(1.0));
    Matrix W(x0.size(),x0.size(),diag);

    while (fx < old_fx) {
      line_search(W*gx);

      dx = x - old_x;
      dg = gx - old_gx;
      Wdg = W*dg;
      dgdx = scalar_product(dg,dx);
      dx /= dgdx;
      u = dgdx + scalar_product(dg,Wdg);

      W.rank1update(u*dx-Wdg,dx);
      W.rank1update(dx,-Wdg);
    }

    return fx;
  }

  double Minimizer::minimize_fr (const Vector &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector d(n);
    Vector old_d(n);
    bool first = true;

    while (fx < old_fx) {
      old_d = d; d = -gx;
      if (!first) {
        double c = scalar_product(gx,gx) / scalar_product(old_gx,old_gx);
        d += c * old_d;
      }
      line_search(d);
      first=false;
    }

    return fx;
  }

  double Minimizer::minimize_pr (const Vector &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector d(n);
    Vector old_d(n);
    Vector y(n);

    bool first = true;

    while (fx < old_fx) {
      old_d.swap(d);
      d.assign(gx.begin(), gx.end());
      d *= -1.0;

      if (!first) {
        double c1 = scalar_product(old_gx,old_gx);

        old_gx -= gx;

        double c = scalar_product(old_gx,gx) / c1;

        old_d *= c;
        d -= old_d;
      }

      line_search(d);
      first=false;
    }

    return fx;
  }

  double Minimizer::minimize_qn (const Vector &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector d(n);
    Vector old_d(n);
    Vector y(n);

    bool first = true;

    while (fx < old_fx) {
      old_d.swap(d);
      d.assign(gx.begin(), gx.end());
      d *= -1.0;

      if (!first) {
        y.assign (gx.begin(), gx.end());
        y -= old_gx;
        double c = scalar_product(y,gx) / scalar_product(y,old_d);
        old_d *= c;
        d += old_d;
      }

      line_search(d);
      first=false;
    }

    return fx;
  }
}
