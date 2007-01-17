
#include <math.h>
#include <time.h>
#include <vb.h>

using namespace vb;

int *fifox,*fifoy;
char *t;

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
    if ((i<img->width-1)&&(t[k+1]!=in)&&(t[k+1]!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img->putpoint (i+1,j,in);
    }
    if ((i>0)&&(t[k-1]!=in)&&(t[k-1]!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img->putpoint (i-1,j,in);
    }
    if ((j<img->height-1)&&(t[k+img->width]!=in)&&(t[k+img->width]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img->putpoint (i,j+1,in);
    }
    if ((j>0)&&(t[k-img->width]!=in)&&(t[k-img->width]!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img->putpoint (i,j-1,in);
    }
  }  
}

int main(int argc, char ** argv)
{
  Image *img;
  long n,i,k;
  char title[80];
  double p1,p2;

  /* arguments -> n */

  if (argc != 4) {
    fprintf(stderr, "Syntaxe : %s <n> <p1> <p2>\n",argv[0]);
    exit(1);
  }
  n=atoi(argv[1]);
  p1=atof(argv[2]);
  p2=atof(argv[3]);

  sprintf(title,"Gradient percolation cluster");
  img=new Image(n,n,2,title);
  t=img->give_me_the_pic();
  srand48(time(0));

  /* Site-percolation --- 1=dedans, 2=dehors */

  for (i=0;i<n*n;i++) {
    k = (int)(drand48()*n*n);
    if ( k < p1*n*n + (p2-p1)*i ) 
      t[i]=1;
    else
      t[i]=2;
  }

  for (i=n*n-n;i<n*n;i++) {
    t[i]=1;
  }

  img->onscreen();
  t=img->give_me_the_pic();

  /* 3=cluster de l'origine */

  fifox = new int[4*img->width*img->height];
  fifoy = new int[4*img->width*img->height];
  cnx(img,0,n-1,3,2);

  img->update();

  /* "Hull" du cluster */

  /* for (i=0;i<2*n;i++) if (t[i]!=3)                     cnx(img,i,0,0,3); */
  /* for (i=0;i<2*n;i++) if (t[img->width*i]!=3)             cnx(img,0,i,0,3); */
  /* for (i=0;i<2*n;i++) if (t[img->width*(i+1)-1]!=3)       cnx(img,img->width-1,i,0,3); */
  /* for (i=0;i<2*n;i++) if (t[img->width*(img->height-1)+i]!=3) cnx(img,i,img->height-1,0,3); */

  for (i=0;i<n*n;i++) if (t[i]==2) t[i]=0;

  /* affichage du resultat */

  std::cout << *img;
  return 0;
}
