/*
 * Interpolation   between   edge   percolation   on  Z^2   (seen   as
 * site-percolation on the covering graph) and site-percolation on the
 * centered square lattice.
 *
 *  Remarque :  pour un  rapport  d'aspect de  1/2, on  trouve par  la
 * formule de Cardy une  proba de traverser de l'ordre de 0.1756468938
 * ou 0.8243531062 selon.
 */

#include <iostream>
#include <time.h>
#include <stdlib.h>

using std::cout;
using std::cerr;

int width;
int height;

void init_lattice (double **l, double q) {
  int i,j;

  for (i=0; i<width; i+=2) {
    for (j=0; j<height; j+=2) {
      l[i][j] = 0.5;
    }
  }
  for (i=1; i<width; i+=2) {
    for (j=1; j<height; j+=2) {
      if ((i+j)%4 == 2) {
	l[i][j] = q;
      } else {
	l[i][j] = 1-q;
      }
    }
  }
}

void do_perc (int **perc, double **lattice) {
  int i,j;

  for (i=0; i<width; i++) {
    for (j=0; j<height; j++) {
      if (drand48() < lattice[i][j]) {
	perc[i][j] = 1;
	//cerr << "*";
      } else {
	perc[i][j] = 0;
	//cerr << ".";
      }
    }
    //cerr << "\n";
  }
}

int traverse (int **perc) {
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
    if (perc[0][j] == 1) {
      perc[0][j] = 2;
      imax++;
      x[imax] = 0;
      y[imax] = j;
    }
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

    if ( ((i%2)==0) && ((j%2)==0) ) { // Sur le reseau carre pair
      if ((i+2<width) && perc[i+2][j]==1) {
	imax++;
	x[imax] = i+2;
	y[imax] = j;
	perc[i+2][j] = 2;
      }
      if ((i-2>=0) && perc[i-2][j]==1) {
	imax++;
	x[imax] = i-2;
	y[imax] = j;
	perc[i-2][j] = 2;
      }
      if ((j+2<height) && perc[i][j+2]==1) {
	imax++;
	x[imax] = i;
	y[imax] = j+2;
	perc[i][j+2] = 2;
      }
      if ((j-2>=0) && perc[i][j-2]==1) {
	imax++;
	x[imax] = i;
	y[imax] = j-2;
	perc[i][j-2] = 2;
      }
    }
    
    if (i>0) {
      if ((j>0) && (perc[i-1][j-1]==1)) {
	imax++;
	x[imax] = i-1;
	y[imax] = j-1;
	perc[i-1][j-1] = 2;
      }
      if ((j+1<height) && (perc[i-1][j+1]==1)) {
	imax++;
	x[imax] = i-1;
	y[imax] = j+1;
	perc[i-1][j+1] = 2;
      }
    }

    if (i+1<width) {
      if ((j>0) && (perc[i+1][j-1]==1)) {
	imax++;
	x[imax] = i+1;
	y[imax] = j-1;
	perc[i+1][j-1] = 2;
      }
      if ((j+1<height) && (perc[i+1][j+1]==1)) {
	imax++;
	x[imax] = i+1;
	y[imax] = j+1;
	perc[i+1][j+1] = 2;
      }
    }
  }
  
  return found;
}
  

int main (int argc, char **argv) {
  double ** lattice;
  int ** perc;
  int i,npass;

  // Initialisations

  if (argc == 4) {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    npass = atoi(argv[3]);
  } else {
    cerr << argv[0] << " : wrong number of args\n";
    cerr << "Syntax : " << argv[0] << " width height npass\n";
    exit (1);
  }
    
  lattice = (double **) calloc (width,sizeof (double *));
  for (i=0; i<width; i++) {
    lattice[i] = (double *) calloc (height, sizeof (double));
  }

  perc = (int **) calloc (width,sizeof (int *));
  for (i=0; i<width; i++) {
    perc[i] = (int *) calloc (height, sizeof (int));
  }

  srand48(time(0));

  // Boucle sur q

  for (double q=0; q<=1.001; q+=0.01) {
    int n=0;
    init_lattice (lattice, q);
    for (int count=0; count<npass; count++) {
      do_perc (perc,lattice);
      n += traverse(perc);
    }
    cout << q << " " << ((double)n) / ((double)npass) << "\n";
    cerr << q << " " << ((double)n) / ((double)npass) << "\n";
  }
}
