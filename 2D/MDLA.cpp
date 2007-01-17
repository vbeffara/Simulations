/*
 * DLA in a bath of exclusion particles.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vb.h>

#define EC fprintf(stderr,"<%d>\n",__LINE__);

using namespace vb;

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

int main(int argc, char ** argv)
{
  Image *img;              /* L image */
  char title[80];          /* Titre ... */
  int n;                   /* Taille du carré */
  char d;                  /* La chaine */
  double p;                /* Renforcement a 1+a */
  
  long x,y,nx,ny,done,ndraw,ngrey;

  /* arguments -> taille du terrain, densité */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : MDLA <n> <p>\n");
    exit(1);
  }
  n=atoi(argv[1]);
  p=atof(argv[2]);

  /* Initialisations */

  srand48(time(0));

  sprintf(title,"A MDLA in density %.2f",p);
  img = new Image(2*n,2*n,2,title);

  img->onscreen();

  ngrey=0;
  for (x=0;x<2*n;x++) {
    for (y=0;y<2*n;y++) {
      if (drand48()<p) { 
	img->putpoint (x,y,1);
	ngrey++;
      }
    }
  }
  img->putpoint (n,n,3);

  /* Simulation */

  done=0; ndraw=0;
  
  while (!done) {
    x = rand() % (2*n);
    y = rand() % (2*n);
    if ((*img)(x,y) == 1) {
      d = rand() % 4;
      nx = x + dx[(int)d];
      ny = y + dy[(int)d];
      if ((nx>=0)&&(ny>=0)&&(nx<2*n)&&(ny<2*n)) {
	switch ((*img)(nx,ny)) {
	case 0:
	  img->putpoint (x,y,0);
	  img->putpoint (nx,ny,1);
	  break;
	case 3:
	  img->putpoint (x,y,3);
	  ngrey--;
	  if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)||(ngrey==0)) done=1;
	  break;
	}
      }
    }
  }
  
  img->cropped = 1;
  std::cout << *img;
  return 0;
}
