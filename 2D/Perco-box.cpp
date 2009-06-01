
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <iostream>

using namespace std;
using namespace vb;

int *fifox,*fifoy;
Image *img;
PRNG prng;

void bndcnx (int ox, int oy) {
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  (*img)(ox+oy*img->width) |= 3;

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&((*img)(k)&16)&&!((*img)(k+1)&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      (*img)(k+1)|=3;
    }
    if ((j<img->height-1)&&((*img)(k)&32)&&!((*img)(k+img->width)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      (*img)(k+img->width)|=3;
    }
    if ((i>0)&&((*img)(k-1)&16)&&!((*img)(k-1)&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      (*img)(k-1)|=3;
    }
    if ((j>0)&&((*img)(k-img->width)&32)&&!((*img)(k-img->width)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      (*img)(k-img->width)|=3;
    }
  }  
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=100,m=0,t=1,p=.5,q=0");
  int n = CLP.as_int('n');
  int m = CLP.as_int('m');
  int t = CLP.as_int('t');
  double p = CLP('p');
  double q = CLP('q');

  if (m==0) m=n;
  if (q==0) q=p;

  img = new Image (n,m,8,"Bond-percolation");
  fifox = new int[m*n];
  fifoy = new int[m*n];

  int ncross = 0;
  ProgressBar PB (t);

  for (int iter=0; iter<t; ++iter) {
    PB.update (iter);

    for (int i=0; i<n; ++i)
      for (int j=0; j<m; ++j)
        (*img)(i,j) = 16*prng.bernoulli(p) + 32*prng.bernoulli(q);

    for (int j=0; j<m; ++j) {
      (*img)(0,j) |= 32;
      (*img)(n-1,j) |= 32;
    }

    bndcnx (0,0);
    if ((*img)(n-1,0) & 3) ++ncross;
  }

  PB.die();
  cout << m << " " << n << " " << p << " " << q << " | " << t << " | " << ncross << " " << double(ncross)/double(t) << endl;
}
