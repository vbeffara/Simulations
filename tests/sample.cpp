
/// @file sample.cpp
/// A simple example of how to use the vb::Image class.

#include <vb/Image.h>
#include <vb/CL_Parser.h>

int n; ///< The size of the image.

/// A simple coloring function.
unsigned char f (int x, int y) {
  int d = (x-n/2)*(x-n/2) + (y-n/2)*(y-n/2);
  return 1+(2*d*((1<<8)-1))/(n*n);
}

/// The main function.
int main(int argc, char *argv[])
{
  vb::CL_Parser CLP (argc,argv,"n=500");
  n = CLP('n');

  vb::Image img (n,n,8,"Sample");
  img.show();

  img.tessellate (f, 0,0, n-1,n-1);

  img.output_png ();
  return 0;
}
