/*
 * Interpolation  between edge percolation on  Z^2 (seen as
 * site-percolation on the  covering graph) and the asymmetrical
 * critical version thereof (i.e. p and 1-p)
 */

#include <iostream>
#include <time.h>

using std::cout;
using std::cerr;

#define EST 32
#define NORD 64

int width;
int height;

void do_perc (char **perc, double p) {
  int i,j;

  for (i=0; i<width; i++) {
    for (j=0; j<height; j++) {
      perc[i][j] = 0;
      if (drand48() < p)
	perc[i][j] += EST;
      if (drand48() < 1-p)
	perc[i][j] += NORD;
    }
  }
}

int traverse (char **perc) {
  static int *x;
  static int *y;
  int i,j,imin,imax;
  int found;

  // Init : on met la colonne 0 a 2

  if (x == NULL) {
    x = (int *) calloc (height*width, sizeof(int));
    y = (int *) calloc (height*width, sizeof(int));
  }

  imin=0; imax=-1;

  for (j=0;j<height;j++) {
      perc[0][j] += 2;
      imax++;
      x[imax] = 0;
      y[imax] = j;
  }

  // Exploration

  found = 0;

  while (imin<=imax) {
    i=x[imin];
    j=y[imin];
    imin++;

    if (i==width-1) {
      found=1;
      break;
    }

    if ( (i<width-1) && (perc[i][j]&EST) && !(perc[i+1][j]&2) ) {
      imax++;
      x[imax] = i+1;
      y[imax] = j;
      perc[i+1][j] += 2;
    }

    if ( (i>0) && (perc[i-1][j]&EST) && !(perc[i-1][j]&2) ) {
      imax++;
      x[imax] = i-1;
      y[imax] = j;
      perc[i-1][j] += 2;
    }

    if ( (j<height-1) && (perc[i][j]&NORD) && !(perc[i][j+1]&2) ) {
      imax++;
      x[imax] = i;
      y[imax] = j+1;
      perc[i][j+1] += 2;
    }

    if ( (j>0) && (perc[i][j-1]&NORD) && !(perc[i][j-1]&2) ) {
      imax++;
      x[imax] = i;
      y[imax] = j-1;
      perc[i][j-1] += 2;
    }
  }
  
  return found;
}
  

int main (int argc, char **argv) {
  char **perc;
  int i,npass;
  double step;

  // Initialisations

  if (argc == 5) {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    npass = atoi(argv[3]);
    step = atof(argv[4]);
  } else {
    cerr << argv[0] << " : wrong number of args\n";
    cerr << "Syntax : " << argv[0] << " width height npass\n";
    exit (1);
  }
    
  perc = (char **) calloc (width,sizeof (char *));
  for (i=0; i<width; i++) {
    perc[i] = (char *) calloc (height, sizeof (char));
  }

  srand48(time(0));

  // Boucle sur q

  for (double q=0; q<=1.001; q+=step) {
    int n=0;
    for (int count=0; count<npass; count++) {
      do_perc (perc,q);
      n += traverse(perc);
    }
    cout << q << " " << ((double)n) / ((double)npass) << "\n";
    cerr << q << " " << ((double)n) / ((double)npass) << "\n";
  }
}
