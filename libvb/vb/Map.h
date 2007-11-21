/// @file Map.h
/// A class for (periodic) planar maps.

#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <vector>
#include <list>
#include <vb/types.h>
#include <vb/Figure.h>

namespace vb {
  typedef std::pair<int,int> Edge;

  class Map {
    public:
      int n;                                ///< The number of vertices.
      std::vector < std::vector<int> > adj; ///< The adjacency list.
      std::vector <cpx> pos;                ///< The location of the points.

      Map (int nn) : n(nn) {
        for (int i=0; i<n; ++i) {
          std::vector<int> l;
          adj.push_back(l);
          pos.push_back(0.0);
        }
      }

      bool contains (Edge e) {
        if (e.first >= n) return false;
        for (std::vector<int>::iterator i = adj[e.first].begin(); i != adj[e.first].end(); ++i)
          if (*i == e.second) return true;
        return false;
      }

      Edge after (Edge e) {
        int cur=-1, prev=-1;
        for (std::vector<int>::iterator i = adj[e.second].begin(); i != adj[e.second].end(); ++i) {
          prev = cur; cur = *i;
          if ((cur == e.first) && (prev >= 0)) return Edge (e.second,prev);
        }
        return Edge (e.second,cur);
      }

      std::list<int> face (Edge e) {
        std::list<int> l;
        int first = e.first; l.push_back(first);
        e = after(e);
        while (e.first != first) {
          l.push_back(e.first);
          e = after(e);
        }
        return l;
      }

      void inscribe (Edge e) {
        std::list<int> face_ext = face(Edge(e.second,e.first));
        int n_ext = face_ext.size();

        for (int i=0; i<n; ++i) pos[i] = 0.0;

        int k=1;
        for (std::list<int>::iterator i = face_ext.begin(); i != face_ext.end(); ++i, --k)
          pos[*i] = cpx(cos(2.0*3.1415927*k/n_ext), sin(2.0*3.1415927*k/n_ext));

        std::list<int> inner;
        for (int i=0; i<n; ++i)
          if (pos[i]==cpx(0.0,0.0)) inner.push_back(i);

        bool dirty=true;

        while (dirty) {
          dirty = 0;
          for (std::list<int>::iterator i = inner.begin(); i != inner.end(); ++i) {
            cpx old = pos[*i]; pos[*i] = 0.0;
            for (std::vector<int>::iterator j = adj[*i].begin(); j != adj[*i].end(); ++j)
              pos[*i] += pos[*j];
            pos[*i] /= adj[*i].size();
            if (pos[*i] != old) dirty = true;
          }
        }
      }

      void print_as_dot (std::ostream &os) {
        os << "digraph G {" << std::endl;
        for (int i=0; i<n; ++i)
          for (std::vector<int>::iterator j = adj[i].begin(); j != adj[i].end(); ++j) {
            if (!contains(Edge(*j,i)))
              os << "  " << i << " -> " << *j << ";" << std::endl;
            else if (i<=*j)
              os << "  " << i << " -> " << *j << " [arrowhead = none]" << std::endl;
          }
        os << "}" << std::endl;
      }

      Figure output_as_figure () {
        Figure F;

        for (int i=0; i<n; ++i) {
          F.dot(pos[i]);
          for (std::vector<int>::iterator j = adj[i].begin(); j != adj[i].end(); ++j)
            F.segment(pos[i],pos[*j]);
        }
        return F;
      }

      int nb_sommets () {
        return n;
      }

      int nb_aretes () {
        int tmp = 0;
        for (int i=0; i<n; ++i) tmp += adj[i].size();
        return (tmp/2);
      }

      int nb_faces () {
        double tmp = 0.0;
        for (int i=0; i<n; ++i)
          for (std::vector<int>::iterator j = adj[i].begin(); j != adj[i].end(); ++j)
            tmp += 1.0/((double) face(Edge(i,*j)).size());
        return (int) floor (tmp + .1);
      }

      int euler () {
        return nb_sommets() - nb_aretes() + nb_faces();
      }

      int genre () {
        return 1 - (euler()/2);
      }
  };

  std::ostream &operator<< (std::ostream &os, Map m) {
    os << m.n << " vertices:" << std::endl;
    for (int i=0; i<m.n; ++i) {
      os << "  " << i << " ->";
      for (std::vector<int>::iterator j = m.adj[i].begin(); j != m.adj[i].end(); ++j)
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

