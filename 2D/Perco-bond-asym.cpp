
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int *fifox,*fifoy;
Image *img;

void cnx (int ox, int oy, Color in, Color out)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img -> putpoint (ox,oy,in); 

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&((*img)(k+1)!=in)&&((*img)(k+1)!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img -> putpoint (k+1,0,in);
    }
    if ((i>0)&&((*img)(k-1)!=in)&&((*img)(k-1)!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img -> putpoint (k-1,0,in);
    }
    if ((j<img->height-1)&&((*img)(k+img->width)!=in)&&((*img)(k+img->width)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img -> putpoint (k,1,in);
    }
    if ((j>0)&&((*img)(k-img->width)!=in)&&((*img)(k-img->width)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img -> putpoint (k,-1,in);
    }
  }  
}

void bndcnx (int ox, int oy)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img -> putpoint (ox,oy, (*img)(ox,oy) | 3);

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&((*img)(k)&16)&&!((*img)(k+1)&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img->putpoint (k+1,0, (*img)(k,1) | 3);
    }
    if ((j<img->height-1)&&((*img)(k)&32)&&!((*img)(k+img->width)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img -> putpoint (k,1, (*img)(k,1) | 3);
    }
    if ((i>0)&&((*img)(k-1)&16)&&!((*img)(k-1)&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img -> putpoint (k-1,0, (*img)(k-1,0) | 3);
    }
    if ((j>0)&&((*img)(k-img->width)&32)&&!((*img)(k-img->width)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img -> putpoint (k,-1, (*img)(k,-1) | 3);
    }
  }  
}

int main(int argc, char ** argv)
{
  int n,i;
  char title[80];
  double p1, p2;

  /* arguments -> p et n */

  CL_Parser CLP (argc,argv,"n=500,p=.6,q=.4");
  n = CLP('n');
  p1 = CLP('p');
  p2 = CLP('q');

  sprintf(title,"Bond-percolation cluster (p1=%f, p2=%f)",p1,p2);
  img = new Image(2*n,2*n,title);

  /* Bond-percolation --- 16->est, 32->nord */

  for (i=0;i<4*n*n;i++) {
    if ( prng.bernoulli(p1) ) img -> putpoint (i,0,17); else img -> putpoint (i,0,1);
    if ( prng.bernoulli(p2) ) img -> putpoint (i,0, (*img)(i,0) + 32);
  }

  /* 3=cluster de l'origine */

  fifox = new int [4*img->width*img->height];
  fifoy = new int [4*img->width*img->height];
  bndcnx(n,n);

  /* Oublier le r�seau */

  for (i=0;i<4*n*n;i++) img -> putpoint (i,0, (*img)(i,0) & 3);

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if ((*img)(i)!=Color(3))                            cnx(i,0,0,3);
  for (i=0;i<2*n;i++) if ((*img)(img->width*i)!=Color(3))                 cnx(0,i,0,3);
  for (i=0;i<2*n;i++) if ((*img)(img->width*(i+1)-1)!=Color(3))           cnx(img->width-1,i,0,3);
  for (i=0;i<2*n;i++) if ((*img)(img->width*(img->height-1)+i)!=Color(3)) cnx(i,img->height-1,0,3);

  /* affichage du resultat */

  return 0;
}