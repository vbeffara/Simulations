/*
 * Excited random walk in the plane.
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>

int dx[4] = {1,0,-1,0};
int dy[4] = {0,1,0,-1};

using namespace vb;

class ERW : public CoarseImage {
public:
  double p,q;

  ERW (int n, CL_Parser CLP) : CoarseImage (n,n, str(fmt("An excited random walk (size=%d)") % n), pow(n,.33)), p(double(CLP('p'))), q(double(CLP('q'))) {};

  void run () {
    int x=0, y=0, n=true_width;

    while ((x>=-n/2)&&(y>=-n/2)&&(x<n)&&(y<n)) {
      int first = 1 - at(x+n/2,y+n/2);
      putpoint (x+n/2,y+n/2,1);

      if ((first == 1) && (prng.bernoulli(p))) {
        if (abs(x)==abs(y)) {
          if ((x==y)  && (x>0)) y--;
          if ((x==-y) && (x>0)) if (prng.bernoulli(q)) x--;
          if ((x==y)  && (x<0)) y++;
          if ((x==-y) && (x>0)) if (prng.bernoulli(q)) x++;
        } else {
          if (x>abs(y))  if (prng.bernoulli(q)) --x;
          if (x<-abs(y)) if (prng.bernoulli(q)) ++x;
          if (y>abs(x))  --y;
          if (y<-abs(x)) ++y;
        }
      } else {
        int d = prng()&3;
        x += dx[d]; y += dy[d];
      }
    }
  }
};

int main(int argc, char ** argv)
{
  CL_Parser CLP (argc,argv,"n=1000,s=1,p=1,q=1");
  int n = CLP('n');
  ERW img(n,CLP);

  prng.seed(int(CLP('s')));
  img.show();
  img.run();
  return 0;
}
