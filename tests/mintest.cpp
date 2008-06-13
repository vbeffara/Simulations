
#include <vb/minimize.h>
#include <vb/Real.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vb;

#define DIM 400

Real f (const Vector<Real> &x, void *context = NULL) {
  Real o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<Real> g (const Vector<Real> &x, void *context = NULL) {
  Vector<Real> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

int main () {
  Vector<Real> x(DIM); for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  Vector<Real> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);
  PointValueGradient<Real> min = minimize_bfgs (f,g,x,NULL,W0);
  cout << "Final value: " << min.value << endl;
}
