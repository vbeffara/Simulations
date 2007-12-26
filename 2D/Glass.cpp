/*
 * Glass - conditioned metropolis Markov chain
 *
 * (contains Contact-connect as a particular case)
 */

#include <vb.h>
#include <unistd.h>
#include <string>

using namespace vb;
using namespace std;

char ok[256];

char title[100];

void init_ok_none (void) {
  /*
   * No conditioning at all, everything is authorized.
   */

  int i;

  for (i=0;i<256;i++)
    ok[i]=1;
}

void init_ok_glass (void) {
  /*
   * This is the usual glass model : flippable iff at least two of the 4
   * neighbors are empty.
   */

  int i,tmp;

  for (i=0;i<256;i++) {
    tmp = 0;
    if (i&1) tmp++;
    if (i&4) tmp++;
    if (i&16) tmp++;
    if (i&64) tmp++;
    if (tmp<=2) ok[i]=1;
    else ok[i]=0;
  }
}

void init_ok_connect4 (void) {
  /*
   * This is connectivity-conditioning, on  the square lattice (which is
   * a bit awkward, in particular it is not ergodic for the wrong reason
   * that the lattice  and dual lattice look different  - the checkboard
   * configuration id fixed ...).
   */

  int i,j,nb1,nb2;
  int tmp1[8],tmp2[8];

  for (i=0;i<256;i++) {
    for (j=0;j<8;j++) {
      tmp1[j] = (i&(1<<j))/(1<<j);
      tmp2[j] = 1<<j;
    }

    if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
    if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];
    if ( (tmp1[4]==tmp1[6]) && (tmp1[4]==tmp1[5]) ) tmp2[6]=tmp2[4];
    if ( (tmp1[6]==tmp1[0]) && (tmp1[6]==tmp1[7]) ) tmp2[0]=tmp2[6];
    if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
    if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];

    nb1 = (tmp1[0]*tmp2[0]) | (tmp1[2]*tmp2[2]) 
      | (tmp1[4]*tmp2[4]) | (tmp1[6]*tmp2[6]);
    nb2 = ((1-tmp1[0])*tmp2[0]) | ((1-tmp1[2])*tmp2[2]) 
      | ((1-tmp1[4])*tmp2[4]) | ((1-tmp1[6])*tmp2[6]);

    nb1 = (nb1&1) + ((nb1&4)/4) + ((nb1&16)/16) + ((nb1&64)/64);
    nb2 = (nb2&1) + ((nb2&4)/4) + ((nb2&16)/16) + ((nb2&64)/64);

    if ((nb1>1)||(nb2>1)) ok[i]=0;
    else ok[i]=1;
  }
}

void init_ok_connect6 (void) {
  /*
   * Connectivity-conditioning on the triangular lattice.
   */

  int tmp[6];
  int i,nb;

  for (i=0;i<256;i++) {
    tmp[0]=(i&1)/1;
    tmp[1]=(i&2)/2;
    tmp[2]=(i&4)/4;
    tmp[3]=(i&16)/16;
    tmp[4]=(i&32)/32;
    tmp[5]=(i&64)/64;

    nb=0;
    if (tmp[1]!=tmp[0]) nb++;
    if (tmp[2]!=tmp[1]) nb++;
    if (tmp[3]!=tmp[2]) nb++;
    if (tmp[4]!=tmp[3]) nb++;
    if (tmp[5]!=tmp[4]) nb++;
    if (tmp[0]!=tmp[5]) nb++;

    if (nb<=2) ok[i]=1;
    else ok[i]=0;
  }
}

int main (int argc, char **argv) {
  long x,y,i,nb;
  PRNG prng;

  CL_Parser CLP (argc,argv,"n=300,p=.5,c=none",
      "Syntax: Glass [-n size] [-p p] [-c conditioning]\n\
      conditioning is one of: none (default), glass, connect4, connect6");

  int n = CLP.as_int('n');
  double p = CLP.as_double('p');
  string c = CLP.as_string('c');
  
  if (c == "none") init_ok_none();
  else if (c == "glass") init_ok_glass();
  else if (c == "connect4") init_ok_connect4();
  else if (c == "connect6") init_ok_connect6();
  else {
    cerr << "Error: conditioning '" << c << "' unknown!" << endl;
    exit (1);
  }

  snprintf (title,99, "A glass process of parameter %g (conditioning: '%s')", p, c.c_str());
  Image img (n,n,1,title);

  for (x=0;x<n;x++)
    for (y=0;y<n;y++)
      if (prng.bernoulli(1-p))
	img.putpoint (x,y,1);

  for (i=0;i<n;i++) {
    img.putpoint (0,i,1);
    img.putpoint (i,0,1);
    img.putpoint (n-1,i,0);
    img.putpoint (i,n-1,0);
  }

  img.show ();

  for (i=2000*n*n; i>0; i--) {
    x = 1 + (prng.rand()%(n-2));
    y = 1 + (prng.rand()%(n-2));

    nb = img(x+1,y) + 2*img(x+1,y+1) + 4*img(x,y+1) + 8*img(x-1,y+1)
      + 16*img(x-1,y) + 32*img(x-1,y-1) + 64*img(x,y-1) + 128*img(x+1,y-1);

    if (ok[nb]) {
      if (prng.bernoulli(p)) {
	if (img(x,y)==0) img.putpoint (x,y,1);
      } else {
	if (img(x,y)==1) img.putpoint (x,y,0);
      }
    }
  }

  std::cout << img;

  return 0;
}
