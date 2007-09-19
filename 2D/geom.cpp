#include <iostream>
#include <vector>
#include <vb/PRNG.h>

#define N 30
#define PI 4*atan(1.0)

using namespace std;
using namespace vb;

PRNG prng;
vector<double> o (N*N);

void geodesique (int i, int j, double x=0.0, double y=0.0, int c=0) {
  for (int step=0; step<200; ++step) {
    cout << (double)i + x << " " << (double)j + y << " " << c << endl;
    int ij = N*i+j;

    // Rotation
    
    int r=0;
    if (y==1) {
      r = 1;
      o[ij] += PI/2;
      y = x;
      x = 0;
    } else if (x==1) {
      r = 2;
      y = 1-y;
      x = 0;
    } else if (y==0) {
      r = 3;
      o[ij] -= PI/2;
      y = 1-x;
      x = 0;
    }

    // Avance

    int di=0, dj=0;

    double t = tan (o[N*i+j]);
    if (t < -y) {           // Vers le bas
      x = -y/t;
      y = 0;
      dj = -1;
    } else if (t < 1-y) {
      x = 1;
      y = y + t;
      di = 1;
    } else {
      x = (1-y)/t;
      y = 1;
      dj = 1;
    }
  
    // Rotation inverse
    
    if (r==1) {
      o[ij] -= PI/2;
      int ti = di;
      di = dj;
      dj = -ti;
      double tx = x;
      x = y;
      y = 1-tx;
    } else if (r==2) {
      di = -di;
      dj = -dj;
      x = 1-x;
      y = 1-y;
    } else if (r==3) {
      o[ij] += PI/2;
      int ti = di;
      di = -dj;
      dj = ti;
      double tx = x;
      x = 1-y;
      y = tx;
    }

    // Update

    x -= di; i += di;
    y -= dj; j += dj;

    // Break
    
    if ( (i<0) || (i>=N) || (j<0) || (j>=N) ) break;
  }
  cout << endl;
}

int main (int argc, char **argv) {
  for (int i=0; i<N*N; ++i) o[i] = 2*PI * prng.uniform();

  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      cerr << "(" << i << "," << j << ")   \r";
      int c = prng.uniform(1<<24);
      geodesique (i,j,0,.5,c);
      geodesique (i,j,1,.5,c);
    }
  }
}
