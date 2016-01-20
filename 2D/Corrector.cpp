#include <vb/Hub.h>
#include <vb/Map.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

int center = 6;
int infinity = 13;

int main (int argc, char **argv) {
  H.init ("Corrector", argc, argv, "n=10,p=.9,b");
  int n = H['n'];
  double p = H['p'];
  bool batch = H['b'];

  Map m (n*n);
  vector<char> adj(n*n,0);

  for (int i=0; i<n; ++i) {
    for (int j=0; j<n; ++j) {
      int ij = i+n*j;

      if (prng.bernoulli(p)) adj[ij] |= 1;
      if (prng.bernoulli(p)) adj[ij] |= 2;
      if ((i>0) && (adj[ij-1] & 1)) adj[ij] |= 4;
      if ((j>0) && (adj[ij-n] & 2)) adj[ij] |= 8;
    }
  }

  for (int i=0; i<n; ++i) {
    adj[i] |= 5;
    adj[n*i] |= 10;
    adj[n-1+n*i] |= 10;
    adj[(n-1)*n+i] |= 5;
  }

  for (int i=0; i<n; ++i) {
    adj[i] &= 7;
    adj[n*i] &= 11;
    adj[n-1+n*i] &= 14;
    adj[(n-1)*n+i] &= 13;
  }

  for (int i=0; i<n; ++i) {
    for (int j=0; j<n; ++j) {
      int ij = i+n*j;

      if ((i<n-1) && (adj[ij] & 1)) m << Edge(ij, ij+1);
      if ((j<n-1) && (adj[ij] & 2)) m << Edge(ij, ij+n);
      if ((i>0) && (adj[ij] & 4)) m << Edge(ij, ij-1);
      if ((j>0) && (adj[ij] & 8)) m << Edge(ij, ij-n);

      m.v[ij] -> z = cpx(i,j);

      m.bd[ij] = ((i==0) || (i==n-1) || (j==0) || (j==n-1));
    }
  }

  if (!batch) { m.show(); m.pause(); }

  double output = m.balance();
  cout << n << " " << p << " " << output << endl;

  if (!batch) m.pause();

  return 0;
}
