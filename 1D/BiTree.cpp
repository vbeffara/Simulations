#include <iostream>

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  CL_Parser CLP (argc, argv, "n=3,m=0,d=5,a=.4,t=10000");
  int n = CLP('n');             // Parameter for the underlying tree
  int m = CLP('m');             // Parameter for the additional trees
  int h = CLP('d');             // Depth of the additional trees
  double a = CLP('a');          // Drift of the walk (rate to go down)
  int t = CLP('t');             // Life time of the process
  
  double p1 = ((n+m)*a)/((n+m)*a+1); // Probability to go down if in the base tree
  double p2 = double(m)/double(m+n); // Conditional probability to enter an additional tree
  double q = (m*a)/(m*a+1);          // Probability to go down while in an additional tree
  
  int x=0;      // Distance from the root
  int y=0;                      // Distance from the root of the current additional tree

  for (int i=0; i<t; ++i) {
    cout << i << " " << x << " " << y << endl;
    if (y>0) {                  // If in an additional tree:
      if (prng.bernoulli(q)) {  //   Going down ?
        ++x; ++y; if (y==h) y=0;
      } else if (x>0) {         //   Going up ?
        --x; --y;
      }
    } else {                    // If in a base tree:
      if (prng.bernoulli(p1)) { //   Going down ?
        ++x;
        if (prng.bernoulli(p2)) ++y;
      } else if (x>0) {
        --x;
      }
    }
  }
}
