
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int *fifox,*fifoy;

void cnx (Image &img, int ox, int oy, Color in, Color out)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img.putpoint (ox,oy,in);

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img.w();
    imin++;
    if ((i<img.w()-1)&&(img.at(i+1,j)!=in)&&(img.at(i+1,j)!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img.putpoint (i+1,j,in);
    }
    if ((i>0)&&(img.at(i-1,j)!=in)&&(img.at(i-1,j)!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img.putpoint (i-1,j,in);
    }
    if ((j<img.h()-1)&&(img.at(i,j+1)!=in)&&(img.at(i,j+1)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img.putpoint (i,j+1,in);
    }
    if ((j>0)&&(img.at(i,j-1)!=in)&&(img.at(i,j-1)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img.putpoint (i,j-1,in);
    }
  }
}

void bndcnx (Image &img, int ox, int oy)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img.putpoint (ox,oy, img.at(ox,oy) | 3);

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img.w();
    imin++;
    if ((i<img.w()-1)&&(img.at(i,j)&16)&&!(img.at(i+1,j)&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img.putpoint (i+1,j, img.at(i+1,j) | 3);
    }
    if ((j<img.h()-1)&&(img.at(i,j)&32)&&!(img.at(i,j+1)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img.putpoint (i,j+1, img.at(i,j+1) | 3);
    }
    if ((i>0)&&(img.at(i-1,j)&16)&&!(img.at(i-1,j)&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img.putpoint (i-1,j, img.at(i-1,j) | 3);
    }
    if ((j>0)&&(img.at(i,j-1)&32)&&!(img.at(i,j-1)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img.putpoint (i,j-1, img.at(i,j-1) | 3);
    }
  }
}

int main(int argc, char ** argv)
{
  int n,i;
  double p;

  /* arguments -> p et n */

  CL_Parser CLP (argc,argv,"n=500,p=.5");
  n = CLP('n');
  p = CLP('p');

  Image img (2*n,2*n, str(fmt("Bond-percolation cluster (p=%)")%p));

  /* Bond-percolation --- 16->est, 32->nord */

  for (int x=0;x<2*n;++x)
    for (int y=0;y<2*n;++y) {
      if ( prng.bernoulli(p) ) img.putpoint (x,y,17); else img.putpoint (x,y,1);
      if ( prng.bernoulli(p) ) img.putpoint (x,y,img.at(x,y)+32);
    }

  /* 3=cluster de l'origine */

  fifox = new int [4*img.w()*img.h()];
  fifoy = new int [4*img.w()*img.h()];
  bndcnx(img,n,n);

  /* Oublier le réseau */

  for (int x=0;x<2*n;++x)
    for (int y=0;y<2*n;++y)
      img.putpoint (x,y, img.at(x,y)&3);

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if (img.at(i,0)!=Color(3))
                        cnx(img,i,0,0,3);
  for (i=0;i<2*n;i++) if (img.at(0,i)!=Color(3))
                        cnx(img,0,i,0,3);
  for (i=0;i<2*n;i++) if (img.at(2*n-1,i)!=Color(3))
                        cnx(img,img.w()-1,i,0,3);
  for (i=0;i<2*n;i++) if (img.at(i,2*n-1)!=Color(3))
                        cnx(img,i,img.h()-1,0,3);

  /* affichage du resultat */

  return 0;
}
