
#include <vb/minimize.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vb;

#define DIM 400

double f (const Vector<double> &x, void *context = NULL) {
  double o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<double> g (const Vector<double> &x, void *context = NULL) {
  Vector<double> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

int main () {
  Vector<double> x(DIM); for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  Vector<double> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);
  PointValueGradient<double> min = minimize_bfgs (f,g,x);
  cout << "Final value: " << min.value << endl;
}
