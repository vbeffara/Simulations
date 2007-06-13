#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmunds_karp_max_flow.hpp>
#include <boost/graph/graph_utility.hpp>
#include <iostream>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace boost;
using namespace std;

typedef adjacency_list_traits < listS, vecS, directedS > Traits;
typedef Traits::edge_descriptor edge_descriptor;

typedef adjacency_list < listS, vecS, directedS,
        no_property,
        property < edge_capacity_t, long,
        property < edge_residual_capacity_t, long,
        property < edge_reverse_t, edge_descriptor > > > > Graph;

void add_one (Graph &g, int i, int j) {
  edge_descriptor e1,e2;
  bool t;

  tie(e1,t) = add_edge(i,j,g);
  tie(e2,t) = add_edge(j,i,g);

  put (edge_reverse, g, e1, e2);
  put (edge_reverse, g, e2, e1);
}

int main(int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=100,t=1,p=.5");
  int n = CLP.as_int('n');
  int n_iter = CLP.as_int('t');
  double p = CLP.as_double('p');

  int stats[5];
  for (int i=0;i<5;++i) stats[i]=0;

  vb::PRNG prng;
  vb::ProgressBar PB (n_iter);

  // Create the graph once and for all

  Graph g (n*n+1);

  for (int x=0; x<n; ++x) {
    for (int y=0; y<n; ++y) {
      int i=x+n*y;
      if (x<n-1) add_one (g,i,i+1);
      if (y<n-1) add_one (g,i,i+n);
    }
  }

  // Wired boundary conditions

  for (int i=0; i<n; ++i) {
    add_one(g,i,n*n);
    add_one(g,n*i,n*n);
    add_one(g,(n-1)+n*i,n*n);
    add_one(g,(n-1)*n+i,n*n);
  }

  // Then simulate

  property_map<Graph,edge_capacity_t>::type cap = get (edge_capacity,g);
  Graph::edge_iterator e,e_final;

  for (int iter=1;iter<=n_iter;++iter) {
    for (tie(e,e_final)=edges(g); e!=e_final; ++e) {
      edge_descriptor e_r = get (edge_reverse,g,*e);
      int o = prng.bernoulli(p);
      cap[*e] = o;
      cap[e_r] = o;
    }

    long flow = edmunds_karp_max_flow (g,(n>>1)*(n+1),n*n);
    stats[flow]++;

    PB.update (iter);
  }

  PB.die();

  cout << n;
  for (int i=1; i<=4; ++i) {
    int bla=0;
    for (int j=i; j<=4; ++j) bla += stats[j];
    cout << " " << (double)bla / (double)n_iter;
  }
  cout << " " << n_iter << endl;
}
