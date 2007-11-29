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

  class Vertex {
    public:
      cpx pos;
      adj_list adj;
      real rad;

      Vertex (cpx z, real r=0.0) : pos(z), rad(r) {};
  };

  class Map;
  real cost_cp (Map);

  class Map {
    public:
      int n;                       ///< The number of vertices.
      std::vector<Vertex> v;       ///< The graph structure.

      std::list<int> bord;         ///< The boundary (if needed for e.g. circle packing).
      int zero,one,infinity;       ///< As the names say.

      Map (int nn) : n(nn), zero(-1), one(-1), infinity(-1) {
        for (int i=0; i<n; ++i) {
          v.push_back(Vertex(0.0));
        }
      }

      adj_list::iterator find_edge (Edge e) {
        if (e.first >= n) return NULL;
        for (adj_list::iterator i = v[e.first].adj.begin(); i != v[e.first].adj.end(); ++i)
          if (*i == e.second) return i;
        return NULL;
      }

      Edge turn_left (Edge e) {
        adj_list::iterator ee = find_edge (Edge(e.second,e.first));
        if (ee == v[e.second].adj.begin()) ee = v[e.second].adj.end();
        --ee;
        return Edge(e.second,*ee);
      }

      Edge turn_right (Edge e) {
        adj_list::iterator ee = find_edge (Edge(e.second,e.first));
        ++ee;
        if (ee == v[e.second].adj.end()) ee = v[e.second].adj.begin();
        return Edge(e.second,*ee);
      }

      void add_before (Edge e, int vv) {
        adj_list::iterator ee = find_edge(e);
        v[e.first].adj.insert (ee,vv);
      }

      void add_after (Edge e, int vv) {
        adj_list::iterator ee = find_edge(e);
        ++ee;
        v[e.first].adj.insert (ee,vv);
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

        for (int i=0; i<n; ++i) v[i].pos = 0.0;

        int k=0;
        for (std::list<int>::iterator i = face_ext.begin(); i != face_ext.end(); ++i, --k)
          v[*i].pos = cpx(cos(2.0*3.1415927*k/n_ext), sin(2.0*3.1415927*k/n_ext));

        std::list<int> inner;
        for (int i=0; i<n; ++i)
          if (v[i].pos==cpx(0.0,0.0)) inner.push_back(i);

        bool dirty=true;

        while (dirty) {
          dirty = 0;
          for (std::list<int>::iterator i = inner.begin(); i != inner.end(); ++i) {
            cpx old = v[*i].pos; v[*i].pos = 0.0;
            for (adj_list::iterator j = v[*i].adj.begin(); j != v[*i].adj.end(); ++j)
              v[*i].pos += v[*j].pos;
            v[*i].pos /= v[*i].adj.size();
            if (v[*i].pos != old) dirty = true;
          }
        }
      }

      std::list<int> split_edges () {
        std::vector<int> tmp;
        for (int i=0; i<n*n; ++i) tmp.push_back(-1);

        std::list<int> output;
        std::vector<adj_list> new_vertices;

        for (int v1=0; v1<n; ++v1) {
          for (adj_list::iterator v2 = v[v1].adj.begin(); v2 != v[v1].adj.end(); ++v2) {
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
          v.push_back(Vertex(0.0));
          v.back().adj = (*i);
          ++n;
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
          std::list<int> f = face(Edge(*i,v[*i].adj.front()));
          if (f.size() == 6) hex_to_triangle(f);

          f = face(Edge(*i,v[*i].adj.back()));
          if (f.size() == 6) hex_to_triangle(f);
        }
      }

      void print_as_dot (std::ostream &os) {
        os << "digraph G {" << std::endl;
        for (int i=0; i<n; ++i)
          for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j) {
            if (find_edge(Edge(*j,i)) == NULL)
              os << "  " << i << " -> " << *j << ";" << std::endl;
            else if (i<=*j)
              os << "  " << i << " -> " << *j << " [arrowhead = none]" << std::endl;
          }
        os << "}" << std::endl;
      }

      Map plot_vertices (Figure &F) {
        for (int i=0; i<n; ++i) {
          F.dot(v[i].pos);
        }
        return (*this);
      }

      Map plot_edges (Figure &F) {
        for (int i=0; i<n; ++i) {
          for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j)
            if ((i<*j) || (find_edge(Edge(*j,i))==NULL)) F.segment(v[i].pos,v[*j].pos);
        }
        return (*this);
      }

      Map plot_circles (Figure &F) {
        for (int i=0; i<n; ++i) {
          if (v[i].rad>0) F.circle(v[i].pos,v[i].rad);
        }
        return (*this);
      }

      int nb_sommets () {
        return n;
      }

      int nb_aretes () {
        int tmp = 0;
        for (int i=0; i<n; ++i) tmp += v[i].adj.size();
        return (tmp/2);
      }

      int nb_faces () {
        double tmp = 0.0;
        for (int i=0; i<n; ++i)
          for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j)
            tmp += 1.0/((double) face(Edge(i,*j)).size());
        return (int) floor (tmp + .1);
      }

      int euler () {
        return nb_sommets() - nb_aretes() + nb_faces();
      }

      int genre () {
        return 1 - (euler()/2);
      }

      void mobius (cpx w, real theta) {
        for (int i=0; i<n; ++i) {
          cpx z = v[i].pos;
          v[i].pos = (z-w)*exp(cpx(0,theta))/(cpx(1,0)-z*conj(w));
        }
      }

      real optimize (real func(Map), real eps = 0) {
        real cost = func(*this);
        real old_cost = cost + eps + 1;
        real tmp_cost = cost;
        while (old_cost - cost > eps) {
          std::cerr << cost << "\r";
          old_cost = cost;
          real delta = sqrt(cost/n/10.0);

          for (int i=0; i<n; ++i) {
            if (i != zero) {
              v[i].pos += cpx(delta,0); tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else {
                v[i].pos -= cpx(2*delta,0); tmp_cost = func(*this);
                if (tmp_cost < cost) cost = tmp_cost;
                else v[i].pos += cpx(delta,0);
              }
            }

            if ((i != zero) && (i != one)) {
              v[i].pos += cpx(0,delta); tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else {
                v[i].pos -= cpx(0,2*delta); tmp_cost = func(*this);
                if (tmp_cost < cost) cost = tmp_cost;
                else v[i].pos += cpx(0,delta);
              }
            }

            v[i].rad += delta; tmp_cost = func(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else {
              v[i].rad -= 2*delta; tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else v[i].rad += delta;
            }
          }
        }

        return cost;
      }

      void circlepack (int _zero, int _one, std::list<int> _bord) {
        bord = _bord;
        zero = _zero;
        one = _one;

        inscribe (bord);
        mobius (v[zero].pos,0);
        mobius (0,-arg(v[one].pos));
        optimize (cost_cp);
      }
  };

  real cost_cp (Map m) {
    real t=0;

    for (int i=0; i<m.n; ++i)
      for (adj_list::iterator j = m.v[i].adj.begin(); j != m.v[i].adj.end(); ++j) {
        cpx e = m.v[*j].pos - m.v[i].pos;
        real d = sqrt ( e.real()*e.real() + e.imag()*e.imag() );
        real r = m.v[i].rad + m.v[*j].rad;
        t += (d-r)*(d-r);
      }

    t /= 2.0;

    for (std::list<int>::iterator j = m.bord.begin(); j != m.bord.end(); ++j) {
      real d = 1.0 - abs(m.v[*j].pos) - m.v[*j].rad;
      t += d*d;
    }

    return t;
  }

  std::ostream &operator<< (std::ostream &os, Map m) {
    os << m.n << " vertices:" << std::endl;
    for (int i=0; i<m.n; ++i) {
      os << "  " << i << " ->";
      for (adj_list::iterator j = m.v[i].adj.begin(); j != m.v[i].adj.end(); ++j)
        os << " " << *j;
      os << std::endl;
    }
    return os;
  }

  Map &operator<< (Map &m, Edge e) {
    m.v[e.first].adj.push_back(e.second);
    return m;
  }
}

#endif

