
#include <vb/Minimizer.h>

using namespace std;
using namespace vb;

#define DIM 400

double f (const Vector<double> &x, void *) {
  double o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<double> g (const Vector<double> &x, void *) {
  Vector<double> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

double fg (const Vector<double> &x, Vector<double> &g, void *) {
  double o = 0;
  for (unsigned int i=0; i<DIM; ++i) {
    o += (1 - cos(x[i]/(i+1)));
    g[i] = sin(x[i]/(i+1))/(i+1);
  }
  return o;
}

int main () {
  Vector<double> x0(DIM); for (unsigned int i=0; i<DIM; ++i) x0[i] = cos(double(i));
  Vector<double> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);

  Minimizer<double> M (DIM,fg);
  double min = M.minimize_qn (x0);
  cout << "Final value: " << min << endl;
}
