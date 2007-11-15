/// @file Map.h
/// A class for (periodic) planar maps.

#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <list>

namespace vb {
  typedef std::pair<int,int> Edge;

  class Map {
    public:
      int n;                              ///< The number of vertices.
      std::vector < std::list<int> > adj; ///< The adjacency list.

      Map (int nn) : n(nn) {
        for (int i=0; i<n; ++i) {
          std::list<int> l;
          adj.push_back(l);
        }
      }

      bool contains (Edge e) {
        if (e.first >= n) return false;
        for (std::list<int>::iterator i = adj[e.first].begin(); i != adj[e.first].end(); ++i)
          if (*i == e.second) return true;
        return false;
      }

      void print_as_dot (std::ostream &os) {
        os << "digraph G {" << std::endl;
        for (int i=0; i<n; ++i)
          for (std::list<int>::iterator j = adj[i].begin(); j != adj[i].end(); ++j) {
            if (!contains(Edge(*j,i)))
              os << "  " << i << " -> " << *j << ";" << std::endl;
            else if (i<=*j)
              os << "  " << i << " -> " << *j << " [arrowhead = none]" << std::endl;
          }
        os << "}" << std::endl;
      }
  };

  std::ostream &operator<< (std::ostream &os, Map m) {
    os << m.n << " vertices:" << std::endl;
    for (int i=0; i<m.n; ++i) {
      os << "  " << i << " ->";
      for (std::list<int>::iterator j = m.adj[i].begin(); j != m.adj[i].end(); ++j)
        os << " " << *j;
      os << std::endl;
    }
    return os;
  }

  Map &operator<< (Map &m, Edge e) {
    m.adj[e.first].push_back(e.second);
    return m;
  }
}

#endif

