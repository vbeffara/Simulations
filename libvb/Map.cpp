#include <vb/Map.h>
#include <vb/Minimizer.h>

namespace vb {
	Map::Map (int nn) : Picture (600,600), n(nn), zero(-1), one(-1), infinity(-1) {
		for (int i=0; i<n; ++i) {
			v.push_back(std::make_unique<Vertex>(0.0));
			bd.push_back(false);
		}
	}

	double Map::left () {
		double l = 0.0;
		for (int i=0; i<n; ++i) l = std::min (l, v[i]->z.real());
		return l;
	}

	double Map::right () {
		double l = 0.0;
		for (int i=0; i<n; ++i) l = std::max (l, v[i]->z.real());
		return l;
	}

	double Map::top () {
		double l = 0.0;
		for (int i=0; i<n; ++i) l = std::max (l, v[i]->z.imag());
		return l;
	}

	double Map::bottom () {
		double l = 0.0;
		for (int i=0; i<n; ++i) l = std::min (l, v[i]->z.imag());
		return l;
	}

	void Map::paint () {
		double width  = right()-left(), mid_x = (right()+left())/2;
		double height = top()-bottom(), mid_y = (top()+bottom())/2;
		double scale_x = w()/width, scale_y = h()/height;

		double scale = std::min (scale_x, scale_y);

		cairo_save(cr);
		cairo_set_source_rgb (cr, 1,1,1);
		cairo_paint(cr);
		cairo_restore(cr);

		cairo_save(cr);
		cairo_translate (cr, w()/2,h()/2);
		cairo_scale (cr, scale,scale);
		cairo_translate (cr, -mid_x,-mid_y);
		cairo_set_source_rgb (cr, 0,0,0);
		cairo_set_line_width (cr, .5/scale);

		for (int i=0; i<n; ++i) {
			for (int j : v[i]->adj) {
				cairo_move_to (cr, v[i]->z.real(), v[i]->z.imag());
				cairo_line_to (cr, v[j]->z.real(), v[j]->z.imag());
			}
		}

		cairo_stroke(cr);
		cairo_restore(cr);
	}

	adj_list::iterator Map::find_edge (const Edge &e) const {
		if (e.first >= n) return v[0]->adj.end();
		for (auto i = v[e.first]->adj.begin(); i != v[e.first]->adj.end(); ++i)
			if (*i == e.second) return i;
		return v[0]->adj.end();
	}

	Edge Map::turn_left (const Edge &e) const {
		auto ee = find_edge (Edge(e.second,e.first));
		if (ee == v[e.second]->adj.begin()) ee = v[e.second]->adj.end();
		--ee;
		return Edge(e.second,*ee);
	}

	Edge Map::turn_right (const Edge &e) const {
		auto ee = find_edge (Edge(e.second,e.first));
		++ee;
		if (ee == v[e.second]->adj.end()) ee = v[e.second]->adj.begin();
		return Edge(e.second,*ee);
	}

	void Map::add_before (const Edge &e, int vv) {
		auto ee = find_edge(e);
		v[e.first]->adj.insert (ee,vv);
	}

	void Map::add_after (const Edge &e, int vv) {
		auto ee = find_edge(e);
		++ee;
		v[e.first]->adj.insert (ee,vv);
	}

	std::vector<int> Map::face (Edge e) {
		std::vector<int> l;
		int first = e.first; l.push_back(first);
		e = turn_left(e);
		while (e.first != first) {
			l.push_back(e.first);
			e = turn_left(e);
		}
		return l;
	}

	void Map::inscribe (const std::vector<int> &face_ext, const double &radius, bool reverse) {
		int n_ext = face_ext.size();

		for (int i=0; i<n; ++i) { bd[i] = false; }
		scale = radius;

		int k=0;
		for (int i : face_ext) {
			--k;
			bd[i] = true;
			double angle = (reverse?-2.0:2.0)*3.1415927*k/n_ext;
			v[i]->z = radius * cpx(cos(angle),sin(angle));
		}
	}

	double Map::balance () {
		Vector<double> x(2*n);

		for (long i=0; i<n; ++i) {
			x[2*i]   = v[i]->z.real();
			x[2*i+1] = v[i]->z.imag();
		}

		Minimizer<double> M (2*n, [this](const Vector<double> &x, Vector<double> &g) { return fg_balance (x,g); });
		double output = M.minimize_qn (x);

		for (long i=0; i<n; ++i) {
			v[i]->z = cpx (M.x[2*i],M.x[2*i+1]);
		}

		update();
		return output;
	}

	void Map::balance_old () {
		bool dirty=true;

		while (dirty) {
			dirty = false;
			for (int i=0; i<n; ++i) {
				if (bd[i]) continue;
				if (v[i]->adj.size() == 0) continue;

				cpx old = v[i]->z; v[i]->z = 0.0;
				for (int j : v[i]->adj) v[i]->z += v[j]->z;
				v[i]->z /= v[i]->adj.size();
				if (v[i]->z != old) dirty = true;
			}
		}
		update();
	}

	std::vector<int> Map::split_edges () {
		std::vector<int> tmp; tmp.reserve(n*n);
		for (int i=0; i<n*n; ++i) tmp.push_back(-1);

		std::vector<int> output;
		std::vector<adj_list> new_vertices;

		for (int v1=0; v1<n; ++v1) {
			for (auto v2 = v[v1]->adj.begin(); v2 != v[v1]->adj.end(); ++v2) {
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

		for (adj_list i : new_vertices) {
			v.push_back(std::make_unique<Vertex>(0.0));
			v.back()->adj = i;
			v.back()->z = (v[i.front()]->z + v[i.back()]->z)/double(2.0);
			bd.push_back(false);
			++n;
		}

		return output;
	}

	void Map::hex_to_triangle (const std::vector<int> &f) {
		auto i = f.begin();
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

	void Map::barycentric () {
		std::vector<int> l = split_edges();
		for (int i : l) {
			std::vector<int> f = face(Edge(i,v[i]->adj.front()));
			if (f.size() == 6) hex_to_triangle(f);

			f = face(Edge(i,v[i]->adj.back()));
			if (f.size() == 6) hex_to_triangle(f);
		}
	}

	void Map::print_as_dot (std::ostream &os) {
		os << "digraph G {" << std::endl;
		for (int i=0; i<n; ++i)
			for (int j : v[i]->adj) {
				if (find_edge(Edge(j,i)) == v[0]->adj.end())
					os << "  " << i << " -> " << j << ";" << std::endl;
				else if (i<=j)
					os << "  " << i << " -> " << j << " [arrowhead = none]" << std::endl;
			}
		os << "}" << std::endl;
	}

	void Map::plot_vertices (Figure * F) {
		for (int i=0; i<n; ++i) {
			F->add (std::make_unique <Dot> (v[i]->z));
		}
	}

	void Map::plot_edges (Figure * F) {
		for (int i=0; i<n; ++i) {
			for (int j : v[i]->adj)
				if ((i<j) || (find_edge(Edge(j,i)) == v[0]->adj.end())) F->add (std::make_unique <Segment> (v[i]->z,v[j]->z));
		}
	}

	void Map::plot_circles (Figure * F) {
		for (int i=0; i<n; ++i) {
			if (v[i]->r>0) F->add (std::make_unique <Circle> (v[i]->z,v[i]->r));
		}
	}

	int Map::nb_aretes () {
		int tmp = 0;
		for (int i=0; i<n; ++i) tmp += v[i]->adj.size();
		return (tmp/2);
	}

	int Map::nb_faces () {
		double tmp = 0.0;
		for (int i=0; i<n; ++i)
			for (int j : v[i]->adj)
				tmp += double(1.0) / double(face(Edge(i,j)).size());
		return floor (tmp + .1);
	}

	int Map::euler () {
		return nb_sommets() - nb_aretes() + nb_faces();
	}

	int Map::genre () {
		return 1 - (euler()/2);
	}

	void Map::mobius (cpx w, const double &theta) {
		for (int i=0; i<n; ++i) {
			cpx z = v[i]->z / scale;
			w /= scale;
			v[i]->z = scale * (z-w)*cpx(cos(theta),sin(theta))/(cpx(1,0)-z*conj(w));
		}
	}

	void Map::rad_to_pos (int _zero, int _one) {
		zero = _zero;
		one = _one;

		double R_max = 0.0;
		for (int i=0; i<n; ++i) if (v[i]->r > R_max) R_max = v[i]->r;

		cpx nowhere (-2*n*R_max, 0.0);
		for (int i=0; i<n; ++i) v[i]->z = nowhere;

		v[zero]->z = cpx(0.0,0.0);
		v[one]->z = cpx (v[zero]->r + v[one]->r, 0.0);

		bool dirty = true; while (dirty) {
			dirty = false;

			for (int i=0; i<n; ++i) {
				if (v[i]->z == nowhere) continue;
				if (v[i]->adj.size() == 0) continue;

				cpx prev_z = v[v[i]->adj.back()]->z;
				double prev_r = v[v[i]->adj.back()]->r;

				for (int j : v[i]->adj) {
					cpx z = v[j]->z;
					if ((prev_z != nowhere) && (z == nowhere) && ((!bd[i])||(!bd[j]))) {
						double x = v[i]->r;
						double y = v[j]->r;
						double t = (((x+y)*(x+y) + (x+prev_r)*(x+prev_r) - (y+prev_r)*(y+prev_r)) / (2*(x+y)*(x+prev_r)));
						if (t<-1.0) t=-1.0;
						if (t>1.0) t=1.0;
						double alpha = acos(t);
						alpha += arg(prev_z-v[i]->z);
						v[j]->z = v[i]->z + cpx((x+y)*cos(alpha), (x+y)*sin(alpha));
						z = v[j]->z;
						dirty = true;
					}
					prev_z = z; prev_r = v[j]->r;
				}
			}
		}
	}

	void Map::rotate (const double &theta) {
		for (int i=0; i<n; ++i) v[i]->z *= cpx(cos(theta),sin(theta));
	}

	void Map::mobius_circle (cpx w, double r) {
		w /= scale;
		r /= scale;

		cpx Delta ( (1+norm(w)-r*r)*(1+norm(w)-r*r) - 4*norm(w), 0 );
		cpx x = (cpx(1+norm(w)-r*r) + sqrt(Delta)) / (cpx(2)*conj(w));
		if (norm(x)>1) x = (cpx(1+norm(w)-r*r) - sqrt(Delta)) / (cpx(2)*conj(w));

		for (int i=0; i<n; ++i) {
			cpx W = v[i]->z/scale;
			double R = v[i]->r/scale;

			cpx A = norm(double(1) - W*conj(x)) - cpx(R*R*norm(x),0);
			cpx B = (cpx(1,0) - W*conj(x)) * (conj(x)-conj(W)) - cpx(R*R,0)*conj(x);
			cpx C = (x-W)*(conj(x)-conj(W)) - cpx(R*R,0);

			v[i]->z = scale * conj(-B/A);
			v[i]->r = scale * sqrt(abs(norm(v[i]->z/scale) - C/A));
		}
	}

	std::ostream &operator<< (std::ostream &os, const Map &m) {
		os << m.n << " vertices:" << std::endl;
		for (int i=0; i<m.n; ++i) {
			os << "  " << i << " ->";
			for (int j : m.v[i]->adj) os << " " << j;
			os << std::endl;
		}
		return os;
	}

	Map &operator<< (Map &m, const Edge &e) {
		m.v[e.first]->adj.push_back(e.second);
		return m;
	}

	double Map::fg_balance (const Vector<double> &x, Vector<double> &g) {
		double c = 0.0;

		for (long i=0; i < n; ++i) {
			g[2*i] = 0;
			g[2*i+1] = 0;

			for (long j : v[i]->adj) {
				c += (x[2*j] - x[2*i]) * (x[2*j] - x[2*i]);
				c += (x[2*j+1] - x[2*i+1]) * (x[2*j+1] - x[2*i+1]);
				if (!(bd[i])) {
					g[2*i]    -= x[2*j] - x[2*i];
					g[2*i+1]    -= x[2*j+1] - x[2*i+1];
				}
			}
		}

		return c;
	}

	double Map::fg_circle_base (const Vector<double> &x, Vector<double> &g) {
		double c = 0.0;

		g = Vector<double>::Zero (g.rows(),1);

		for (long i=0; i < n; ++i) {
			for (long j : v[i]->adj) {
				double dx = x[3*j]-x[3*i];
				double dy = x[3*j+1]-x[3*i+1];
				double l = sqrt(dx*dx + dy*dy);
				double sr = x[3*i+2] + x[3*j+2];
				double lsr = l-sr;
				double lsrl = lsr/l;

				c += lsr * lsr;

				g[3*i]   -= lsrl*dx;
				g[3*i+1] -= lsrl*dy;
				g[3*i+2] -= lsr;
			}
		}

		return c;
	}

	double Map::fg_circle_bd (const Vector<double> &x, Vector<double> &g) {
		double c = fg_circle_base (x,g);

		for (long i=0; i < n; ++i)
			if (bd[i])
				g[3*i+2] = 0.0;

		return c;
	}

	double Map::fg_circle_disk (const Vector<double> &x, Vector<double> &g) {
		double c = fg_circle_base (x,g);

		for (long i=0; i < n; ++i) {
			double l2 = x[3*i]*x[3*i] + x[3*i+1]*x[3*i+1];
			double l = sqrt(l2);
			double r = x[3*i+2];

			if ((bd[i]) || (l+r>1.0)) {
				double lr1 = l+r-1.0;
				double lr1r = lr1/l;

				c += .1*lr1*lr1;

				g[3*i]   += .1*lr1r*x[3*i];
				g[3*i+1] += .1*lr1r*x[3*i+1];
				g[3*i+2] += .1*lr1;
			}
		}

		return c;
	}
}
