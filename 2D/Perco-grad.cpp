
#include <math.h>
#include <vb.h>
#include <vb/CL_Parser.h>

using namespace vb;

int *fifox,*fifoy;

void cnx (Image *img, long ox, long oy, long in, long out)
{
  long imin,imax;
  long i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img->putpoint (ox,oy,in); 

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->width;
    imin++;
    if ((i<img->width-1)&&((*img)(k+1)!=in)&&((*img)(k+1)!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img->putpoint (i+1,j,in);
    }
    if ((i>0)&&((*img)(k-1)!=in)&&((*img)(k-1)!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img->putpoint (i-1,j,in);
    }
    if ((j<img->height-1)&&((*img)(k+img->width)!=in)&&((*img)(k+img->width)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img->putpoint (i,j+1,in);
    }
    if ((j>0)&&((*img)(k-img->width)!=in)&&((*img)(k-img->width)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img->putpoint (i,j-1,in);
    }
  }  
}

int main(int argc, char ** argv)
{
  CL_Parser clp (argc, argv, "n=500,p=0,q=1,W=0,H=0");
  int n = clp.as_int('n');
  int w = clp.as_int('W');
  int h = clp.as_int('H');
  double p1 = clp('p');
  double p2 = clp('q');

  if ((w*h==0) && (w+h!=0)) {
    std::cerr << "Please specify both width and height, or neither." << std::endl;
    exit(1);
  }

  if (w*h==0) { w=n; h=n; }

  char title[80];
  sprintf(title,"Gradient percolation cluster");

  Image *img = new Image(w,h,2,title);

  PRNG prng;

  /* Site-percolation --- 1=dedans, 2=dehors */

  long i;
  for (i=0;i<w*h;i++) {
    if ( prng.bernoulli( p1 + (p2-p1)*((double)i / (w*h)) )) 
      (*img)(i)=1;
    else
      (*img)(i)=2;
  }

  for (i=w*h-w;i<w*h;i++) {
    (*img)(i)=1;
  }

  img->show();

  /* 3=cluster de l'origine */

  fifox = new int[4*img->width*img->height];
  fifoy = new int[4*img->width*img->height];
  cnx(img,0,h-1,3,2);

  img->update();

  /* "Hull" du cluster */

  /* for (i=0;i<2*n;i++) if ((*img)(i)!=3)                     cnx(img,i,0,0,3); */
  /* for (i=0;i<2*n;i++) if ((*img)(img->width*i)!=3)             cnx(img,0,i,0,3); */
  /* for (i=0;i<2*n;i++) if ((*img)(img->width*(i+1)-1)!=3)       cnx(img,img->width-1,i,0,3); */
  /* for (i=0;i<2*n;i++) if ((*img)(img->width*(img->height-1)+i)!=3) cnx(img,i,img->height-1,0,3); */

  for (i=0;i<w*h;i++) if ((*img)(i)==2) (*img)(i)=0;

  /* affichage du resultat */

  std::cout << *img;
  return 0;
}
