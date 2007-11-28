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
  typedef std::list<int>     adj_list;

  class Map {
    public:
      int n;                       ///< The number of vertices.
      std::vector<adj_list> adj;   ///< The adjacency list.
      std::vector<cpx> pos;        ///< The location of the points.

      Map (int nn) : n(nn) {
        for (int i=0; i<n; ++i) {
          adj_list l;
          adj.push_back(l);
          pos.push_back(0.0);
        }
      }

      adj_list::iterator find_edge (Edge e) {
        if (e.first >= n) return NULL;
        for (adj_list::iterator i = adj[e.first].begin(); i != adj[e.first].end(); ++i)
          if (*i == e.second) return i;
        return NULL;
      }

      Edge turn_left (Edge e) {
        adj_list::iterator ee = find_edge (Edge(e.second,e.first));
        if (ee == adj[e.second].begin()) ee = adj[e.second].end();
        --ee;
        return Edge(e.second,*ee);
      }

      Edge turn_right (Edge e) {
        adj_list::iterator ee = find_edge (Edge(e.second,e.first));
        ++ee;
        if (ee == adj[e.second].end()) ee = adj[e.second].begin();
        return Edge(e.second,*ee);
      }

      void add_before (Edge e, int v) {
        adj_list::iterator ee = find_edge(e);
        adj[e.first].insert (ee,v);
      }

      void add_after (Edge e, int v) {
        adj_list::iterator ee = find_edge(e);
        ++ee;
        adj[e.first].insert (ee,v);
      }

      std::list<int> face (Edge e) {
        std::list<int> l;
        int first = e.first; l.push_back(first);
        e = turn_left(e);
        while (e.first != first) {
          l.push_back(e.first);
          e = turn_left(e);
        }
        return l;
      }

      void inscribe (std::list<int> face_ext) {
        int n_ext = face_ext.size();

        for (int i=0; i<n; ++i) pos[i] = 0.0;

        int k=0;
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
            for (adj_list::iterator j = adj[*i].begin(); j != adj[*i].end(); ++j)
              pos[*i] += pos[*j];
            pos[*i] /= adj[*i].size();
            if (pos[*i] != old) dirty = true;
          }
        }
      }

      std::list<int> split_edges () {
        std::vector<int> tmp;
        for (int i=0; i<n*n; ++i) tmp.push_back(-1);

        std::list<int> output;
        std::vector<adj_list> new_vertices;

        for (int v1=0; v1<n; ++v1) {
          for (adj_list::iterator v2 = adj[v1].begin(); v2 != adj[v1].end(); ++v2) {
            int v3 = tmp[v1 + n*(*v2)];
            if (v3 >= 0) {                    // If the reverse is already filled
              (*v2) = v3;                     // Then point to it
            } else {                          // If it is not
              adj_list l;                     // Create it,
              l.push_back(v1);                // Populate it,
              l.push_back(*v2);               // With its two neighbors,
              new_vertices.push_back(l);      // Add it to the new vertices,
              v3 = n+new_vertices.size()-1;   // Get its number,
              tmp[(*v2) + n*v1] = v3;         // And mark it for the second way,
              output.push_back(v3);           // Add it to the return list,
              (*v2) = v3;                     // Point to it.
            }
          }
        }

        for (std::vector<adj_list>::iterator i = new_vertices.begin(); i != new_vertices.end(); ++i) {
          ++n;
          adj.push_back(*i);
          pos.push_back(0.0);
        }

        return output;
      }

      void hex_to_triangle (std::list<int> f) {
        std::list<int>::iterator i = f.begin();
        int x = *i; ++i;
        int b = *i; ++i;
        int y = *i; ++i;
        int c = *i; ++i;
        int z = *i; ++i;
        int a = *i; ++i;

        add_after (Edge(x,b),y);
        add_after (Edge(y,c),z);
        add_after (Edge(z,a),x);

        add_before (Edge(x,a),z);
        add_before (Edge(y,b),x);
        add_before (Edge(z,c),y);
      }

      void barycentric () {
        std::list<int> l = split_edges();
        for (std::list<int>::iterator i = l.begin(); i != l.end(); ++i) {
          std::list<int> f = face(Edge(*i,adj[*i].front()));
          if (f.size() == 6) hex_to_triangle(f);

          f = face(Edge(*i,adj[*i].back()));
          if (f.size() == 6) hex_to_triangle(f);
        }
      }

      void print_as_dot (std::ostream &os) {
        os << "digraph G {" << std::endl;
        for (int i=0; i<n; ++i)
          for (adj_list::iterator j = adj[i].begin(); j != adj[i].end(); ++j) {
            if (find_edge(Edge(*j,i)) == NULL)
              os << "  " << i << " -> " << *j << ";" << std::endl;
            else if (i<=*j)
              os << "  " << i << " -> " << *j << " [arrowhead = none]" << std::endl;
          }
        os << "}" << std::endl;
      }

      Figure output_as_figure (bool do_dots = true) {
        Figure F;

        for (int i=0; i<n; ++i) {
          if(do_dots) F.dot(pos[i]);
          for (adj_list::iterator j = adj[i].begin(); j != adj[i].end(); ++j)
            if ((i<*j) || (find_edge(Edge(*j,i))==NULL)) F.segment(pos[i],pos[*j]);
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
          for (adj_list::iterator j = adj[i].begin(); j != adj[i].end(); ++j)
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
      for (adj_list::iterator j = m.adj[i].begin(); j != m.adj[i].end(); ++j)
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

