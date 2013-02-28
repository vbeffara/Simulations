
/// @file sample.cpp
/// A simple example of how to use the vb::Image class.

#include <vb/Image.h>
#include <vb/CL_Parser.h>

using namespace vb;

class Sample : public Image {
public:
  int n;

  Sample (int _n) : Image (_n,_n,"sample"), n(_n) {}

  vb::Color compute (coo z) {
    int d = norm (z-coo(n/2,n/2));
    return vb::Color ( 1+(2*d*((1<<8)-1))/(n*n) );
  }
};

int main (int argc, char *argv[]) {
  vb::CL_Parser CLP (argc,argv,"n=500");
  int n = CLP ('n');
  Sample S (n);
  S.tessel (0,0, n-1,n-1);
  S.output();
  return 0;
}
