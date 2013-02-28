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
  for (int i=0; i<256; i++) ok[i]=1;
}

void init_ok_glass (void) {
   // Flippable iff at least two of the 4 neighbors are empty.
  for (int i=0; i<256; i++) {
    int tmp = 0;
    if (i&1) tmp++;
    if (i&4) tmp++;
    if (i&16) tmp++;
    if (i&64) tmp++;
    ok[i] = (tmp<=2 ? 1 : 0);
  }
}

void init_ok_connect4 (void) {
  /* This is connectivity-conditioning, on  the square lattice (which is
   * a bit awkward, in particular it is not ergodic for the wrong reason
   * that the lattice  and dual lattice look different  - the checkboard
   * configuration is fixed ...). */

  int tmp1[8],tmp2[8];

  for (int i=0; i<256; i++) {
    for (int j=0; j<8; j++) {
      tmp1[j] = (i&(1<<j)) / (1<<j);
      tmp2[j] = 1<<j;
    }

    if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
    if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];
    if ( (tmp1[4]==tmp1[6]) && (tmp1[4]==tmp1[5]) ) tmp2[6]=tmp2[4];
    if ( (tmp1[6]==tmp1[0]) && (tmp1[6]==tmp1[7]) ) tmp2[0]=tmp2[6];
    if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
    if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];

    int nb1 = (tmp1[0]*tmp2[0]) | (tmp1[2]*tmp2[2]) | (tmp1[4]*tmp2[4]) | (tmp1[6]*tmp2[6]);
    int nb2 = ((1-tmp1[0])*tmp2[0]) | ((1-tmp1[2])*tmp2[2]) | ((1-tmp1[4])*tmp2[4]) | ((1-tmp1[6])*tmp2[6]);

    nb1 = (nb1&1) + ((nb1&4)/4) + ((nb1&16)/16) + ((nb1&64)/64);
    nb2 = (nb2&1) + ((nb2&4)/4) + ((nb2&16)/16) + ((nb2&64)/64);

    ok[i] = ((nb1>1)||(nb2>1)  ? 0 : 1);
  }
}

void init_ok_connect6 (void) {
  // Connectivity-conditioning on the triangular lattice.

  int tmp[6];

  for (int i=0; i<256; i++) {
    tmp[0]=(i&1)/1;
    tmp[1]=(i&2)/2;
    tmp[2]=(i&4)/4;
    tmp[3]=(i&16)/16;
    tmp[4]=(i&32)/32;
    tmp[5]=(i&64)/64;

    int nb=0;
    if (tmp[1]!=tmp[0]) nb++;
    if (tmp[2]!=tmp[1]) nb++;
    if (tmp[3]!=tmp[2]) nb++;
    if (tmp[4]!=tmp[3]) nb++;
    if (tmp[5]!=tmp[4]) nb++;
    if (tmp[0]!=tmp[5]) nb++;

    ok[i] = (nb<=2 ? 1 : 0);
  }
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=300,p=.5,c=none",
      "Syntax: Glass [-n size] [-p p] [-c conditioning]\n\
      conditioning is one of: none (default), glass, connect4, connect6");
  int    n = CLP('n');
  double p = CLP('p');
  string c = CLP('c');

  if      (c == "none")     init_ok_none();
  else if (c == "glass")    init_ok_glass();
  else if (c == "connect4") init_ok_connect4();
  else if (c == "connect6") init_ok_connect6();
  else {
    cerr << "Error: conditioning '" << c << "' unknown!" << endl;
    exit (1);
  }

  Image img (n,n, str(fmt("A glass process of parameter %f (conditioning: %s)")%p%c));

  img.fill(0,0,0);
  for (int i=0; i<n; i++) img.putpoint (i,n/2,255);
  img.show();

  for (int i=2000*n*n; ; i--) {
    int x = 1 + (prng()%(n-2));
    int y = 1 + (prng()%(n-2));
    int nb = 0;
    if (img.at(x+1,y  ) != Color(0)) nb += 1;
    if (img.at(x+1,y+1) != Color(0)) nb += 2;
    if (img.at(x  ,y+1) != Color(0)) nb += 4;
    if (img.at(x-1,y+1) != Color(0)) nb += 8;
    if (img.at(x-1,y  ) != Color(0)) nb += 16;
    if (img.at(x-1,y-1) != Color(0)) nb += 32;
    if (img.at(x  ,y-1) != Color(0)) nb += 64;
    if (img.at(x+1,y-1) != Color(0)) nb += 128;

    if (ok[nb]) {
      if (prng.bernoulli(p)) {
	if (img.at(x,y) == Color(0)) {
          int tmp = img.at(x+1,y);
          tmp    |= img.at (x-1,y);
          tmp    |= img.at (x,y+1);
          tmp    |= img.at (x,y-1);
          if (tmp==0) tmp=85;
          img.putpoint (x,y,tmp);
        }
      } else img.putpoint(x,y,0);
    }
  }
  return 0;
}
