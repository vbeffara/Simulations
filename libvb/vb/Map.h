/// @file
/// A class for (periodic) planar maps.

#ifndef MAP_H
#define MAP_H

#include <vb/Figure.h>
#include <vb/Minimizer.h>

namespace vb {
  typedef std::pair<int,int> Edge;     ///< A simple alias for map edges.
  typedef std::list<int>     adj_list; ///< A simple alias for adjacency lists.

  /** A vertex of a vb::Map.
   *
   * It inherits from vb::Circle, because it has a position and a radius 
   * (for circle packings), but this is certainly not optimal, 
   * especially since we don't relate vb::Map to vb::Figure much anymore 
   * ...
   */

  class Vertex : public Circle {
    public:
      adj_list adj; ///< The list of adjacent vertices.

      /// The default constructor.
      Vertex (const cpx &z = cpx(0.0,0.0), const double &r = 0.0) : Circle(z,r) {};

      /// Shortcut for a point on the real line.
      Vertex (const double &z, const double &r = 0.0) : Circle(z,r) {};
  };

  /** The main class for a planar map.
   *
   * It is essentially a graph structure with ordered adjacency lists. 
   * Many functions are provided for building maps, walking through 
   * them, circle-packing them, etc. Most of that is probably contained 
   * in the Boost library, or implementable using it, but not that 
   * easily I believe.
   */

  class Map : public Picture {
    public:
      int n;                  ///< The number of vertices.
      std::vector<Vertex*> v; ///< The graph structure.
      std::vector<bool> bd;   ///< The boundary vertices.

      int zero;               ///< The index of a vertex to locate at the origin.
      int one;                ///< The index of a vertex to locate on the real axis.
      int infinity;           ///< The index of a vertex "at infinity" (when that makes sense).
      double scale;           ///< The scale of the embedding.

      /** The constructor for a vb::MAp with no edges.
       *
       * Of course the size of the map (number of vertices) can be 
       * changed afterwards, but adding edges with the << operator will 
       * not do it.
       *
       * @param nn The number of vertices in the map.
       */

      Map (int nn);

      /** Find an edge in the map.
       *
       * Given an edge, find it in one of the adjacency lists of the 
       * vb::Map; returns NULL if the edge is not present, else return 
       * an iterator for easier use later.
       *
       * @param e The edge to look for.
       */

      adj_list::iterator find_edge (const Edge &e) const;

      Edge turn_left  (const Edge &e) const; ///< Walk on the map, turning left.
      Edge turn_right (const Edge &e) const; ///< Walk on the map, turning right.

      /** Add an edge to the map.
       *
       * This looks for an edge in the map, and before it in the 
       * adjacency list it adds a new edge with the specified endpoint.
       *
       * @param e  The edge to look for.
       * @param vv The endpoint of the new edge.
       */

      void add_before (const Edge &e, int vv);

      /** Add an edge to the map.
       *
       * This looks for an edge in the map, and after it in the 
       * adjacency list it adds a new edge with the specified endpoint.
       *
       * @param e  The edge to look for.
       * @param vv The endpoint of the new edge.
       */

      void add_after (const Edge &e, int vv);

      /** Compute the face associated to an edge.
       *
       * This returns, as a list of vertex indices, the border of the 
       * face determined by the edge e. If all goes well (meaning if the 
       * vb::Map structure is valid), the face is homeomorphic to a 
       * circle, and its border is oriented along e.
       *
       * @param e The base edge of the face.
       */

      std::list<int> face (Edge e);

      /** Position the given vertices as a regular polygon.
       *
       * This is useful e.g. for embedding the graph, when the list 
       * given as first parameter is the outer face of a planar graph, 
       * but this need not be the case.
       *
       * @param face_ext The list of vertices to put along a circle.
       * @param radius   The radius of the circle (sqrt(n) is a good choice).
       * @param reverse  If true, rotate clockwise.
       */

      void inscribe (const std::list<int> &face_ext, const double &radius = 1.0, bool reverse = false);

      /** Produce a balanced embedding of the map.
       *
       * This means that every non-boundary point is the barycenter of 
       * its neighbors; boundary points (those specified by bd) are not 
       * affected, so it is a good idea to use inscribe() before that.
       */

      double balance ();

      /** Produce a balanced embedding of the map.
       *
       * This means that every non-boundary point is the barycenter of 
       * its neighbors; boundary points (those specified by bd) are not 
       * affected, so it is a good idea to use inscribe() before that.
       *
       * The algorithm is the old one (iterated relaxation), kept for 
       * further reference.
       */

      void balance_old ();

      /// Add a new vertex at the middle of every edge in the map.

      std::list<int> split_edges ();

      /** Inscribe a triangle in the given hexagon.
       *
       * This is performed when doing barycentric subdivision of a map; 
       * if f does not have 6 elements, it does nothing.
       *
       * @param f The hexagon to split into 4 triangles.
       */

      void hex_to_triangle (const std::list<int> &f);

      /** Perform barycentric subdivision of the map.
       *
       * This (at the moment) only affects faces that are triangles. To 
       * exclude a face from being split (e.g. the outer face), add an 
       * inner vertex to it before the subdivision, and it will be 
       * protected.
       */

      void barycentric ();

      /// Output a DOT format representation of the map to the given stream.

      void print_as_dot (std::ostream &os);

      /// Add the vb::Map vertices to the given target vb::Figure.

      void plot_vertices (Figure &F);

      /// Add the vb::Map edges to the given target vb::Figure.

      void plot_edges (Figure &F);

      /** Add the vb::Map circles to the given target vb::Figure.
       *
       * This will skip vertices with a non-positive radius.
       *
       * @param F The target vb::Figure.
       */

      void plot_circles (Figure &F);

      /// Returns the number of vertices in the map.

      int nb_sommets () { return n; }

      /// Returns the number of edges in the map.

      int nb_aretes ();

      /// Returns the number of faces in the map.

      int nb_faces ();

      /// Returns the Euler characteristic of the map.

      int euler ();

      /// Returns the genus of the map.

      int genre ();

      /** Maps each vertex of the map using the given Möbius transform.
       *
       * The Möbius map is given by one complex parameter w and one real 
       * parameter theta. w is mapped to 0, and theta is the rotation 
       * term (at least if w=0, if not then the meaning of theta is 
       * quite arbitrary).
       *
       * Note that this does not care for the radii of the circles; it 
       * will break tangencies.
       *
       * @param w     The point that will be mapped to 0.
       * @param theta The rotation angle.
       */

      void mobius (cpx w, const double &theta);

      /** Find the minimum of the given function.
       *
       * This tries to find a map embedding minimizing the given cost 
       * function. If it is faster to compute the difference in costs 
       * between one embedding and an updated embedding where exactly 
       * one of the vertices is moved and/or has its radius updated, the 
       * algorithm can be hinted about that by giving it a helper 
       * function.
       *
       * The algorith is very stupid, but works reasonably efficiently 
       * (for instance, for circle packings when minimizing the sum of 
       * squares of mismatches, it is actually quicker than ACPA in most 
       * cases), except when it doesn't: there is no guarantee of 
       * convergence or anything.
       *
       * This works in-place (i.e. it affects the embedding of the 
       * current map), and returns the value of the cost function for 
       * the final embedding.
       *
       * @param  func       The cost function to minimize.
       * @param  delta_func The helper function.
       */

      double optimize (double func(const Map&), double delta_func(const Map&,int,cpx,double));

      /** Produce a circle packing of the map.
       *
       * This produces a circle packing whose combinatorics are given by 
       * the map. It only makes sense if the map is planar, in which 
       * case the circle configuration will be contained in a disk and 
       * the specified disks will be internally tangent to it.
       *
       * Uniqueness is obtained by specifying which vertex should be 
       * mapped to 0, and which one (not necessarily one of the 
       * neighbors of the former) is to land on the positive real axis.  
       * If those two are the same, who knows what will happen.
       *
       * @param _zero The index of the vertex to place at the origin.
       * @param _one  The index of the vertex to place on the real line.
       * @param _bord The list of boundary vertices.
       */

      double circlepack (int _zero, int _one, const std::list<int> &_bord);

      /** Compute the labels for a circle packing.
       *
       * This uses the algorithm given by Collins and Stephenson in "A 
       * circle packing algorithm" (Comput. Geom. 25 (2003), no. 3, 
       * 233--256), hence the name of the method. It is very stable, 
       * reasonably quick (though apparently not as quick as 
       * optimize() in the "good" cases ...), and does not depend on the 
       * genus.
       *
       * Note that there is no ramification, i.e. the total angle at 
       * every vertex is 2pi.
       *
       * Again, boundary vertices are not affected, so one can fix their 
       * radii in advance. If no vertex is marked as a boundary vertex, 
       * it is not clear what happens.
       */

      double ACPA ();

      /** Given radii for each vertex, compute an embedding of the map.
       *
       * This is typically used after ACPA(). The algorithm used is 
       * very stupid, and only makes sense for planar maps. The vertices 
       * given as parameter have to be neighbors, unfortunately.
       *
       * @todo Find a better algorithm, and possibly a way to make it 
       * work at least for genus 1.
       *
       * @param _zero The index of a vertex to be mapped to the origin.
       * @param _one  The neighbor of the former to put on the real axis.
       */

      void rad_to_pos (int _zero, int _one);

      /// Rotate the whole embedding by a given angle.

      void rotate (const double &theta);

      /** Apply a Möbius transform to the circles of the embedding.
       *
       * It is similar to mobius() but works on circles instead, so it 
       * does preserve tangencies. The parameter describe a circle that 
       * is mapped to a circle centered at the origin.
       *
       * There is no rotation parameter here, so one should call 
       * rotate() afterwards if needed.
       *
       * @param w The center of the circle to move to the origin.
       * @param r The radius of the circle to move to the origin.
       */

      void mobius_circle (cpx w, double r);

      /// Returns the leftmost coordinate of anything on the map.
      double left ();

      /// Returns the rightmost coordinate of anything on the map.
      double right ();

      /// Returns the topmost coordinate of anything on the map.
      double top ();

      /// Returns the bottom-most coordinate of anything on the map.
      double bottom ();

    protected:
      /// Update the contents of AutoWindow::stage.
      virtual void paint ();
  };

  /// Export a vb::Map as text (kind of DOT-looking).

  std::ostream &operator<< (std::ostream &os, const Map &m);

  /// Add an vb::Edge to a vb::Map.

  Map &operator<< (Map &m, const Edge &e);

  /** Compute the square distance to a balanced embedding.
   *
   * @param x       The list of point coordinates (3 per vertex).
   * @param g       The vector in which to compute the gradient.
   * @param context A pointer to a vb::Map.
   */

  double Map_fg_balance (const Vector &x, Vector &g, void *context);

  /** Compute the square distance to a circle packing.
   *
   * This is meant as a base to "real" circle packing helpers, and it 
   * imposes no constraint at all on the packing except the tangency 
   * structure.
   *
   * @param x       The list of point coordinates (3 per vertex).
   * @param g       The vector in which to compute the gradient.
   * @param context A pointer to a vb::Map.
   */

  double Map_fg_circle_base (const Vector &x, Vector &g, void *context);

  /** Compute the square distance to a circle packing.
   *
   * The points at which bd[] is true are considered to be of fixed 
   * radius, so the gradient in their radius is always 0.0. For 
   * inscribing a graph in the unit circle, use Map_fg_circle_disk.
   *
   * @param x       The list of point coordinates (3 per vertex).
   * @param g       The vector in which to compute the gradient.
   * @param context A pointer to a vb::Map.
   */

  double Map_fg_circle_bd (const Vector &x, Vector &g, void *context);

  /** Compute the square distance to a circle packing of the unit disk.
   *
   * The points for which bd[] is true will correspond to circles that 
   * are tangent to the unit circle.
   *
   * @param x       The list of point coordinates (3 per vertex).
   * @param g       The vector in which to compute the gradient.
   * @param context A pointer to a vb::Map.
   */

  double Map_fg_circle_disk (const Vector &x, Vector &g, void *context);
}

#endif

