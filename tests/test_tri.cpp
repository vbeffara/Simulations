#include <vb/TriMatrix.h>
#include <iostream>

using namespace vb;

int main () {
  TriMatrix <int> M (18);

  M.put (35,42,3);

  std::cout << "Far   : " << M(981327,2371827) << std::endl;
  std::cout << "Set   : " << M(35,42)          << std::endl;
  std::cout << "Unset : " << M(3,4)            << std::endl;

  return 0;
}
