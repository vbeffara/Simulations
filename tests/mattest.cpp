
#include <vb/Matrix.h>
#include <vb/types.h>
#include <iostream>

using namespace vb;
using namespace std;

vb::real SQRT2 (sqrt(2.0));
vb::real SQRT3 (sqrt(3.0));

vb::real f (Matrix<vb::real> x) {
  vb::real x1 = x.data[0][0];
  vb::real x2 = x.data[1][0];

  return (x1-SQRT2)*(x1-SQRT2) + (x2-SQRT3)*(x2-SQRT3);
}

Matrix<vb::real> g (Matrix<vb::real> x) {
  Matrix<vb::real> out(2,1);
  out.data[0][0] = ((x.data[0][0]-SQRT2)*vb::real(2.0));
  out.data[1][0] = ((x.data[1][0]-SQRT3)*vb::real(2.0));
  return out;
}

#define m1 vb::real(.3)
#define m2 vb::real(.7)

vb::real line_search (Matrix<vb::real> x, Matrix<vb::real> d) {
  vb::real t_l = 0.0, t_r = 0.0, t(1.0);
  vb::real q_0 = f(x), qq_0 = m2 * scalar_product (g(x),d);
  while (true) {
    //cout << "  " << t_l << " < " << t << " < " << t_r << " - " << f(x+t*d) << endl;
    vb::real q = f (x+t*d);
    vb::real qq = scalar_product (g(x+t*d),d);
    vb::real y = q_0 + m1 * t * qq_0;

    //cout << "  " << t << " - " << q << endl;

    if ((q<=y) && (qq>=qq_0)) break;
    if (q>y) t_r=t;
    else t_l = t;

    if (t_r>t_l) t = (t_r+t_l)/vb::real(2.0);
    else t = t_l * vb::real(2.0);
  }
  return t;
}

int main() {
  Matrix<vb::real> x(2,1);
  vb::real ff = f(x);
  Matrix<vb::real> gg = g(x);

  Matrix<vb::real> W(2,2);
  for (int i=0; i<2; ++i) W.data[i][i] = vb::real(1.0);

  vb::real old_f = ff + vb::real(1.0);

  for (int i=0; i<10; ++i) {
    cout << x << " -> " << ff << endl;
    old_f = ff;

    Matrix<vb::real> dd = - W * gg;
    Matrix<vb::real> ss = dd * line_search(x,dd) ; x += ss;
    Matrix<vb::real> newgg = g(x);
    Matrix<vb::real> yy = newgg - gg;

    if (norm_squared(ss) == vb::real(0.0)) break;

    vb::real u = vb::real(1.0) + scalar_product(yy,W*yy) / scalar_product(yy,ss);
    W += u * ss * transpose(ss) / scalar_product(yy,ss);
    W -= (ss*transpose(yy)*W + W*yy*transpose(ss)) / scalar_product(yy,ss);

    ff = f(x); gg = g(x);
  }
}
