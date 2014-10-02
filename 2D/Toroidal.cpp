// Mode: 1 -> disks along the bones,
//       2 -> disks elsewhere,
//       4 -> edges along the bones,
//       8 -> edges around the bones,
//      16 -> edges elsewhere.
// Good modes are:
//    3 -> circle packing;
//    4 -> just the bones;
//   17 -> chain of empty disks plus small triangles;
//   24 -> whole triangulation;

#include <vb/CL_Parser.h>
#include <vb/Figure.h>
#include <vb/Hypermap_lib.h>
#include <vb/PRNG.h>
#include <iomanip>

using namespace vb;
using namespace std;

double alpha_xyz (double x, double y, double z) { return acos (((x+y)*(x+y)+(x+z)*(x+z)-(y+z)*(y+z))/(2*(x+y)*(x+z))); }

class Vertex	{ public: cpx z;  	double r;	bool bone; vector<int> adj; vector<unsigned> edges;	Vertex () { r=1; } };
class Edge  	{ public: int src;	double a;	bool bone; };

class Toroidal : public Hypermap { public:
	vector<Vertex>	V;
	vector<Edge>  	E;

	int n_skel;
	cpx m;
	Hub H;

	Toroidal (Hub H_) : Hypermap(H_lib(H_['g'])), n_skel(n_edges()), H(H_) {
		for (int i=0; i<int(H['o']); ++i) Hypermap::operator= (Hypermap::split_edges());
		from_hypermap();
	}

	void split_edges () {
		Hypermap::operator= (Hypermap::split_edges());
		from_hypermap();
		for (int i=0; i<V.size()/4; ++i) for (int j : V[i].adj) V[j].r = V[i].r;
	}

	void from_hypermap () {
		assert(is_triangulation());
		Cycles sc = sigma.cycles(); int nb = sc.size();
		V.resize(nb); E.resize(6*nb);
        int i=0;
		for (auto & v : V) {
			v.z = NAN; v.bone=false; v.adj.clear(); v.edges = sc[i];
			for (int e : v.edges) { E[e].a = NAN; E[e].bone = false; E[e].src = i; }
			++i;
		}
		for (auto & v : V) for (int e : v.edges) { v.adj.push_back(E[alpha[e]].src); }
	}

	void acpa (double r) {
		double e = 1, old_e = 2;
		while ((e > 1e-3) || (e < old_e)) {
			cerr << e << "     \r";
			old_e = e; e = 0;
			for (auto & v : V) {
				double s=-2*M_PI, r0=v.r, r1, r2=V[v.adj.back()].r;
				for (int ll : v.adj) { r1=r2; r2=V[ll].r; s += alpha_xyz (r0,r1,r2); }
				v.r *= 1 + r*s/(2*M_PI); if (v.r<0) v.r /= 1 + r*s/(2*M_PI); ;
				e += abs(s);
			}
		}
	}

	void pack (double r) {
		acpa(r);

		E[0].a=0; int ne=n_edges();
		bool flag=true; while (flag) { flag = false;
			for (int i=0; i<ne; ++i) {
				if (std::isnan(E[i].a))         	continue;
				if (std::isnan(E[alpha[i]].a)) {	E[alpha[i]].a = E[i].a+M_PI; flag = true; }
				if (std::isnan(E[sigma[i]].a)) {	double x = V[E[i].src].r, y = V[E[alpha[i]].src].r, z = V[E[alpha[sigma[i]]].src].r;
				                                	E[sigma[i]].a = E[i].a + alpha_xyz(x,y,z); flag = true; }
			}
		}

		V[0].z=0; vector<cpx> periods;
		flag=true; while (flag) { flag=false; periods.clear();
			for (int e=0; e<ne; ++e) {
				int i=E[e].src; if (std::isnan(real(V[i].z))) continue;
				int j=E[alpha[e]].src; double l = V[i].r + V[j].r;
				cpx z = V[i].z + polar(l,E[e].a);
				if (std::isnan(real(V[j].z))) { flag = true; V[E[alpha[e]].src].z = z; }
				else if (abs(V[j].z-z) > V[0].r/2) { periods.push_back(V[j].z-z); }
			}
		}

		cpx p1 = periods[0]; for (cpx p : periods) if (abs(p)<abs(p1)) { p1=p; }

		for (auto	& v : V)     	{ v.z /= p1; v.r /= abs(p1); }
		for (auto	& e : E)     	{ e.a -= arg(p1); }
		for (cpx 	&p : periods)	p /= p1;

		vector<cpx> moduli; for (cpx p : periods) if (abs(imag(p)) > .1) moduli.push_back(p);
		double n2 = abs(moduli[0]); m = moduli[0];
		for (cpx p : moduli) if (abs(p)<n2) { n2=abs(p); m=p; }
		if (imag(m)<0) m = -m; while (real(m)<-.499) m+=1; while (real(m)>.501) m-=1;

		double slope = real(m) / imag(m);
		for (auto & v : V) {
			while (imag(v.z) < 0)                    	v.z += m;
			while (imag(v.z) > imag(m))              	v.z -= m;
			while (real(v.z) < slope * imag(v.z))    	v.z += 1;
			while (real(v.z) > slope * imag(v.z) + 1)	v.z -= 1;
		}
	}

	void flip() {
		for (auto & e : E) e.a += M_PI;
		for (auto & v : V) v.z = 1.0+m-v.z;
	}

	void output_pdf () {
		for (int e=0; e<n_skel; ++e) {
			int ee=e; do {
				E[ee].bone=true; E[alpha[e]].bone=true; V[E[ee].src].bone=true; ee = sigma[sigma[sigma[alpha[ee]]]];
			} while (!E[ee].bone);
		}

		Figure F; vector<cpx> eee; int mode = H['m'];
		F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,200,1)));

		for (int a=-2; a<3; ++a) {
			for (int b=-1; b<2; ++b) {
				for (auto v : V) {
					cpx z = v.z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.7)||(real(z)<-.8)||(real(z)>2.6)) continue;
					if ( ((mode&1)&&(v.bone)) || ((mode&2)&&(!v.bone)) ) F.add (new Circle (z,v.r,Pen(0,.15)));
					for (int e : v.edges) if ( ((mode&4)&&(E[e].bone)) || ((mode&8)&&(v.bone)) || ((mode&16)&&(!v.bone)) ) {
						eee.push_back(z);
						eee.push_back(z+polar(v.r,E[e].a));
						eee.push_back(NAN);
					}
				}
			}
		}

		if (eee.size()) F.add (new Path (eee,Pen(0,.06)));
		F.title = H.title; F.output_pdf();
	}
};

int main (int argc, char ** argv) {
	Hub H ("Toroidal triangulation",argc,argv,"n=4,o=0,r=2.7,m=4,f,g=C5");
	Toroidal G (H);

	cerr << setprecision(15); cout << setprecision(15);
	for (int i=H['o']; i<=int(H['n']); ++i) {
		cerr << "Step " << i << ": " << G;
		G.pack(H['r']); if(H['f']) G.flip();

		cpx q = exp(I * M_PI * G.m);
		cpx theta2(0), old_theta2(-1); for (int n=0; theta2 != old_theta2; ++n) { old_theta2=theta2; theta2 += 2.0 * pow (q, (n+.5)*(n+.5)); }
		cpx theta3(1), old_theta3(-1); for (int n=1; theta3 != old_theta3; ++n) { old_theta3=theta3; theta3 += 2.0 * pow (q, n*n); }
		cpx lambda = pow(theta2/theta3,4);
		cpx j = 256.0 * pow(1.0-lambda+pow(lambda,2),3) / pow(lambda*(1.0-lambda),2);

		cerr << "  tau = " << G.m << endl;
		cerr << "  j   = " << j << endl;

		if (i<int(H['n'])) G.split_edges();
	}

	G.output_pdf();
}
