/*
 * Simulation d'une marche à boucles effacées dans Z^2, tuée à la
 * sortie d'un carré de côté donné.
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

#define EC fprintf(stderr,"<%d>\n",__LINE__);

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

long l;

char TRANS[] = "ENWS";

int main(int argc, char ** argv)
{
  Image *img;              /* L image */
  char title[80];          /* Titre ... */
  int n;                   /* Taille du carré */
  char d;                  /* La chaine */
  
  long i,x,y,done;

  int imin,imax,jmin,jmax;

  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=500");
  n = CLP('n');

  /* Initialisations */

  sprintf(title,"A loop-erased walk of size %d",n);
  img = new Image(2*n,2*n,title);
  if (!img) exit(1);

  /* Simulation */

  i=0; done=0; x=n; y=n;
  while (!done) {
    d = prng.rand()&3;
    img -> putpoint (x,y,d);
    x += dx[(int)d];
    y += dy[(int)d];
    /* fprintf(stderr,"%8ld\r",++i); */

    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }

  /* Crop */

  done=0; x=n; y=n; imin=n; imax=n; jmin=n; jmax=n;
  while (!done) {
    d = (*img)(x,y);
    x += dx[(int)d];
    y += dy[(int)d];

    if (x>imax) imax=x;
    if (x<imin) imin=x;
    if (y>jmax) jmax=y;
    if (y<jmin) jmin=y;

    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }  

  /* Affichage */

  printf ("%%!PS-Adobe-2.0 EPSF-2.0\n");
  printf ("%%%%Title: %s\n",title);
  printf ("%%%%Creator: LERW.c (VB)\n");
  printf ("%%%%BoundingBox: 0 0 %d %d\n", 3*(imax-imin)+6, 3*(jmax-jmin)+6);

  printf ("\nsave 20 dict begin /x %d def /y %d def\n", 3*(n-imin)+3, 3*(n-jmin)+3);
  printf ("/E {/x x 3 add def x y lineto} bind def\n");
  printf ("/N {/y y 3 add def x y lineto} bind def\n");
  printf ("/W {/x x 3 sub def x y lineto} bind def\n");
  printf ("/S {/y y 3 sub def x y lineto} bind def\n");
  printf ("newpath x y moveto");

  i=0; done=0; x=n; y=n; l=0;
  while (!done) {
    l++;
    d = (*img)(x,y);
    if (!((i++)%40)) printf ("\n");
    printf ("%c ", TRANS[(int)d]);
    x += dx[(int)d];
    y += dy[(int)d];
    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }  

  printf ("\nstroke end restore\n");

  fprintf (stderr, "%ld pas\n", l);

  return 0;
}
