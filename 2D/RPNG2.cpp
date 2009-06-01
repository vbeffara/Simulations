#include <vb.h>
#include <iostream>

using namespace vb;

int main (int argc, char **argv) {
  CL_Parser clp (argc,argv,"n=500,l=.2,a=0,b=0");
  int n = clp.as_int('n');
  double l = clp('l');
  int a = clp.as_int('a');
  int b = clp.as_int('b');
  l = l/(1+l);

  PRNG prng;

  Image img (n,n,1,"RPNG Process");

  int h[n];
  for (int i=0; i<n; ++i) {
    h[i]=(i<(n>>1) ? a : b);
    img.putpoint_safe (i,(n>>1)-h[i],1);
  }
  img.show();

  unsigned long e=0;

  for (unsigned long t=0;;++t) {
    int x = (int)prng.uniform(n);
    int xd = (x+1)%n;
    int xg = (x+n-1)%n;
    int nx,delta;

    if (prng.bernoulli(l)) { // nucleation
      img.putpoint_safe (x,(n>>1) - h[x],0);
      img.putpoint_safe (x,(n>>1) - h[x]+1,1);
      img.putpoint_safe (x,n>>1,1);

      img.putpoint_safe (xd,(n>>1) - h[xd],0);
      img.putpoint_safe (xd,(n>>1) - h[xd]-1,1);
      img.putpoint_safe (xd,n>>1,1);

      e = e - (h[x]*h[x]) - (h[xd]*h[xd]);
      h[x]--; h[xd]++;
      e = e + (h[x]*h[x]) + (h[xd]*h[xd]);
    } else if (h[x] != 0) { // jump
      if (prng.bernoulli(.5)) nx=xd; else nx=xg;
      if (h[x]>0) delta=1; else delta=-1;

      img.putpoint_safe (x,(n>>1) - h[x],0);
      img.putpoint_safe (x,(n>>1) - h[x]+delta,1);
      img.putpoint_safe (x,n>>1,1);

      img.putpoint_safe (nx,(n>>1) - h[nx],0);
      img.putpoint_safe (nx,(n>>1) - h[nx]-delta,1);
      img.putpoint_safe (nx,n>>1,1);

      e = e - (h[x]*h[x]) - (h[nx]*h[nx]);
      h[x] -= delta; h[nx] += delta;
      e = e + (h[x]*h[x]) + (h[nx]*h[nx]);
    }

/*  if (!(t%(n*n))) {
      r_e = (9*r_e+e)/10;
      std::cerr << r_e << std::endl;
    } */
  }

  return 0;
}
