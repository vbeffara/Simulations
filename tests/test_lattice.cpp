#include <vb/Lattice.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>
#include <iomanip>

using namespace vb;
using namespace std;

Lattice G () {
	Lattice G(12, cpx(0,sqrt(6.0/7)));

	G.bond(0,1).bond(1,0,coo(1,0)).bond(0,2).bond(0,11).bond(0,3).bond(1,3).bond(1,4).bond(4,0,coo(1,0));
	G.bond(2,11).bond(8,11).bond(5,11).bond(5,8).bond(6,11).bond(5,6).bond(6,8).bond(3,11).bond(3,6);
	G.bond(3,7).bond(3,4).bond(6,7).bond(6,9).bond(4,7).bond(4,10).bond(4,2,coo(1,0)).bond(10,2,coo(1,0));
	G.bond(2,8).bond(2,0,coo(0,1)).bond(8,0,coo(0,1)).bond(8,9).bond(9,0,coo(0,1)).bond(7,9).bond(7,10);
	G.bond(9,10).bond(9,1,coo(0,1)).bond(10,1,coo(0,1)).bond(10,0,coo(1,1));
	return G;
}

int main (int, char **) {
	Lattice L = SV();
	L.relax(1e-14);
	L.tau = L.tau_rw();
	L.optimize (cost_cp);
	H.L->info ("Modulus: tau = {}", L.tau);

	Pen p (Color(255,0,0),1,Color(255,255,0));

	Figure F;

	vector<cpx> fd;
	fd.push_back(L(coo(1,1)));
	fd.push_back(L(coo(2,1)));
	fd.push_back(L(coo(2,2)));
	fd.push_back(L(coo(1,2)));
	F.add (std::make_unique <Polygon> (fd, Pen(0,0,200,true)));

	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			for (unsigned k=0; k<L.n; ++k)
				F.add (std::make_unique <Circle> (L(coo(i,j),k), L.r[k], Pen(0,.2)));

	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j)
			for (unsigned k=0; k<L.n; ++k)
				for (unsigned l=0; l<L.adj[k].size(); ++l)
					F.add (std::make_unique <Segment> (L(coo(i,j),k), L(coo(i,j),k) + L.shift(k,l), Pen(0,.1)));

	F.show(); F.pause(); F.output_pdf();
	return 0;
}
