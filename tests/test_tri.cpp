#include <vb/TriMatrix.h>
#include <iostream>

using namespace vb;

int main () {
  TriMatrix <int> M;

  M.put (coo(35,42),3);
  M.put (coo(1234,5678),5);
  M.put (coo(91823749,-2793474),23);

  std::cout << "Far   : " << M.at(coo(981327,2371827))    << std::endl;
  std::cout << "Set 1 : " << M.at(coo(35,42))             << std::endl;
  std::cout << "Set 2 : " << M.at(coo(1234,5678))         << std::endl;
  std::cout << "Set 3 : " << M.at(coo(91823749,-2793474)) << std::endl;
  std::cout << "Unset : " << M.at(coo(3,4))               << std::endl;

  return 0;
}
