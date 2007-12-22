
/// @file sample.cpp
/// A simple example of how to use the vb::Image class.

#include <vb/Image.h>
#include <vb/CL_Parser.h>

#include <iostream>

int n; // The size of the image.

// A simple coloring function:

char f (int x, int y) {
  int d = (x-n/2)*(x-n/2) + (y-n/2)*(y-n/2);
  return 1+(2*d*((1<<8)-1))/(n*n);
}

// The main function:

int main(int argc, char *argv[])
{
  // Get the size as parameter n.

  vb::CL_Parser CLP (argc,argv,"n=500");
  n = CLP.as_int('n');

  // Create an image and put it on the screen

  vb::Image img (n,n,8,"Sample");
  img.onscreen();

  // Fill it according to some function, here f

  img.tessellate (f, 0,0, n-1,n-1);

  // Then produce an EPS file

  std::cout << img;
  return 0;
}
