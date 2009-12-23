
#include <vb/Minimizer.h>

using namespace std;
using namespace vb;

#define DIM 400

double f (const Vector &x, void *context = NULL) {
  double o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector g (const Vector &x, void *context = NULL) {
  Vector out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

double fg (const Vector &x, Vector &g, void *context = NULL) {
  double o = 0;
  for (unsigned int i=0; i<DIM; ++i) {
    o += (1 - cos(x[i]/(i+1)));
    g[i] = sin(x[i]/(i+1))/(i+1);
  }
  return o;
}

int main () {
  Vector x0(DIM); for (unsigned int i=0; i<DIM; ++i) x0[i] = cos(double(i));
  Vector W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);

  Minimizer<double> M (DIM,fg);
  double min = M.minimize_qn (x0);
  cout << "Final value: " << min << endl;
}
