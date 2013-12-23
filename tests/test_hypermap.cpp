#include <vb/CL_Parser.h>
#include <vb/Figure.h>
#include <vb/Hypermap.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <cassert>

using namespace vb;
using namespace std;

double alpha_xyz (double x, double y, double z) { return acos (((x+y)*(x+y)+(x+z)*(x+z)-(y+z)*(y+z))/(2*(x+y)*(x+z))); }

class Toroidal : public Hypermap { // Triangulation of torus
	public:
		Toroidal (const Hypermap &H) : Hypermap(H),
				src(n_edges()), adj(n_black()), rad(n_black(),1), angle(n_edges(),NAN), place(n_black(),NAN), circ(n_black(),true) {
			assert(is_triangulation());
            for (int i=0; i<n_black(); ++i) for (int e : sigma.c[i]) src[e]=i;
			for (int i=0; i<n_black(); ++i) for (int e : sigma.c[i]) adj[i].push_back(src[alpha[e]]); 
		}

		void acpa (double r) {
			double e = 1, old_e = 2;
			while ((e > 1e-3) || (e < old_e)) {
				cerr << e << "     \r";
				old_e = e; e = 0;
				for (int i=0; i<n_black(); ++i) {
					double s=-2*M_PI, r0=rad[i], r1, r2=rad[adj[i].back()];
					for (int ll : adj[i]) { r1=r2; r2=rad[ll]; s += alpha_xyz (r0,r1,r2); }
					rad[i] *= 1 + r*s/(2*M_PI); if (rad[i]<0) rad[i] /= 1 + r*s/(2*M_PI); ;
					e += abs(s);
				}
			}
		}

		void pack (double r) {
			acpa(r);

			angle[0]=0;
			bool flag=true; while (flag) { flag = false;
				for (int i=0; i<n_edges(); ++i) {
					if (isnan(angle[i]))         	continue;
					if (isnan(angle[alpha[i]])) {	angle[alpha[i]] = angle[i]+M_PI; flag = true; }
					if (isnan(angle[sigma[i]])) {	double x = rad[src[i]], y = rad[src[alpha[i]]], z = rad[src[alpha[sigma[i]]]];
					                             	angle[sigma[i]] = angle[i] + alpha_xyz(x,y,z); flag = true; }
				}
			}

			place[0]=0; vector<cpx> periods;
			flag=true; while (flag) { flag=false; periods.clear();
				for (int e=0; e<n_edges(); ++e) {
					int i=src[e]; if (isnan(real(place[i]))) continue;
					int j=src[alpha[e]]; double l = rad[i] + rad[j];
					cpx z = place[i] + cpx(l*cos(angle[e]),l*sin(angle[e]));
					if (isnan(real(place[j]))) { flag = true; place[src[alpha[e]]] = z; }
					else if (abs(place[j]-z) > rad[0]/2) { 
						bool fresh = true;
						for (cpx p : periods) if (abs(place[j]-z-p) < rad[0]/2) fresh = false;
						if (fresh) periods.push_back(place[j]-z); 
					}
				}
			}

			cpx p1 = periods[0]; for (cpx p : periods) if (abs(p)<abs(p1)) { p1=p; }

			for (cpx   	&z : place)  	z /= p1;
			for (double	&a : angle)  	a -= arg(p1);
			for (cpx   	&p : periods)	p /= p1;
			for (double	&r : rad)    	r /= abs(p1);

			vector<cpx> moduli; for (cpx p : periods) if (abs(imag(p)) > .1) moduli.push_back(p);
			double n2 = abs(moduli[0]); m = moduli[0];
			for (cpx p : moduli) if (abs(p)<n2) { n2=abs(p); m=p; }
			if (imag(m)<0) m = -m; while (real(m)<-.5) m+=1; while (real(m)>.5) m-=1;

			double slope = real(m) / imag(m);
			for (cpx &z : place) {
				while (imag(z) < 0)                  	z += m;
				while (imag(z) > imag(m))            	z -= m;
				while (real(z) < slope * imag(z))    	z += 1;
				while (real(z) > slope * imag(z) + 1)	z -= 1;
			}
		}

		void output_pdf (int E=0) {
			if (E>0) {
				for (int i=0; i<n_black(); ++i) circ[i] = false;
				for (int e=0; e<E; ++e) { int ee=e; do { circ[src[ee]]=true; ee = sigma[sigma[sigma[alpha[ee]]]]; } while (!circ[src[ee]]); }
			}
			double slope = real(m) / imag(m);
			Figure F;
			F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,200,1)));
			vector<cpx> eee;
			for (int a=-1; a<=2; ++a)
				for (int b=-1; b<=1; ++b)
					for (int e=0; e<n_edges(); ++e) {
						int i=src[e]; cpx z = place[i] + cpx(a) + cpx(b)*m;
						if ((imag(z)<-.6)||(imag(z)>1.6)) continue;
						if ((real(z)<-.6+slope*imag(z)) || (real(z)>2.6+slope*imag(z))) continue;
						if ((e==sigma.c[i][0]) && circ[i]) F.add (new Circle (z,rad[i],Pen(0,.12)));
						if (!circ[i]) { eee.push_back(z); eee.push_back(z+cpx(rad[i]*cos(angle[e]),rad[i]*sin(angle[e]))); eee.push_back(NAN); }
					}
			F.add (new Path (eee,Pen(0,.06)));
			F.output_pdf("period");
		}

		vector<int> src;
		vector<vector<int>> adj;
		vector<double> rad,angle;
		vector<cpx> place;
		vector<bool> circ;
		cpx m;
};

int main (int argc, char ** argv) {
	// Triangle on the sphere
	Hypermap T;
	T.sigma = {{0,1},{2,3},{4,5}};
	T.alpha = {{0,2},{3,4},{1,5}};
	T.phi   = {{0,3,5},{1,4,2}};

	// Two triangles, this one is invalid but some of the algorithms work.
	Hypermap TT;
	TT.sigma    = {{0,1},{2,3},{4,5}, {6,7},{8,9},{10,11}};
	TT.alpha    = {{0,2},{3,4},{1,5}, {6,8},{9,10},{7,11}};
	TT.phi      = {{0,3,5},{1,4,2},   {6,9,11}, {7,10,8}};

	// This is the planar hypermap in Zvonkine's paper. Encode using cycles.
	Hypermap H1;
	H1.sigma    = { { 0, 1, 2 }, { 3, 4 }, { 5 }, { 6, 7, 8 } };
	H1.alpha    = { { 0, 3 }, { 1, 8, 2 }, { 4, 5, 6 }, { 7 } };
	H1.phi      = { { 0, 4, 8 }, { 1 }, { 2, 7, 6, 5, 3 } };

	// This is a toric graph whose lift is the centered square lattice. Encode using images.
	Hypermap H2;
	H2.sigma    = { 1, 2, 3, 4, 5, 6, 7, 0, 11, 8, 9, 10 };
	H2.alpha    = { 4, 11, 6, 10, 0, 9, 2, 8, 7, 5, 3, 1 };
	H2.phi      = { 3, 8, 5, 11, 7, 10, 1, 9, 6, 4, 2, 0 };

	// The envelope-house in Zvonkine.
	Hypermap H3;
	H3.sigma    = {{0,1,2},{3,4},{5,6,7,8},{9,10},{11,12,13}};
	H3.alpha    = {{0,13},{1,6},{2,3},{4,5},{7,12},{8,9},{10,11}};
	H3.phi      = {{0,12,6},{1,5,3},{2,4,8,10,13},{7,11,9}};

	// Hyperbolic surface of genus 2 from the octogon.
	Hypermap O;
	O.sigma = {3,2,4,1,7,6,0,5};
	O.alpha = {{0,1},{2,3},{4,5},{6,7}};
	O.phi   = {3,6,0,1,7,2,4,5};

	// C5 graph
	Hypermap C5;
	C5.sigma = {{0,29,15,16,6,12,5},{1,2,19,27},{7,25,18,20,8},{9,22,28,26,24,17,10,13},{3,4,11,14,23,21}};
	C5.alpha = {{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23},{24,25},{26,27},{28,29}};
	C5.phi   = {{11,17,15},{4,12,10},{6,8,13},{20,23,9},{1,5,3},{18,2,21},{16,24,7},{19,25,26},{0,27,28},{22,14,29}};

	CL_Parser CLP (argc,argv,"n=4,r=2.6");
	Hypermap G=C5;

	for (int i=0; i<int(CLP('n')); ++i) G = G.split_edges(); cerr << G;

	Toroidal H(G);
	H.pack(CLP('r')); cerr << H.m << endl;
	H.output_pdf(30);
	H.output_graph_dot(cout);
}
