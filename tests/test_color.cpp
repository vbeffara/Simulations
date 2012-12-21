#include <vb/Image.h>

using namespace vb;

int main () {
  Image img (256,256,"test_color");

  for (int i=0; i<256; ++i)
    for (int j=0; j<256; ++j)
      img.put (coo(i,j), Color(i,j,(8*(i+j))%256));

  img.output_png();
  return 0;
}
