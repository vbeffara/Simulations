
#include <vb/minimize.h>
#include <iostream>
#include <math.h>

#define Real double

using namespace std;
using namespace vb;

#define DIM 400

Real f (const Vector<Real> &x) {
  Real o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<Real> g (const Vector<Real> &x) {
  Vector<Real> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

int main () {
  Vector<Real> x(DIM); for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  Vector<Real> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);
  PointValueGradient<Real> min = minimize_bfgs (f,g,x);
  cout << "Final value: " << min.value << endl;
}
