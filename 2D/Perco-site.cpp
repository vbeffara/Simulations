
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
  t[ox+oy*img->pitch]=in; 

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->pitch;
    imin++;
    if ((i<img->pitch-1)&&(t[k+1]!=in)&&(t[k+1]!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      t[k+1]=in;
    }
    if ((i>0)&&(t[k-1]!=in)&&(t[k-1]!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      t[k-1]=in;
    }
    if ((j<img->height-1)&&(t[k+img->pitch]!=in)&&(t[k+img->pitch]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      t[k+img->pitch]=in;
    }
    if ((j>0)&&(t[k-img->pitch]!=in)&&(t[k-img->pitch]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      t[k-img->pitch]=in;
    }
  }  
}

int main(int argc, char ** argv)
{
  Image *img;
  int n,i;
  char title[80];
  double p;

  /* arguments -> p et n */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : Perco <p> <n>\n");
    exit(1);
  }
  p=atof(argv[1]);
  n=atoi(argv[2]);

  sprintf(title,"Site-percolation cluster (p=%f)",p);
  img=new Image(2*n,2*n,2,title);
  t=img->give_me_the_pic(); //FIXME
  srand48(time(0));

  /* Site-percolation --- 1=dedans, 2=dehors */

  for (i=0;i<4*n*n;i++) {
    if ( drand48() < p ) 
      t[i]=1;
    else
      t[i]=2;
  }

  /* 3=cluster de l'origine */

  fifox = new int [4*img->pitch*img->height];
  fifoy = new int [4*img->pitch*img->height];
  cnx(img,n,n,3,2);

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if (t[i]!=3)                     cnx(img,i,0,0,3);
  for (i=0;i<2*n;i++) if (t[img->pitch*i]!=3)             cnx(img,0,i,0,3);
  for (i=0;i<2*n;i++) if (t[img->pitch*(i+1)-1]!=3)       cnx(img,img->pitch-1,i,0,3);
  for (i=0;i<2*n;i++) if (t[img->pitch*(img->height-1)+i]!=3) cnx(img,i,img->height-1,0,3);

  for (i=0;i<4*n*n;i++) if (t[i]==2) t[i]=1;

  /* affichage du resultat */

  std::cout << *img;
  return 0;
}
