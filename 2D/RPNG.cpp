#include <vb/CoarseImage.h>
#include <vb/Hub.h>

using namespace vb;

int main (int argc, char **argv) {
  H.init ("Randomized Polynuclear Growth", argc,argv, "n=500,e=0.01,f,c=0");
  const int    n = H['n'];
  const double e = H['e'];
  const bool   f = H['f'];
  const double c = H['c'];

  CoarseImage img (n,n,10);
  img.show();

  int *height = new int[n];
  for (int i=0; i<n; ++i) {
    height[i]=0;
    img.put(coo(i,0),1);
  }

  bool running = true;
  while (running) {
    int x = 1 + prng()%(n-2);
    int h = height[x];
    int d = prng()&(1<<14);

    if (d) { // If trying to grow
      if ( (height[x-1]>h) || (height[x+1]>h)  || (prng.bernoulli(e)) ||
	   ((c>0)&&(x==n/2)&&(prng.bernoulli(c))) )
	++ height[x];
    } else { // If trying to shrink
      if ( (height[x-1]<h) || (height[x+1]<h) )
	-- height[x];
    }

    if (height[x]>=n) break;

    if (height[x]<h) img.put(coo(x,h),0);
    if (height[x]>h) img.put(coo(x,h+1),1);

    if(f) {
      height[0]=height[1];
      img.put(coo(0,height[0]),1);
      img.put(coo(0,height[0]+1),0);

      height[n-1]=height[n-2];
      img.put(coo(n-1,height[n-1]),1);
      img.put(coo(n-1,height[n-1]+1),0);
    }
  }

  delete[] height; return 0;
}
