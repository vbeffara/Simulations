
#include <vb/Matrix.h>
#include <vb/types.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vb;

//namespace vb { typedef double Real; }

Real f (Matrix<Real> x) {
  return 2.0 - cos(x.data[0][0]) - cos(x.data[1][0]);
}

Matrix<Real> g (Matrix<Real> x) {
  Matrix<Real> out(2,1);
  out.data[0][0] = sin(x.data[0][0]);
  out.data[1][0] = sin(x.data[1][0]);
  return out;
}

#define m1 .4
#define m2 .8

Real line_search (Matrix<Real> x, Matrix<Real> d) {
  bool reverse = false;
  if (scalar_product(g(x),d)>0) { reverse = true; d = -d; }
  Real t_l = 0.0, t_r = 0.0, t(1.0);
  Real q_0 = f(x), qq_0 = m2 * scalar_product (g(x),d);
  while (true) {
    Real q = f (x+t*d);
    Real qq = scalar_product (g(x+t*d),d);
    Real y = q_0 + m1 * t * qq_0;

    if ((q<=y) && (qq>=qq_0)) break;
    if (q>y) t_r=t;
    else t_l = t;

    if (t_r>t_l) t = (t_r+t_l)/2.0;
    else t = t_l * 2.0;

    if (t-t_l+1.0 == 1.0) break;
  }
  if (reverse) { t = -t; }
  return t;
}

int main() {
  Matrix<Real> x(2,1);
  x.data[0][0] = 1.0;
  x.data[1][0] = .5;

  Real ff = f(x);
  Matrix<Real> gg = g(x);

  Matrix<Real> W(2,2);
  for (int i=0; i<2; ++i) W.data[i][i] = Real(1.0);

  Real old_f = ff + Real(1.0);

  for (int i=0;;++i) {
    cerr << i << " " << x << " -> " << ff << endl;
    old_f = ff;

    Matrix<Real> dd = - W * gg;
    Matrix<Real> ss = dd * line_search(x,dd) ; x += ss;
    Real newff = f(x);
    Matrix<Real> newgg = g(x);
    Matrix<Real> yy = newgg - gg;

    if (newff >= ff) { x -= ss; break; }
    if (scalar_product(yy,ss) <= 0.0) { x -= ss; break; }

    Real u = Real(1.0) + scalar_product(yy,W*yy) / scalar_product(yy,ss);
    W += u * ss * transpose(ss) / scalar_product(yy,ss);
    W -= (ss*transpose(yy)*W + W*yy*transpose(ss)) / scalar_product(yy,ss);

    ff = newff; gg = newgg;
  }
  cout << "Final value: " << x << " -> " << ff << endl;
}
