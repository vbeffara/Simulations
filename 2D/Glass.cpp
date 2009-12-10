/*
 * Glass - conditioned metropolis Markov chain
 *
 * (contains Contact-connect as a particular case)
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

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
  long x,y,i;
  PRNG prng;

  CL_Parser CLP (argc,argv,"n=300,p=.5,c=none",
      "Syntax: Glass [-n size] [-p p] [-c conditioning]\n\
      conditioning is one of: none (default), glass, connect4, connect6");

  int n = CLP('n');
  double p = CLP('p');
  string c = CLP('c');
  
  if (c == "none") init_ok_none();
  else if (c == "glass") init_ok_glass();
  else if (c == "connect4") init_ok_connect4();
  else if (c == "connect6") init_ok_connect6();
  else {
    cerr << "Error: conditioning '" << c << "' unknown!" << endl;
    exit (1);
  }

  snprintf (title,99, "A glass process of parameter %g (conditioning: '%s')", p, c.c_str());
  Image img (n,n,2,title);

  for (i=0;i<n;i++) {
    img.putpoint (i,n/2,3);
  }

  img.show ();

  for (i=2000*n*n; ; i--) {
    x = 1 + (prng.rand()%(n-2));
    y = 1 + (prng.rand()%(n-2));

    int nb = 0;
    if (img(x+1,y)) nb += 1; 
    if (img(x+1,y+1))  nb += 2; 
    if (img(x,y+1))  nb += 4; 
    if (img(x-1,y+1)) nb += 8; 
    if (img(x-1,y))  nb += 16; 
    if (img(x-1,y-1))  nb += 32; 
    if (img(x,y-1))  nb += 64; 
    if (img(x+1,y-1)) nb += 128; 

    if (ok[nb]) {
      if (prng.bernoulli(p)) {
	if (img(x,y)==0) {
          int tmp = img(x+1,y);
          tmp |= img (x-1,y);
          tmp |= img (x,y+1);
          tmp |= img (x,y-1);
          if (tmp==0) tmp=1;
          img.putpoint (x,y,tmp);
        }
      } else {
	if (img(x,y)>0) img.putpoint (x,y,0);
      }
    }
  }

  return 0;
}
