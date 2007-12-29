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

      Map (int nn);

      adj_list::iterator find_edge (Edge e);

      Edge turn_left (Edge e);
      Edge turn_right (Edge e);

      void add_before (Edge e, int vv);
      void add_after (Edge e, int vv);

      std::list<int> face (Edge e);

      void inscribe (std::list<int> face_ext, real radius = 1.0, bool reverse = false);

      void balance (Figure *F = NULL);

      std::list<int> split_edges ();

      void hex_to_triangle (std::list<int> f);

      void barycentric ();

      void print_as_dot (std::ostream &os);

      void plot_vertices (Figure &F);

      void plot_edges (Figure &F);

      void plot_circles (Figure &F);

      int nb_sommets () { return n; }
      int nb_aretes ();
      int nb_faces ();
      int euler ();
      int genre ();

      void mobius (cpx w, real theta);

      real optimize (real func(const Map&), real delta_func(const Map&,int,cpx,real));

      real circlepack (int _zero, int _one, std::list<int> _bord);

      real ACPA ();

      void rad_to_pos (int _zero, int _one);

      void rotate (real theta);

      void mobius_circle (cpx w, real r);

      real circlepack2 (int _zero, int _one, const adj_list _bord);
  };

  real       cost_cp (const Map &m);
  real delta_cost_cp (const Map &m, int i, cpx z, real _r);

  std::ostream &operator<< (std::ostream &os, Map m);

  Map &operator<< (Map &m, Edge e);
}

#endif

