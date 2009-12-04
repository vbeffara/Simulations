#include <vb/TriMatrix.h>
#include <iostream>

using namespace vb;

int main () {
  TriMatrix <int> M (-1);

  M.put (35,42,3);
  M.put (1234,5678,5);
  M.put (91823749,-2793474,23);

  std::cout << "Far   : " << M(981327,2371827)    << std::endl;
  std::cout << "Set 1 : " << M(35,42)             << std::endl;
  std::cout << "Set 2 : " << M(1234,5678)         << std::endl;
  std::cout << "Set 3 : " << M(91823749,-2793474) << std::endl;
  std::cout << "Unset : " << M(3,4)               << std::endl;

  return 0;
}
