#pragma once
#include <vb/Figure.h>
#include <vb/LinearAlgebra.h>

namespace vb {
	using Edge = std::pair<int,int>;
	using adj_list = std::vector<long>;

	class Vertex : public Circle { public:
		adj_list adj;
		Vertex (const cpx &z = cpx(0.0,0.0), const double &r = 0.0) : Circle(z,r) {};
		Vertex (const double &z, const double &r = 0.0) : Circle(z,r) {};
	};

	class Map : public Picture { public:
		int n;
		std::vector<std::unique_ptr<Vertex>> v;
		std::vector<bool> bd;

		int zero;
		int one;
		int infinity;
		double scale=1;

		Map (int nn);

		adj_list::iterator find_edge (const Edge &e) const;

		Edge turn_left  (const Edge &e) const;
		Edge turn_right (const Edge &e) const;

		void add_before (const Edge &e, int vv);
		void add_after (const Edge &e, int vv);

		std::vector<int> face (Edge e);

		void inscribe (const std::vector<int> &face_ext, const double &radius = 1.0, bool reverse = false);

		double balance ();
		void balance_old ();

		std::vector<int> split_edges ();

		void hex_to_triangle (const std::vector<int> &f);
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

		void mobius (cpx w, const double &theta);

		void rad_to_pos (int _zero, int _one);

		void rotate (const double &theta);

		void mobius_circle (cpx w, double r);

		double left ();
		double right ();
		double top ();
		double bottom ();

		double fg_balance (const Vector<double> &x, Vector<double> &g);
		double fg_circle_base (const Vector<double> &x, Vector<double> &g);
		double fg_circle_bd (const Vector<double> &x, Vector<double> &g);
		double fg_circle_disk (const Vector<double> &x, Vector<double> &g);

	protected:
		void paint () override;
	};

	std::ostream &operator<< (std::ostream &os, const Map &m);

	Map &operator<< (Map &m, const Edge &e);
}
