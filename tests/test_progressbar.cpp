
#include <vb/ProgressBar.h>

using namespace vb;

int main (int argc, char **argv) {
  int t = 6e8;
  ProgressBar P (t);
  for (int i=0; i<t; ++i) {
    P.set(i);
  }
}
