#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmunds_karp_max_flow.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <iostream>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace boost;
using namespace std;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
typedef adjacency_list < listS, vecS, directedS,
        property < vertex_name_t, std::string >,
        property < edge_capacity_t, long,
        property < edge_residual_capacity_t, long,
        property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;
typedef graph_traits<Graph>::edge_descriptor edge_descriptor;

Graph *g;

property_map < Graph, edge_capacity_t >::type cap;
property_map < Graph, edge_reverse_t >::type rev;

void add_one (int i, int j) {
  edge_descriptor e1,e2;
  bool t;

  tie(e1,t) = add_edge(i,j,*g);
  tie(e2,t) = add_edge(j,i,*g);
  cap[e1] = 1; cap[e2] = 1;
  rev[e1] = e2; rev[e2] = e1;
}

int main(int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=100,t=1");
  int n = CLP.as_int('n');
  int n_iter = CLP.as_int('t');

  int stats[5];
  for (int i=0;i<5;++i) stats[i]=0;

  vb::PRNG prng;
  vb::ProgressBar PB (n_iter);

  for (int iter=1;iter<=n_iter;++iter) {
    PB.update (iter);

    g = new Graph (n*n+1);
    cap = get(edge_capacity, *g);
    rev = get(edge_reverse, *g);

    char *adj = new char[n*n];

    for (int x=0; x<n; ++x)
      for (int y=0; y<n; ++y)
        adj[x+n*y] = (char)prng.uniform(4);

    for (int x=0; x<n; ++x) {
      for (int y=0; y<n; ++y) {
        int i=x+n*y;
        if ((x<n-1)&&(adj[i]&1)) add_one(i,i+1);
        if ((y<n-1)&&(adj[i]&2)) add_one(i,i+n);
      }
    }

    for (int i=0; i<n; ++i) {
      add_one(i,n*n);
      add_one(n*i,n*n);
      add_one((n-1)+n*i,n*n);
      add_one((n-1)*n+i,n*n);
    }

    long flow = edmunds_karp_max_flow (*g,(n>>1)*(n+1),n*n);
    stats[flow]++;

    delete g;
    delete[] adj;
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
