#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <time.h>
#include <vb.h>

using namespace vb;

#define EC fprintf(stderr,"<%d>\n",__LINE__);

int dx[4] = {1,0,-1,0};  /*  0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};
char dir[5] = "ENWS";
char prog[5] = "-\\|/";

/*  Puisque les headers ne marchent pas de toute façon ... */

int hash_function (int x, int y, int nmax)
{
  /*  Beurk ! */
  /*  Le mieux c'est (a*x+b*y)%nmax avec a,b,nmax premiers entre eux. */

  return (x*y)%nmax;
}

int is_self_avoiding (char *w, int n)
{
  static int *x_hash;
  static int *y_hash;

  int x,y,l,i,k,ok;
  
  if (!x_hash) {
    x_hash = new int[2*n];
    y_hash = new int[2*n];
  }
  bzero (x_hash,2*n*sizeof(int));
  bzero (y_hash,2*n*sizeof(int));
  
  x=n+1; y=n+(n<<1)+1; l=0; ok=1;
  k=hash_function(x,y,2*n);
  x_hash[k]=x;
  y_hash[k]=y;

  for (i=0;(i<n)&&(ok==1);i++) {
    l=(l+w[i])%4;
    x+=dx[l];
    y+=dy[l];
    k=hash_function(x,y,2*n);

    while ((ok)&&(x_hash[k]!=0)&&(y_hash[k]!=0)) {
      if ((x_hash[k]==x)&&(y_hash[k]==y)) ok=0;
      else k=(k+1)%(2*n);
    }

    x_hash[k]=x;
    y_hash[k]=y;
  }

  return ok;
}

int main(int argc, char ** argv)
{
  int n;                   /*  Nombre de pas */
  char *d;                 /*  La chaine */
  int t;                   /*  Durée de la simulation */
  char title[100];

  int i,j,k,done;
  char l=0;

  /* arguments -> nb de pas */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : SAW <n> <t>\n");
    exit(1);
  }
  n=atoi(argv[1]);
  t=atoi(argv[2]);

  /* Initialisations */

  srand48(time(0));
  d = new char[n];
  for (int i=0; i<n; i++)
    d[i]=0;
  done = 0;

  /* Simulation par Monte-Carlo */

  for (i=0;i<t;) {
    j = lrand48()%n;         /*  Position du pivot */
    k = 1+(lrand48()%3);     /*  Rotation = 1, 2 ou 3 mod 4 */
    l = d[j]; d[j]=(d[j]+k)%4;
    
    if (is_self_avoiding(d,n)) i++;
    else d[j]=l;
    
    if (!(i%128))
      fprintf (stderr,"\r%c (%d/%d)",prog[(i>>7)&3],i,t);
  }

  for (i=0;i<n;i++) {
    l=(l+d[i])%4;
    d[i]=l;
  }

  sprintf (title,"A self-avoiding walk of length %d",n);
  printout_path (d,n,title);

  return 0;
}
