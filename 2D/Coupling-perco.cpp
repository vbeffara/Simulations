/*
 * Coupling two realizations of percolation in an annulus, both conditioned to
 * have 4  arms and  not 6, but  conditioned to  touch different sides  of the
 * boundary with those 4  arms. Aim = they should more or  less agree inside a
 * small disk.
 */

#define RAD1 10
#define RAD2 20
#define RAD3 40

#define N (2*RAD3)

#include <vb.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef int testfunction (char*);

using namespace vb;

int compute_diff (char *c1, char *c2, Image *d) {
  int i,j,n;

  n=0;
  for (i=0;i<N;i++) {
    for (j=0;j<N;j++) {
      if (c1[i+N*j]==c2[i+N*j]) {
	d->putpoint(i,j,0);
      } else if (c1[i+N*j]>c2[i+N*j]) {
	d->putpoint(i,j,2);
	n++;
      } else {
	d->putpoint(i,j,3);
	n++;
      }
    }
  }
  return n;
}

void compute_cpts (char *c, int *cpts, int r1, int r2) {
  /*
   * Determine the connected  components in the annulus of  radii r1<r2 around
   * the center of the square, and put that in *cpts. 
   */

  int x,y,t,dirty;

  /*  initialisation */

  t=0;
  for (x=0;x<N;x++) {
    for (y=0;y<N;y++) {
      if (c[x+N*y]==1) {
	cpts[x+N*y]=(++t);
      } else {
	cpts[x+N*y]=-(++t);
      }
    }
  }

  /*  hole in the middle ie for r<r1 */

  for (x=-r1+1;x<r1-1;x++)
    for (y=-r1+1;y<r1-1;y++)
      cpts[(N>>1)+x+N*((N>>1)+y)]=0;

  /*  scan; on garde le plus grand |.| indice de chaque cluster */

  dirty=1;
  while (dirty==1) {
    dirty = 0;
    for (x=0;x<N;x++) {
      for (y=0;y<N;y++) {
	if ( (cpts[x+N*y]>0) && (x>0) && (cpts[x-1+N*y]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x-1+N*y]; dirty=1; }
	if ( (cpts[x+N*y]>0) && (x<N-1) && (cpts[x+1+N*y]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+1+N*y]; dirty=1; }
	if ( (cpts[x+N*y]>0) && (y>0) && (cpts[x+N*y-N]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+N*y-N]; dirty=1; }
	if ( (cpts[x+N*y]>0) && (y<N-1) && (cpts[x+N*y+N]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+N*y+N]; dirty=1; }
	if ( (cpts[x+N*y]>0) && (x>0) && (y>0) && (cpts[x-1+N*y-N]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x-1+N*y-N]; dirty=1; }
	if ( (cpts[x+N*y]>0) && (x<N-1) && (y<N-1) && (cpts[x+1+N*y+N]>cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+1+N*y+N]; dirty=1; }

	if ( (cpts[x+N*y]<0) && (x>0) && (cpts[x-1+N*y]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x-1+N*y]; dirty=1; }
	if ( (cpts[x+N*y]<0) && (x<N-1) && (cpts[x+1+N*y]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+1+N*y]; dirty=1; }
	if ( (cpts[x+N*y]<0) && (y>0) && (cpts[x+N*y-N]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+N*y-N]; dirty=1; }
	if ( (cpts[x+N*y]<0) && (y<N-1) && (cpts[x+N*y+N]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+N*y+N]; dirty=1; }
	if ( (cpts[x+N*y]<0) && (x>0) && (y>0) && (cpts[x-1+N*y-N]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x-1+N*y-N]; dirty=1; }
	if ( (cpts[x+N*y]<0) && (x<N-1) && (y<N-1) && (cpts[x+1+N*y+N]<cpts[x+N*y]) ) { cpts[x+N*y]=cpts[x+1+N*y+N]; dirty=1; }
      }
    }
  }
}

int nbarms (char *c, int r1, int r2, int sides) {
  /*
   * Compute the number of connected  components spanning from r1 to r2 inside
   * the annulus  (should be 0, 1  or even).
   */

  int n,k,i;
  static char *table = NULL;
  static int *expl = NULL;

  if (table == NULL) table = new char[2*N*N+1];
  if (expl == NULL) expl = new int[N*N];

  compute_cpts (c,expl,r1,r2);

  for (i=0;i<2*N*N+1;i++) table[i]=0;

  for (i=-r2;i<r2;i++) {
    if (sides&1) table[N*N+expl[(N>>1)+i+N*((N>>1)-r2)]]=1;
    if (sides&2) table[N*N+expl[(N>>1)+i+N*((N>>1)+r2-1)]]=1;
    if (sides&4) table[N*N+expl[(N>>1)-r2+N*((N>>1)+i)]]=1;
    if (sides&8) table[N*N+expl[(N>>1)+r2-1+N*((N>>1)+i)]]=1;
  }

  n=0;
  for (i=-r1;i<r1;i++) {
    k = expl[(N>>1)+i + N*((N>>1)-r1)];
    if (table[N*N+k]==1) {
      table[N*N+k]=0;
      n++;
    }
    k = expl[(N>>1)+i + N*((N>>1)+r1-1)];
    if (table[N*N+k]==1) {
      table[N*N+k]=0;
      n++;
    }
    k = expl[(N>>1)-r1 + N*((N>>1)+i)];
    if (table[N*N+k]==1) {
      table[N*N+k]=0;
      n++;
    }
    k = expl[(N>>1)+r1-1 + N*((N>>1)+i)];
    if (table[N*N+k]==1) {
      table[N*N+k]=0;
      n++;
    }
  }

  return n;
}

int test1 (char *c) {
  /*
   * 4 bras entre RAD1 et RAD3.
   * 4 entre RAD1 et RAD2 et entre RAD2 et RAD3 i.e. jamais 6.
   */

  if (nbarms(c,RAD1,RAD3,15)!=4) return 0;
  if (nbarms(c,RAD2,RAD3,15)!=4) return 0;
  if (nbarms(c,RAD1,RAD2,15)!=4) return 0;
  return 1;
}

int test2 (char *c) {
  /*
   * test 1 plus exactement un bras sur le cote 1 entre RAD2 et RAD3.
   */

  if ( (test1(c)==1) && (nbarms(c,RAD1,RAD3,6)==2) ) return 1;
  return 0;
}

void monte_carlo (char *c1, char *c2, Image *diff, int duration) {
  int t,x,y,good,err;
  char old,z;

  /*  le point a essayer */

  for (t=0;t<duration;t++) {
    good=0;
    while (good==0) {
      x = lrand48()%N;
      y = lrand48()%N;
      z = lrand48()%2;
      if ((x<(N>>1)-RAD1) || (x>=(N>>1)+RAD1) || (y<(N>>1)-RAD1) || (y>=(N>>1)+RAD1)) good=1;
    }

    /*  config 1 : juste cond. a 4 bras entre r1 et r3=(N>>1)=N/2 */

    old = c1[x+y*N];
    c1[x+y*N]=z;
    if (test1(c1)==0) c1[x+y*N]=old;

    /*  config 1 : juste cond. a 4 bras entre r1 et r3=(N>>1)=N/2 */

    old = c2[x+y*N];
    c2[x+y*N]=z;
    if (test2(c2)==0) c2[x+y*N]=old;

    /*  affichage de la difference */

    err = compute_diff (c1,c2,diff);
    fprintf (stderr, "%d\n", err);
    diff->update();
  }
}

void pick (Image *c, testfunction test) {
  int good,x,y,n;

  good=0;
  n=0;
  
  while (good==0) {
    fprintf (stderr,"%d \r",++n);
    for (x=0;x<N;x++)
      for (y=0;y<N;y++)
	c->putpoint (x,y,(lrand48()%2));

    for (x=-RAD1+1;x<RAD1-1;x++) {
      for (y=-RAD1+1;y<RAD1-1;y++) {
	c->putpoint ((N>>1)+x,(N>>1)+y,0);
	c->putpoint ((N>>1)+x,(N>>1)+y,0);
      }
    }

    good = test (c->give_me_the_pic());
  }
  fprintf (stderr,"\n");
}

int main (int argc, char ** argv) {
  int i;
  Image *img,*img1,*img2;

  img1 = new Image (N,N,1,"The first configuration");
  img2 = new Image (N,N,1,"The second configuration");
  img = new Image(N,N,2,"The difference");

  srand48(time(0));

  pick (img1,test2);
  for (i=0;i<N*N;i++) (*img2)(i) = (*img1)(i);

  img->onscreen();
  compute_diff(img1->give_me_the_pic(),img2->give_me_the_pic(),img);
  img->update();

  monte_carlo (img1->give_me_the_pic(),img2->give_me_the_pic(),img,N*N*N);

  return 0;
}
