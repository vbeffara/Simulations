#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/TriMatrix.h>

using namespace std;
using namespace vb;

class Walker {
  public:
    Walker ();
    ~Walker ();

    int step (int dir, int add);

    int norm1 () const;

    int pos[3];
    int length;

  private:
    void reframe ();

    TriMatrix<int> *Max[3];
    TriMatrix<int> *Min[3];
};

Walker::Walker () : length(0) {
  for (int i=0; i<3; ++i) {
    pos[i] = 0;
    Max[i] = new TriMatrix<int> (INT_MIN); Max[i] -> put (0,0,0);
    Min[i] = new TriMatrix<int> (INT_MAX); Min[i] -> put (0,0,0);
  }
}

Walker::~Walker () {
  for (int i=0; i<3; ++i) {
    delete Max[i]; Max[i] = NULL;
    delete Min[i]; Min[i] = NULL;
  }
}

int Walker::step (int dir, int add) {
  if (add==0) return 0;
  if (add>0) {
    if (pos[dir] < Max[dir] -> get(pos[(dir+1)%3], pos[(dir+2)%3])) return 0;
    pos[dir] += add;
    reframe();
    ++ length;
  } else {
    if (pos[dir] > Min[dir] -> get(pos[(dir+1)%3], pos[(dir+2)%3])) return 0;
    pos[dir] += add;
    reframe();
    ++ length;
  }
  return 1;
}

void Walker::reframe () {
  for (int i=0; i<3; ++i) {
    if (pos[i] > Max[i] -> get(pos[(i+1)%3], pos[(i+2)%3]))
      Max[i] -> put(pos[(i+1)%3], pos[(i+2)%3], pos[i]);
    if (pos[i] < Min[i] -> get(pos[(i+1)%3], pos[(i+2)%3]))
      Min[i] -> put(pos[(i+1)%3], pos[(i+2)%3], pos[i]);
  }
}

int Walker::norm1 () const {
  return abs(pos[0]) + abs(pos[1]) + abs(pos[2]);
}

ostream &operator<< (ostream &o, const Walker &W) {
  o << W.pos[0] << ' ' << W.pos[1] << ' ' << W.pos[2] << ' ' << W.norm1() << endl;
  return o;
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc, argv, "l=1000,n=1");
  int l = CLP('l');
  int n = CLP('n');

  int *ends = new int[l]; for (int i=0; i<l; ++i) ends[i]=0;

  PRNG prng;

  for (int i=0; i<n; ++i) {
    Walker W;

    while (W.length < l) {
      if (n==1) cout << W;
      ends[W.length] += W.norm1();
      while (0 == W.step (prng.rand()%3, 2*(prng.rand()%2) - 1)) { };
    }
  }

  if (n>1) {
    for (int i=0; i<l; ++i) {
      cout << i << ' ' << double(ends[i])/double(n) << endl;
    }
  }

  delete[] ends;
  return 0;
}
