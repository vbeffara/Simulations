
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

/** A simple example of how to use the libvb library.
 * @file sample.cpp
 *
 * It does nothing interesting, but demonstrates the vb::Image class,
 * with the vb::Image::tessellate() method and EPS creation. Here is a
 * line-by-line description of the main() function:
 *
 * @example sample.cpp
 * @dontinclude sample.cpp
 * @skip int main
 * @until {
 * First, create an instance of vb::CL_Parser. Use it to get the value
 * of n, the size of the image (the default is 500 here):
 * @skip CLP
 * @until as_int
 * Then, create a vb::Image of this size, and display it on the screen:
 * @skip Image
 * @until onscreen
 * Fill it using the coloring function f:
 * @skipline tessellate
 * And finally, export it to std::cout as an EPS file and exit:
 * @skip <<
 * @until }
 *
 * Full source code of sample.cpp:
 */

