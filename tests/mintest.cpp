
#include <vb/Minimizer.h>
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

Real fg (const Vector<Real> &x, Vector<Real> &g, void *context = NULL) {
  Real o = 0;
  for (unsigned int i=0; i<DIM; ++i) {
    o += (1 - cos(x[i]/(i+1)));
    g[i] = sin(x[i]/(i+1))/(i+1);
  }
  return o;
}

int main () {
  Vector<Real> x0(DIM); for (unsigned int i=0; i<DIM; ++i) x0[i] = cos((double)i);
  Vector<Real> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);

  Minimizer<Real> M (DIM,fg);
  Real min = M.minimize_qn (x0);
  cout << "Final value: " << min << endl;
}
