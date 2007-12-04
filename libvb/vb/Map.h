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

      Vertex (cpx z=cpx(0.0,0.0), real r=0.0) : pos(z), rad(r) {};
  };

  class Map;
  real cost_cp (const Map&);
  real delta_cost_cp (const Map&,int,cpx,real);

  class Map {
    public:
      int n;                       ///< The number of vertices.
      std::vector<Vertex> v;       ///< The graph structure.
      std::vector<bool> bd;        ///< The boundary vertices.

      int zero,one,infinity;       ///< As the names say.
      real scale;                  ///< Default scale of the embedding.

      Map (int nn) : n(nn), zero(-1), one(-1), infinity(-1) {
        for (int i=0; i<n; ++i) {
          v.push_back(Vertex(0.0));
          bd.push_back(false);
        }
      }

      adj_list::iterator find_edge (Edge e) {
        if (e.first >= n) return (adj_list::iterator) NULL;
        for (adj_list::iterator i = v[e.first].adj.begin(); i != v[e.first].adj.end(); ++i)
          if (*i == e.second) return i;
        return (adj_list::iterator) NULL;
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

      void inscribe (std::list<int> face_ext, real radius = 1.0, bool reverse = false) {
        int n_ext = face_ext.size();

        for (int i=0; i<n; ++i) { v[i].pos = 0.0; bd[i] = false; }
        scale = radius;

        int k=0;
        for (std::list<int>::iterator i = face_ext.begin(); i != face_ext.end(); ++i, --k) {
          bd[*i] = true;
          v[*i].pos = radius * exp(cpx(0,(reverse?-2.0:2.0)*3.1415927*k/n_ext));
        }
      }

      void balance () {
        bool dirty=true;

        while (dirty) {
          dirty = 0;
          for (int i=0; i<n; ++i) {
            if (bd[i]) continue;
            if (v[i].adj.size() == 0) continue;

            cpx old = v[i].pos; v[i].pos = 0.0;
            for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j)
              v[i].pos += v[*j].pos;
            v[i].pos /= v[i].adj.size();
            if (v[i].pos != old) dirty = true;
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
          bd.push_back(false);
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
            if (find_edge(Edge(*j,i)) == (adj_list::iterator) NULL)
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
            if ((i<*j) || (find_edge(Edge(*j,i)) == (adj_list::iterator) NULL)) F.segment(v[i].pos,v[*j].pos);
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
        real tmp = 0.0;
        for (int i=0; i<n; ++i)
          for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j)
            tmp += 1.0/((real) face(Edge(i,*j)).size());
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
          cpx z = v[i].pos / scale;
          w /= scale;
          v[i].pos = scale * (z-w)*exp(cpx(0,theta))/(cpx(1,0)-z*conj(w));
        }
      }

      real optimize (real func(const Map&), real delta_func(const Map&,int,cpx,real)) {
        real cost = func(*this);
        real delta_cost = 1;
        real delta = 1.0;
        real dv;

        while (delta_cost > 0) {
          std::cerr << cost << "         \r";
          delta_cost = 0.0;
          delta = sqrt(cost/n/10000);

          for (int i=0; i<n; ++i) {
            if (i != zero) {
              dv = delta_func(*this,i,v[i].pos-cpx(delta,0),v[i].rad); if (dv<0) { v[i].pos -= cpx(delta,0); delta_cost -= dv; }
              else {
                dv = delta_func(*this,i,v[i].pos+cpx(delta,0),v[i].rad); if (dv<0) { v[i].pos += cpx(delta,0); delta_cost -= dv; }
              }
            }

            if ((i != zero) && (i != one)) {
              dv = delta_func(*this,i,v[i].pos-cpx(0,delta),v[i].rad); if(dv<0) { v[i].pos -= cpx(0,delta); delta_cost -= dv; }
              else {
                dv = delta_func(*this,i,v[i].pos+cpx(0,delta),v[i].rad); if(dv<0) { v[i].pos += cpx(0,delta); delta_cost -= dv; }
              }
            }

            dv = delta_func(*this,i,v[i].pos,v[i].rad-delta); if(dv<0) { v[i].rad -= delta; delta_cost -= dv; }
            else {
              dv = delta_func(*this,i,v[i].pos,v[i].rad+delta); if(dv<0) { v[i].rad += delta; delta_cost -= dv; }
            }
          }

          //delta_cost = cost - func(*this);
          cost -= delta_cost;
        }

        return cost;
      }

      real circlepack (int _zero, int _one, std::list<int> _bord) {
        zero = _zero;
        one = _one;

        inscribe (_bord, sqrt((real)n)-0.5);
        balance ();

        scale = sqrt((real)n);
        mobius (v[zero].pos,0);
        mobius (0,-arg(v[one].pos));

        for (int i=0; i<n; ++i) v[i].rad = 1.0;
        return optimize (cost_cp,delta_cost_cp);
      }

      real ACPA () {
        for (int i=0; i<n; ++i) if (v[i].rad == 0.0) v[i].rad = 1.0;

        real E = 8*n;
        real old_E = E + 1.0;

        while (E < old_E) {
          //std::cerr << E << "   " << (old_E-E)/E << "        \r";
          old_E = E;
          E = 0.0;

          for (int i=0; i<n; ++i) {
            if (bd[i]) continue;
            if (v[i].adj.size() == 0) continue;

            int k = v[i].adj.size();

            real x = v[i].rad;
            real y = v[v[i].adj.back()].rad, z;
            real theta = 0.0;
            for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j) {
              z = y; y = v[*j].rad;
              theta += acos (((x+y)*(x+y) + (x+z)*(x+z) - (y+z)*(y+z)) / (2*(x+y)*(x+z)));
            }
            E += fabs(theta - 8*atan(1));

            real beta = sin (theta/(2*k));
            real delta = sin (4*atan(1)/k);

            v[i].rad *= ((1-delta)/delta) * (beta/(1-beta));
          }
        }

        return E;
      }

      void rad_to_pos (int _zero, int _one) {
        zero = _zero;
        one = _one;

        double R_max = 0.0;
        for (int i=0; i<n; ++i) if (v[i].rad > R_max) R_max = v[i].rad;

        cpx nowhere (-2*n*R_max, 0.0);
        for (int i=0; i<n; ++i) v[i].pos = nowhere;

        v[zero].pos = cpx(0.0,0.0);
        v[one].pos = cpx (v[zero].rad + v[one].rad, 0.0);

        bool dirty = true; while (dirty) {
          dirty = false;

          for (int i=0; i<n; ++i) {
            if (v[i].pos == nowhere) continue;
            if (v[i].adj.size() == 0) continue;

            cpx prev_z = v[v[i].adj.back()].pos;
            real prev_r = v[v[i].adj.back()].rad;

            for (adj_list::iterator j = v[i].adj.begin(); j != v[i].adj.end(); ++j) {
              cpx z = v[*j].pos;
              if ((prev_z != nowhere) && (z == nowhere) && ((!bd[i])||(!bd[*j]))) {
                real x = v[i].rad;
                real y = v[*j].rad;
                real alpha = acos (((x+y)*(x+y) + (x+prev_r)*(x+prev_r) - (y+prev_r)*(y+prev_r)) / (2*(x+y)*(x+prev_r)));
                v[*j].pos = v[i].pos + (x+y) * exp(cpx(0.0,alpha + arg(prev_z-v[i].pos)));
                z = v[*j].pos;
                dirty = true;
              }

              prev_z = z; prev_r = v[*j].rad;
            }
          }
        }
      }

      real circlepack2 (int _zero, int _one, const adj_list _bord) {
        //std::cerr << "First, add the outer vertex.\n";

        v.push_back (Vertex(cpx(0.0),sqrt((real)n)));
        v[n].adj = _bord;
        ++n;

        int prev_i = _bord.back();
        for (adj_list::const_iterator i = _bord.begin(); i != _bord.end(); ++i) {
          add_before (Edge(*i,prev_i),n-1);
          prev_i = *i;
        }

        //std::cerr << "Circle-pack this using ACPA.\n";

        for (int i=0; i<n; ++i) bd[i]=false;

        bd[n-1] = true; v[n-1].rad = sqrt((real)n);
        bd[_bord.front()] = true; v[_bord.front()].rad = sqrt((real)n);
        bd[_bord.back()] = true; v[_bord.back()].rad = sqrt((real)n);
        real output = ACPA();
        rad_to_pos (_bord.front(), _bord.back());
        return output;
      }
  };

  real cost_cp (const Map &m) {
    real t=0;

    for (int i=0; i<m.n; ++i) {
      for (adj_list::const_iterator j = m.v[i].adj.begin(); j != m.v[i].adj.end(); ++j) {
        cpx e = m.v[*j].pos - m.v[i].pos;
        real d = ( e.real()*e.real() + e.imag()*e.imag() );
        real r = m.v[i].rad + m.v[*j].rad;
        t += (d-r*r)*(d-r*r)/2;
      }

      if (m.bd[i]) {
        cpx e = m.v[i].pos;
        real r = abs(e);
        real dr = (m.scale - m.v[i].rad);
        real d = dr - r;
        t += d*d;
      }
    }

    return t;
  }

  real delta_cost_cp (const Map &m, int i, cpx z, real _r) {
    real t = 0.0;

    for (adj_list::const_iterator j = m.v[i].adj.begin(); j != m.v[i].adj.end(); ++j) {
      cpx e = m.v[*j].pos - m.v[i].pos;
      real d = ( e.real()*e.real() + e.imag()*e.imag() );
      real r = m.v[i].rad + m.v[*j].rad;
      t -= (d-r*r)*(d-r*r);

      e = m.v[*j].pos - z;
      d = ( e.real()*e.real() + e.imag()*e.imag() );
      r = _r + m.v[*j].rad;
      t += (d-r*r)*(d-r*r);
    }
    
    if (m.bd[i]) {
      cpx e = m.v[i].pos;
      real r = abs(e);
      real dr = (m.scale - m.v[i].rad);
      real d = dr - r;
      t -= d*d;

      dr = (m.scale - _r);
      d = dr - abs(z);
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

