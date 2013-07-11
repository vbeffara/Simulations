/// @file
/// Implementation of minimizer algorithms.

#include <vb/Minimizer.h>

namespace vb {
  Minimizer::Minimizer (unsigned int n_,
                        double fg_ (const Vector &, Vector &, void *),
                        void *context_)
    : n(n_), f(NULL), g(NULL), fg(fg_), cb(NULL), context(context_) {
      init();
    }

  Minimizer::Minimizer (unsigned int n_,
                        double f_ (const Vector &, void *),
                        Vector g_ (const Vector &, void *),
                        void *context_)
    : n(n_), f(f_), g(g_), fg(NULL), cb(NULL), context(context_) {
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
    er = 1.0;
    ler = 0;

    x = Vector (n);
    gx = Vector (n);
    old_x = Vector (n);
    old_gx = Vector (n);
  }

  void Minimizer::line_search (const Vector &d) {
    old_x.swap(x); old_fx=fx; old_gx.swap(gx);

    double qq_0 = .8 * inner_prod (old_gx,d);
    double dir = (qq_0>0 ? -1 : 1);
    double t_l = 0.0, t_r = 0.0, t = dir;
    double y;

    bool refining = false;

    while (true) {
      x = old_x + t*d;

      compute();

      y = old_fx + .3 * t * qq_0;

      if ((fx<=y) && (dir*inner_prod (gx,d) >= dir*qq_0)) break;
      if (fx>y) { t_r=t; refining = true; } else t_l = t;
      if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
      if (t-t_l+1.0 == 1.0) break;
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
    compute(x0);

    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector dx,dg,Wdg;
    double dgdx,u;

    Vector diag = W0;
    if (diag.size() == 0) diag = Vector (x0.size(), double(1.0));
    Matrix W(x0.size(),x0.size());
    for (unsigned int i=0; i<x0.size(); ++i) W(i,i) = diag[i];

    while (fx < old_fx) {
      line_search(prod(W,gx));

      dx = x - old_x;
      dg = gx - old_gx;
      Wdg = prod(W,dg);
      dgdx = inner_prod(dg,dx);
      dx /= dgdx;
      u = dgdx + inner_prod(dg,Wdg);

      W += outer_prod (u*dx-Wdg,dx) - outer_prod (dx,Wdg);
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
        double c = inner_prod(gx,gx) / inner_prod(old_gx,old_gx);
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
      d = -gx;

      if (!first) {
        double c1 = inner_prod(old_gx,old_gx);

        old_gx -= gx;

        old_d *= inner_prod(old_gx,gx) / c1;
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
      d = -gx;

      if (!first) {
        y = gx - old_gx;
        d += old_d * inner_prod(y,gx) / inner_prod(y,old_d);
      }

      line_search(d);
      first=false;

      if (cb) cb(x,fx,context);
    }

    return fx;
  }
}
