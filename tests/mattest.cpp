
#include <vb/Matrix.h>
#include <vb/types.h>
#include <iostream>

using namespace vb;
using namespace std;

int main() {
  Matrix<vb::real> M(3,3);
  Matrix<vb::real> N(3,3);

  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      M.data[i][j] = i+j;
      N.data[i][j] = i+j;
    }
  }

  cout << M << endl;
  cout << N << endl;
  M *= N;
  cout << M << endl;
}
