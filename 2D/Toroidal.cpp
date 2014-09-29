#include <vb/CL_Parser.h>
#include <vb/Figure.h>
#include <vb/Hypermap_lib.h>
#include <vb/PRNG.h>
#include <iomanip>

using namespace vb;
using namespace std;

double alpha_xyz (double x, double y, double z) { return acos (((x+y)*(x+y)+(x+z)*(x+z)-(y+z)*(y+z))/(2*(x+y)*(x+z))); }

class Vertex { public:
	Vertex () { z=NAN; }
	cpx z;
};

class Toroidal : public Hypermap { public:
	vector<Vertex> V;

	Toroidal (const Hypermap &H) : Hypermap(H),
			 V(n_black()), src(n_edges()), adj(n_black()), rad(n_black(),1), angle(n_edges(),NAN),
			bone_v(n_black(),false), bone_e(n_edges(),false) {
		assert(is_triangulation());
		Cycles sc = sigma.cycles(); int nb=n_black();
        for (int i=0; i<nb; ++i) for (int e : sc[i]) src[e]=i;
		for (int i=0; i<nb; ++i) for (int e : sc[i]) adj[i].push_back(src[alpha[e]]);
	}

	void acpa (double r) {
		double e = 1, old_e = 2;
		int n = n_black();
		while ((e > 1e-3) || (e < old_e)) {
			cerr << e << "     \r";
			old_e = e; e = 0;
			for (int i=0; i<n; ++i) {
				double s=-2*M_PI, r0=rad[i], r1, r2=rad[adj[i].back()];
				for (int ll : adj[i]) { r1=r2; r2=rad[ll]; s += alpha_xyz (r0,r1,r2); }
				rad[i] *= 1 + r*s/(2*M_PI); if (rad[i]<0) rad[i] /= 1 + r*s/(2*M_PI); ;
				e += abs(s);
			}
		}
	}

	void pack (double r, bool flip = false) {
		acpa(r);

		angle[0]=0; int ne=n_edges();
		bool flag=true; while (flag) { flag = false;
			for (int i=0; i<ne; ++i) {
				if (std::isnan(angle[i]))         	continue;
				if (std::isnan(angle[alpha[i]])) {	angle[alpha[i]] = angle[i]+M_PI; flag = true; }
				if (std::isnan(angle[sigma[i]])) {	double x = rad[src[i]], y = rad[src[alpha[i]]], z = rad[src[alpha[sigma[i]]]];
				                                  	angle[sigma[i]] = angle[i] + alpha_xyz(x,y,z); flag = true; }
			}
		}

		V[0].z=0; vector<cpx> periods;
		flag=true; while (flag) { flag=false; periods.clear();
			for (int e=0; e<ne; ++e) {
				int i=src[e]; if (std::isnan(real(V[i].z))) continue;
				int j=src[alpha[e]]; double l = rad[i] + rad[j];
				cpx z = V[i].z + cpx(l*cos(angle[e]),l*sin(angle[e]));
				if (std::isnan(real(V[j].z))) { flag = true; V[src[alpha[e]]].z = z; }
				else if (abs(V[j].z-z) > rad[0]/2) {
					bool fresh = true;
					for (cpx p : periods) if (abs(V[j].z-z-p) < rad[0]/2) fresh = false;
					if (fresh) periods.push_back(V[j].z-z);
				}
			}
		}

		cpx p1 = periods[0]; for (cpx p : periods) if (abs(p)<abs(p1)) { p1=p; }

		for (auto  	&v : V)      	v.z /= p1;
		for (double	&a : angle)  	a -= arg(p1);
		for (cpx   	&p : periods)	p /= p1;
		for (double	&r : rad)    	r /= abs(p1);

		vector<cpx> moduli; for (cpx p : periods) if (abs(imag(p)) > .1) moduli.push_back(p);
		double n2 = abs(moduli[0]); m = moduli[0];
		for (cpx p : moduli) if (abs(p)<n2) { n2=abs(p); m=p; }
		if (imag(m)<0) m = -m; while (real(m)<-.499) m+=1; while (real(m)>.501) m-=1;

		if (flip) { for (auto & a : angle) a += M_PI; for (auto & v : V) v.z = -v.z; }

		double slope = real(m) / imag(m);
		for (auto & v : V) {
			while (imag(v.z) < 0)                    	v.z += m;
			while (imag(v.z) > imag(m))              	v.z -= m;
			while (real(v.z) < slope * imag(v.z))    	v.z += 1;
			while (real(v.z) > slope * imag(v.z) + 1)	v.z -= 1;
		}
	}

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

	void output_pdf (int E=0, int mode=255) {
		for (int e=0; e<E; ++e) {
			int ee=e; do {
				bone_e[ee]=true; bone_e[alpha[e]]=true; bone_v[src[ee]]=true; ee = sigma[sigma[sigma[alpha[ee]]]];
			} while (!bone_e[ee]);
		}
		double slope = real(m) / imag(m);
		Figure F;
		F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,200,1)));
		vector<cpx> eee; unsigned ne=n_edges(); Cycles cc=sigma.cycles();
		for (int a=-2; a<=3; ++a)
			for (int b=-1; b<=1; ++b)
				for (unsigned e=0; e<ne; ++e) {
					int i=src[e]; cpx z = V[i].z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.6)) continue;
					if ((real(z)<-.6+0*slope*imag(z)) || (real(z)>2.6+0*slope*imag(z))) continue;
					if (e == cc[i][0])
						if ( ((mode&1)&&(bone_v[i])) || ((mode&2)&&(!bone_v[i])) )
							F.add (new Circle (z,rad[i],Pen(0,.15)));
					if ( ((mode&4)&&(bone_e[e])) || ((mode&8)&&(bone_v[i])) || ((mode&16)&&(!bone_v[i])) ) {
						eee.push_back(z);
						eee.push_back(z+cpx(rad[i]*cos(angle[e]),rad[i]*sin(angle[e])));
						eee.push_back(NAN);
					}
				}
		if (eee.size()) F.add (new Path (eee,Pen(0,.06)));
		F.output_pdf("period");
	}

	vector<int> src;
	vector<vector<int>> adj;
	vector<double> rad,angle;
	vector<bool> bone_v, bone_e;
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
