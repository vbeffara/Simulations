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

class Vertex	{ public: cpx z;  	double r;	bool bone; vector<int> adj;	Vertex ()	{ z=NAN; r=1;	bone=false; } };
class Edge  	{ public: int src;	double a;	bool bone;                 	Edge ()  	{ a=NAN;     	bone=false; } };

class Toroidal : public Hypermap { public:
	vector<Vertex>	V;
	vector<Edge>  	E;

	Toroidal (const Hypermap &H) : Hypermap(H), V(n_black()), E(n_edges()) {
		assert(is_triangulation());
		Cycles sc = sigma.cycles(); int nb=n_black();
        for (int i=0; i<nb; ++i) for (int e : sc[i]) E[e].src=i;
		for (int i=0; i<nb; ++i) for (int e : sc[i]) V[i].adj.push_back(E[alpha[e]].src);
	}

	void acpa (double r) {
		double e = 1, old_e = 2;
		int n = n_black();
		while ((e > 1e-3) || (e < old_e)) {
			cerr << e << "     \r";
			old_e = e; e = 0;
			for (int i=0; i<n; ++i) {
				double s=-2*M_PI, r0=V[i].r, r1, r2=V[V[i].adj.back()].r;
				for (int ll : V[i].adj) { r1=r2; r2=V[ll].r; s += alpha_xyz (r0,r1,r2); }
				V[i].r *= 1 + r*s/(2*M_PI); if (V[i].r<0) V[i].r /= 1 + r*s/(2*M_PI); ;
				e += abs(s);
			}
		}
	}

	void pack (double r, bool flip = false) {
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
				else if (abs(V[j].z-z) > V[0].r/2) {
					bool fresh = true;
					for (cpx p : periods) if (abs(V[j].z-z-p) < V[0].r/2) fresh = false;
					if (fresh) periods.push_back(V[j].z-z);
				}
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

		if (flip) { for (auto & e : E) e.a += M_PI; for (auto & v : V) v.z = m-v.z; }

		double slope = real(m) / imag(m);
		for (auto & v : V) {
			while (imag(v.z) < 0)                    	v.z += m;
			while (imag(v.z) > imag(m))              	v.z -= m;
			while (real(v.z) < slope * imag(v.z))    	v.z += 1;
			while (real(v.z) > slope * imag(v.z) + 1)	v.z -= 1;
		}
	}

	void output_pdf (int EE=0, int mode=255) {
		for (int e=0; e<EE; ++e) {
			int ee=e; do {
				E[ee].bone=true; E[alpha[e]].bone=true; V[E[ee].src].bone=true; ee = sigma[sigma[sigma[alpha[ee]]]];
			} while (!E[ee].bone);
		}
		double slope = real(m) / imag(m);
		Figure F;
		F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,200,1)));
		vector<cpx> eee; unsigned ne=n_edges(); Cycles cc=sigma.cycles();
		for (int a=-2; a<=3; ++a)
			for (int b=-1; b<=1; ++b)
				for (unsigned e=0; e<ne; ++e) {
					int i=E[e].src; cpx z = V[i].z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.6)) continue;
					if ((real(z)<-.6+0*slope*imag(z)) || (real(z)>2.6+0*slope*imag(z))) continue;
					if (e == cc[i][0])
						if ( ((mode&1)&&(V[i].bone)) || ((mode&2)&&(!V[i].bone)) )
							F.add (new Circle (z,V[i].r,Pen(0,.15)));
					if ( ((mode&4)&&(E[e].bone)) || ((mode&8)&&(V[i].bone)) || ((mode&16)&&(!V[i].bone)) ) {
						eee.push_back(z);
						eee.push_back(z+polar(V[i].r,E[e].a));
						eee.push_back(NAN);
					}
				}
		if (eee.size()) F.add (new Path (eee,Pen(0,.06)));
		F.output_pdf("period");
	}

	cpx m;
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"n=4,o=0,r=2.6,m=4,a=3,b");
	Hypermap G = H_C5();

	for (int i=0; i<int(CLP('o')); ++i) G = G.split_edges();
	int n_skel = G.n_edges();

	cerr << setprecision(15); cout << setprecision(15);
	for (int i=CLP('o'); i<=int(CLP('n')); ++i) {
		cerr << "Step " << i << ": " << G; Toroidal H(G); H.pack(CLP('r'),CLP('b'));
		if (i==int(CLP('n'))) H.output_pdf(n_skel,CLP('m'));

		cpx q = exp(I * M_PI * H.m);
		cpx theta2(0), old_theta2(-1); for (int n=0; theta2 != old_theta2; ++n) { old_theta2=theta2; theta2 += 2.0 * pow (q, (n+.5)*(n+.5)); }
		cpx theta3(1), old_theta3(-1); for (int n=1; theta3 != old_theta3; ++n) { old_theta3=theta3; theta3 += 2.0 * pow (q, n*n); }
		cpx lambda = pow(theta2/theta3,4);
		cpx j = 256.0 * pow(1.0-lambda+pow(lambda,2),3) / pow(lambda*(1.0-lambda),2);

		cerr << "    tau = " << H.m << endl;
		cerr << "j (tau) = " << j << endl;

		G = G.split_edges();
	}
}
