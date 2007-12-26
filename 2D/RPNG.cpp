
#include <iostream>
#include <vb.h>
#include <vb/CoarseImage.h>

int main (int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=500,e=0.01,f,c=0");
  const int n = CLP.as_int('n');
  const double e = CLP.as_double('e');
  const bool f = CLP.as_bool('f');
  const double c = CLP.as_double('c');

  vb::CoarseImage img (n,n,10,"RPNG");
  img.show();

  int *height = new int[n];
  for (int i=0; i<n; ++i) {
    height[i]=0;
    img.putpoint(i,0,1);
  }

  vb::PRNG prng;

  bool running = true;
  while (running) {
    int x = 1 + prng.rand()%(n-2);
    int h = height[x];
    int d = prng.rand()&(1<<14);

    if (d) { // If trying to grow
      if ( (height[x-1]>h) || (height[x+1]>h)  || (prng.bernoulli(e)) ||
	   ((c>0)&&(x==n/2)&&(prng.bernoulli(c))) )
	++ height[x];
    } else { // If trying to shrink
      if ( (height[x-1]<h) || (height[x+1]<h) )
	-- height[x];
    }

    if (height[x]>=n) break;

    if (height[x]<h) img.putpoint(x,h,0);
    if (height[x]>h) img.putpoint(x,h+1,1);

    if(f) {
      height[0]=height[1]; 
      img.putpoint(0,height[0],1);
      img.putpoint(0,height[0]+1,0);

      height[n-1]=height[n-2];
      img.putpoint(n-1,height[n-1],1);
      img.putpoint(n-1,height[n-1]+1,0);
    }
  }

  std::cout << img;
  return 0;
}
