#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/TriMatrix.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

class Walker {
  public:
    Walker    ();

    int step  (int dir, int add);
    int norm1 () const;

    vector <int> pos;
    int          length;

  private:
    void reframe ();

    vector < TriMatrix<int> > Max;
    vector < TriMatrix<int> > Min;
};

Walker::Walker () : pos(3,0), length(0), Max(3,INT_MIN), Min(3,INT_MAX) {
  for (int i=0; i<3; ++i) {
    Max[i].put (0,0,0);
    Min[i].put (0,0,0);
  }
}

int Walker::step (int dir, int add) {
  if (add==0) return 0;
  if (add>0) {
    if (pos[dir] < Max[dir].get(pos[(dir+1)%3], pos[(dir+2)%3])) return 0;
    pos[dir] += add;
    reframe();
    ++ length;
  } else {
    if (pos[dir] > Min[dir].get(pos[(dir+1)%3], pos[(dir+2)%3])) return 0;
    pos[dir] += add;
    reframe();
    ++ length;
  }
  return 1;
}

void Walker::reframe () {
  for (int i=0; i<3; ++i) {
    if (pos[i] > Max[i].get(pos[(i+1)%3], pos[(i+2)%3]))
      Max[i].put(pos[(i+1)%3], pos[(i+2)%3], pos[i]);
    if (pos[i] < Min[i].get(pos[(i+1)%3], pos[(i+2)%3]))
      Min[i].put(pos[(i+1)%3], pos[(i+2)%3], pos[i]);
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

  vector <int> ends (l,0);

  { ProgressBar PB ((n>1 ? n : l));
    for (int i=0; i<n; ++i) {
      Walker W;

      while (W.length < l) {
        if (n==1) cout << W;
        ends[W.length] += W.norm1();
        while (0 == W.step (prng()%3, 2*(prng()%2) - 1)) { };
        if (n==1) PB.update (W.length);
      }

      if (n>1) PB.update(i);
    }
  }

  if (n>1) {
    for (int i=0; i<l; ++i) {
      cout << i << ' ' << double(ends[i])/double(n) << endl;
    }
  }

  return 0;
}
