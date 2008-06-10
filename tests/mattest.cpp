
#include <vb/Matrix.h>
#include <vb/types.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vb;

//namespace vb { typedef double Real; }

Real f (Matrix<vb::Real> x) {
  return 2.0 - cos(x.data[0][0]) - cos(x.data[1][0]);
}

Matrix<vb::Real> g (Matrix<vb::Real> x) {
  Matrix<vb::Real> out(2,1);
  out.data[0][0] = sin(x.data[0][0]);
  out.data[1][0] = sin(x.data[1][0]);
  return out;
}

#define m1 vb::Real(.4)
#define m2 vb::Real(.8)

vb::Real line_search (Matrix<vb::Real> x, Matrix<vb::Real> d) {
  bool reverse = false;
  if (scalar_product(g(x),d)>0) { reverse = true; d = -d; }
  vb::Real t_l = 0.0, t_r = 0.0, t(1.0);
  vb::Real q_0 = f(x), qq_0 = m2 * scalar_product (g(x),d);
  while (true) {
    vb::Real q = f (x+t*d);
    vb::Real qq = scalar_product (g(x+t*d),d);
    vb::Real y = q_0 + m1 * t * qq_0;

    if ((q<=y) && (qq>=qq_0)) break;
    if (q>y) t_r=t;
    else t_l = t;

    if (t_r>t_l) t = (t_r+t_l)/vb::Real(2.0);
    else t = t_l * vb::Real(2.0);

    if (t-t_l+1.0 == vb::Real(1.0)) break;
  }
  if (reverse) { t = -t; }
  return t;
}

int main() {
  Matrix<vb::Real> x(2,1);
  x.data[0][0] = 1.0;
  x.data[1][0] = .5;

  vb::Real ff = f(x);
  Matrix<vb::Real> gg = g(x);

  Matrix<vb::Real> W(2,2);
  for (int i=0; i<2; ++i) W.data[i][i] = vb::Real(1.0);

  vb::Real old_f = ff + vb::Real(1.0);

  for (int i=0;;++i) {
    cerr << i << " " << x << " -> " << ff << endl;
    old_f = ff;

    Matrix<vb::Real> dd = - W * gg;
    Matrix<vb::Real> ss = dd * line_search(x,dd) ; x += ss;
    vb::Real newff = f(x);
    Matrix<vb::Real> newgg = g(x);
    Matrix<vb::Real> yy = newgg - gg;

    if (newff >= ff) { x -= ss; break; }
    if (scalar_product(yy,ss) <= 0.0) { x -= ss; break; }

    vb::Real u = vb::Real(1.0) + scalar_product(yy,W*yy) / scalar_product(yy,ss);
    W += u * ss * transpose(ss) / scalar_product(yy,ss);
    W -= (ss*transpose(yy)*W + W*yy*transpose(ss)) / scalar_product(yy,ss);

    ff = newff; gg = newgg;
  }
  cout << "Final value: " << x << " -> " << ff << endl;
}
