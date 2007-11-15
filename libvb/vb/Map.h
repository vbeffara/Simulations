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

