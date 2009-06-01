
#include <vb.h>

int main (int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=500,p=.75",
		"Syntax: Perco-oriented [-n <size>]");
  const int n = CLP.as_int('n');
  const double p = CLP('p');

  vb::Image img (n,n,1,"Oriented site-percolation");
  img.show();

  vb::PRNG prng;

  img.putpoint(0,0,1);

  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      if (prng.bernoulli(p))
	if ( ((i>0)&&img(i-1,j)) || ((j>0)&&img(i,j-1)) )
	  img.putpoint (i,j,1);

  std::cout << img;

  return 0;
}
