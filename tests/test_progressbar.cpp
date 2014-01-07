
#include <vb/ProgressBar.h>

using namespace vb;

int main (int argc, char **argv) {
  int t = 1e9;
  { ProgressBar P (t); for (int i=0; i<t; ++i) P.set(i); }
  { ProgressBar P (t); for (int i=0; i<t; ++i) P.set(i); }
}
