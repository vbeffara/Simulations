
/** A simple example of how to use the libvb library.
 *
 * It does nothing interesting, but demonstrates the vb::Image class,
 * with the tesselate() method and EPS creation.
 *
 * @file sample.cpp
 */

#include <vb/Image.h>
#include <vb/CL_Parser.h>

#include <iostream>

int n; ///< The size of the image.

/** A simple coloring function. */

char f (int x, int y) {
  int d = (x-n/2)*(x-n/2) + (y-n/2)*(y-n/2);
  return 1+(2*d*((1<<4)-1))/(n*n);
}

/** The main() function. */

int main(int argc, char *argv[])
{
  // Get the size as parameter n.

  vb::CL_Parser CLP (argc,argv,"n=500");
  n = CLP.as_int('n');

  // Create an image and put it on the screen

  vb::Image img (n,n,4,"Sample");
  img.onscreen();

  // Fill it according to some function, here f

  img.tesselate (f, 0,0, n-1,n-1);
  img.update();

  // Then produce an EPS file

  std::cout << img;
  return 0;
}
