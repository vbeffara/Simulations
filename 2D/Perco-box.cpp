
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <iostream>

using namespace std;
using namespace vb;

int *fifox,*fifoy;
Image *img;

void bndcnx (int ox, int oy) {
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img -> put (coo(ox,oy), (*img).at(coo(ox,oy)) | 3);

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->w();
    imin++;
    if ((i<img->w()-1)&&((*img).at(coo(i,j))&16)&&!((*img).at(coo(i+1,j))&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img -> put (coo(i+1,j), (*img).at(coo(i+1,j)) | 3);
    }
    if ((j<img->h()-1)&&((*img).at(coo(i,j))&32)&&!((*img).at(coo(i,j+1))&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img -> put (coo(i,j+1), (*img).at(coo(i,j+1)) | 3);
    }
    if ((i>0)&&((*img).at(coo(i-1,j))&16)&&!((*img).at(coo(i-1,j))&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img -> put (coo(i-1,j), (*img).at(coo(i-1,j)) | 3);
    }
    if ((j>0)&&((*img).at(coo(i,j-1))&32)&&!((*img).at(coo(i,j-1))&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img -> put (coo(i,j-1), (*img).at(coo(i,j-1)) | 3);
    }
  }
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=100,m=0,t=1,p=.5,q=0");
  int n = CLP('n');
  int m = CLP('m');
  int t = CLP('t');
  double p = CLP('p');
  double q = CLP('q');

  if (m==0) m=n;
  if (q==0) q=p;

  img = new Image (n,m,"Bond-percolation");
  fifox = new int[m*n];
  fifoy = new int[m*n];

  int ncross = 0;

  { ProgressBar PB (t);

    for (int iter=0; iter<t; ++iter) {
      PB.set (iter);

      for (int i=0; i<n; ++i)
        for (int j=0; j<m; ++j)
          img -> put (coo(i,j), 16*prng.bernoulli(p) + 32*prng.bernoulli(q));

      for (int j=0; j<m; ++j) {
        img -> put (coo(0,j), (*img).at(coo(0,j)) | 32);
        img -> put (coo(n-1,j), (*img).at(coo(n-1,j)) | 32);
      }

      bndcnx (0,0);
      if ((*img).at(coo(n-1,0)) & 3) ++ncross;
    }
  }

  cout << m << " " << n << " " << p << " " << q << " | " << t << " | " << ncross << " " << double(ncross)/double(t) << endl;
}
