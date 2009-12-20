#include <vb/CL_Parser.h>
#include <vb/Map.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace vb;

PRNG prng;

class Triangulation : public Map {
  public:
    Triangulation (int);
    Edge random_edge () const;
    bool flip (Edge e);
};

Triangulation::Triangulation (int n) : Map(n) {
  for (int i=1; i<n; ++i) (*this) << Edge(0,i);
  for (int i=n-1; i>1; --i) (*this) << Edge(1,i); (*this) << Edge(1,0);
  (*this) << Edge(2,0) << Edge(2,1) << Edge(2,3);
  for (int i=3; i<n-1; ++i) (*this) << Edge(i,0) << Edge(i,i-1) << Edge(i,1) << Edge(i,i+1);
  (*this) << Edge(n-1,0) << Edge(n-1,n-2) << Edge(n-1,1);
}

Edge Triangulation::random_edge () const {
  int i = prng.uniform (n);
  int j = prng.uniform (v[i]->adj.size());
  adj_list::iterator k;

  for (k=v[i]->adj.begin(); j>0; ++k, --j) { };
  return Edge (i,*k);
}

bool Triangulation::flip (Edge e) {
  int i = e.first;
  int j = e.second;

  adj_list::iterator e1 = find_edge(Edge(i,j));
  if (e1 == v[i]->adj.begin()) e1 = v[i]->adj.end(); --e1;
  int k = *e1;

  adj_list::iterator e2 = find_edge(Edge(j,i));
  if (e2 == v[j]->adj.begin()) e2 = v[j]->adj.end(); --e2;
  int l = *e2;

  if (k==l) return false;
  if (find_edge(Edge(k,l)) != (adj_list::iterator) NULL) return false;

  ++e1; if (e1 == v[i]->adj.end()) e1 = v[i]->adj.begin();
  v[i]->adj.erase(e1);

  ++e2; if (e2 == v[j]->adj.end()) e2 = v[j]->adj.begin();
  v[j]->adj.erase(e2);

  add_after (Edge(k,j),l);
  add_after (Edge(l,i),k);

  return true;
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=10,t=-1");
  int n = CLP('n');
  int t = CLP('t'); if (t==-1) t=50*n*n;

  Triangulation T (n);
  T.inscribe (T.face (Edge(0,1)));

  ProgressBar P (t);
  for (int i=0; i<t; ++i) {
    T.flip (T.random_edge());
    P.update(i);
  }
  P.die();

  T.show();
  T.inscribe (T.face (Edge (0,*(T.v[0]->adj.begin()))));
  T.balance_old();
  T.pause();

  std::cerr << T;
}
