#include <vb/Map.h>
#include <vb/Minimizer.h>

using namespace vb;

void cb (const Vector<double> &, double fx) { H.L->trace ("Current : {}", fx); }

int main (int argc, char ** argv) {
	H.init ("Test: circle packing", argc, argv, "s=4,v");

	Map m (13);
	m	<< Edge(0,1) << Edge(0,3) << Edge(0,5)
	 	<< Edge(1,2) << Edge(1,4) << Edge(1,6) << Edge(1,3) << Edge(1,0)
	 	<< Edge(2,7) << Edge(2,4) << Edge(2,1)
	 	<< Edge(3,0) << Edge(3,1) << Edge(3,6) << Edge(3,5)
	 	<< Edge(4,1) << Edge(4,2) << Edge(4,7) << Edge(4,6)
	 	<< Edge(5,0) << Edge(5,3) << Edge(5,6) << Edge(5,8) << Edge(5,10)
	 	<< Edge(6,1) << Edge(6,4) << Edge(6,7) << Edge(6,9) << Edge(6,11) << Edge(6,8) << Edge(6,5) << Edge(6,3)
	 	<< Edge(7,12) << Edge(7,9) << Edge(7,6) << Edge(7,4) << Edge(7,2)
	 	<< Edge(8,5) << Edge(8,6) << Edge(8,11) << Edge(8,10)
	 	<< Edge(9,6) << Edge(9,7) << Edge(9,12) << Edge(9,11)
	 	<< Edge(10,5) << Edge(10,8) << Edge(10,11)
	 	<< Edge(11,10) << Edge(11,8) << Edge(11,6) << Edge(11,9) << Edge(11,12)
	 	<< Edge(12,11) << Edge(12,9) << Edge(12,7);

	for (int i=0; i<int(H['s']); ++i) m.barycentric();
	m.inscribe(m.face(Edge(1,m.v[1]->adj.back()))); m.balance(); m.show();

	Vector<double> x(3*m.n); double r = 1.0/sqrt(m.n);

	for (long i=0; i<m.n; ++i) {
		x[3*i]        = m.v[i]->z.real() / (1-r);
		x[3*i+1]      = m.v[i]->z.imag() / (1-r);
		x[3*i+2]      = .8*r;
	}

	Minimizer<double> MM (3*m.n, [&m](const Vector<double> &x, Vector<double> * g) { return m.fg_circle_disk (x,g); });
	if (H['v']) { MM.cb = cb; spdlog::set_level(spdlog::level::trace); }
	MM.minimize_qn (x); x = MM.x;

	H.L->info ("Number of vertices:    {}", m.n);
	H.L->info ("Final value of f:      {}", MM.fx);
	H.L->info ("Final square gradient: {}", MM.gx.squaredNorm());

	for (long i=0; i<m.n; ++i) {
		m.v[i]->z = cpx (x[3*i], x[3*i+1]);
		m.v[i]->r = x[3*i+2];
	}

	Figure f; m.plot_circles(&f); f.add (std::make_unique <Circle> (cpx(0.0,0.0),1.0)); f.show(); f.pause(); f.output ();
}
