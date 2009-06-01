
#include <vb.h>

#define MAIN_BIT 1
#define L1_BIT 2
#define L2_BIT 4
#define R1_BIT 8
#define R2_BIT 16
#define ALL_BITS 31

using namespace vb;

PRNG prng;

class Automaton {
public:
  Automaton (int);
  ~Automaton ();

  void swap() { char *tmp=main; main=alt; alt=tmp; };

  void randomize (double);
  void shift ();
  void emit ();
  void forget (double r);
  void effect (double r);

  int size;
  char *main;
  char *alt;
};

Automaton::Automaton (int n) : 
  size(n), main (new char[n]), alt (new char[n]) {
  for (int i=0; i<size; ++i)
    main[i]=0;
}

Automaton::~Automaton () {
  delete[] alt;
  delete[] main;
}

void Automaton::randomize (double e) {
  for (int i=0; i<size; ++i)
    if (prng.bernoulli(e))
      main[i] = prng.rand() & ALL_BITS;
}

void Automaton::shift () {
  for (int i=0; i<size; ++i)
    alt[i] = (main[i]&MAIN_BIT) 
      + (main[(i+size-1)%size]&R1_BIT)
      + (main[(i+size-1)%size]&R2_BIT)
      + (main[(i+1)%size]&L1_BIT)
      + (main[(i+1)%size]&L2_BIT);
  this->swap();
}

void Automaton::emit () {
  for (int i=0; i<size; i++) {
    alt[i] = main[i];
    if ( (main[i]&1) && !(main[(i+size-1)%size]&1) ) alt[i] |= L1_BIT|R1_BIT;
    if ( !(main[i]&1) && (main[(i+size-1)%size]&1) ) alt[i] |= L2_BIT|R2_BIT;
  }
  this->swap();
}

void Automaton::forget (double r) {
  for (int i=0; i<size; ++i)
    if (prng.bernoulli(r))
      main[i] &= 1;
}

void Automaton::effect (double r) {
  for (int i=0; i<size; i++)
    alt[i] = main[i];
  for (int i=0; i<size; i++) {
    if ( !(main[i]&1) && (main[(i+size-1)%size]&1) &&
	 (main[i]&L1_BIT) && (prng.bernoulli(r)) )
      alt[i] |= 1;
    else if ( !(main[i]&1) && (main[(i+size-1)%size]&1) &&
	 (main[i]&R1_BIT) && (prng.bernoulli(r)) )
      alt[(i+size-1)%size] &= 30;
    else if ( (main[i]&1) && !(main[(i+size-1)%size]&1) &&
	 (main[i]&L2_BIT) && (prng.bernoulli(r)) )
      alt[i] &= 30;
    else if ( (main[i]&1) && !(main[(i+size-1)%size]&1) &&
	 (main[i]&R2_BIT) && (prng.bernoulli(r)) )
      alt[(i+size-1)%size] |= 1;
  }
  this->swap();
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=500,e=.03,r=.05");
  const int n = CLP('n');
  const double e = CLP('e');
  const double r = CLP('r');

  Image img (n,n,1,"Cellular automaton");
  img.show();

  Automaton a(n);

  for (int x=0; x<n; ++x)
    a.main[x] = prng.rand()&31;

  for (int i=0; ; ++i) {
    int nb=0;
    for (int x=0; x<n; ++x) {
      img.putpoint (x,i%n,(a.main[x]>>0)&1);
      nb += a.main[x]&1;
    }
    //    img.update();
    //std::cerr << nb << std::endl;
    for (int j=0; j<100; ++j) {
      a.randomize(e);
      a.emit();
      a.shift();
      a.effect(1);
      a.forget(r);
    }
  }

  return 0;
}
