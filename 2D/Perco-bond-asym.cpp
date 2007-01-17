
#include <math.h>
#include <time.h>
#include <vb.h>

using namespace vb;

int *fifox,*fifoy;
char *t;

void cnx (Image *img, int ox, int oy, char in, char out)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  t[ox+oy*img->width]=in; 

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&(t[k+1]!=in)&&(t[k+1]!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      t[k+1]=in;
    }
    if ((i>0)&&(t[k-1]!=in)&&(t[k-1]!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      t[k-1]=in;
    }
    if ((j<img->height-1)&&(t[k+img->width]!=in)&&(t[k+img->width]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      t[k+img->width]=in;
    }
    if ((j>0)&&(t[k-img->width]!=in)&&(t[k-img->width]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      t[k-img->width]=in;
    }
  }  
}

void bndcnx (Image *img, int ox, int oy)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  t[ox+oy*img->width] |= 3;

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&(t[k]&16)&&!(t[k+1]&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      t[k+1]|=3;
    }
    if ((j<img->height-1)&&(t[k]&32)&&!(t[k+img->width]&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      t[k+img->width]|=3;
    }
    if ((i>0)&&(t[k-1]&16)&&!(t[k-1]&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      t[k-1]|=3;
    }
    if ((j>0)&&(t[k-img->width]&32)&&!(t[k-img->width]&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      t[k-img->width]|=3;
    }
  }  
}

int main(int argc, char ** argv)
{
  Image *img;
  int n,i;
  char title[80];
  double p1, p2;

  /* arguments -> p et n */

  if (argc != 4) {
    fprintf(stderr, "Syntaxe : Perco <p1> <p2> <n>\n");
    exit(1);
  }
  p1=atof(argv[1]);
  p2=atof(argv[2]);
  n=atoi(argv[3]);

  sprintf(title,"Bond-percolation cluster (p1=%f, p2=%f)",p1,p2);
  img = new Image(2*n,2*n,1,title);
  t=img->give_me_the_pic();
  srand48(time(0));

  /* Bond-percolation --- 16->est, 32->nord */

  for (i=0;i<4*n*n;i++) {
    if ( drand48() < p1 ) t[i]=17; else t[i]=1;
    if ( drand48() < p2 ) t[i]+=32;
  }

  /* 3=cluster de l'origine */

  fifox = new int [4*img->width*img->height];
  fifoy = new int [4*img->width*img->height];
  bndcnx(img,n,n);

  /* Oublier le réseau */

  for (i=0;i<4*n*n;i++) t[i]&=3;

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if (t[i]!=3)                     cnx(img,i,0,0,3);
  for (i=0;i<2*n;i++) if (t[img->width*i]!=3)             cnx(img,0,i,0,3);
  for (i=0;i<2*n;i++) if (t[img->width*(i+1)-1]!=3)       cnx(img,img->width-1,i,0,3);
  for (i=0;i<2*n;i++) if (t[img->width*(img->height-1)+i]!=3) cnx(img,i,img->height-1,0,3);

  /* affichage du resultat */

  std::cout << *img;
  return 0;
}
