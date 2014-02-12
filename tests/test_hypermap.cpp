#include <vb/CL_Parser.h>
#include <vb/Figure.h>
#include <vb/Hypermap.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace vb;
using namespace std;

double alpha_xyz (double x, double y, double z) { return acos (((x+y)*(x+y)+(x+z)*(x+z)-(y+z)*(y+z))/(2*(x+y)*(x+z))); }

class Toroidal : public Hypermap { // Triangulation of torus
	public:
		Toroidal (const Hypermap &H) : Hypermap(H),
				src(n_edges()), adj(n_black()), rad(n_black(),1), angle(n_edges(),NAN), place(n_black(),NAN),
				bone_v(n_black(),false), bone_e(n_edges(),false) {
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
			vector<cpx> eee;
			for (int a=-1; a<=2; ++a)
				for (int b=-1; b<=1; ++b)
					for (int e=0; e<n_edges(); ++e) {
						int i=src[e]; cpx z = place[i] + cpx(a) + cpx(b)*m;
						if ((imag(z)<-.6)||(imag(z)>1.6)) continue;
						if ((real(z)<-.6+slope*imag(z)) || (real(z)>2.6+slope*imag(z))) continue;
						if (e == sigma.c[i][0])
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
		vector<cpx> place;
		vector<bool> bone_v, bone_e;
		cpx m;
};

Hypermap artem (int n) {
	vector<int> sigma(6*n), alpha(6*n), phi(6*n);
	for (int i=0; i<n; ++i) {
		sigma[i]=2*n+i; sigma[n+i]=5*n+i+1; sigma[2*n+i]=n+i-1; sigma[3*n+i]=3*n+i+1; sigma[4*n+i]=4*n+i-1; sigma[5*n+i]=i;
		alpha[i]=n+i; alpha[n+i]=i; alpha[2*n+i]=3*n+i; alpha[3*n+i]=2*n+i; alpha[4*n+i]=5*n+i; alpha[5*n+i]=4*n+i;
		phi[i]=2*n+i+1; phi[n+i]=5*n+i; phi[2*n+i]=3*n+i-1; phi[3*n+i]=i; phi[4*n+i]=n+i-1; phi[5*n+i]=4*n+i+1;
	}
	sigma[2*n-1]=3*n; sigma[2*n]=5*n-1; sigma[4*n-1]=5*n; sigma[4*n]=2*n-1;
	phi[n-1]=4*n; phi[2*n]=2*n-1; phi[3*n]=0; phi[4*n]=4*n-1; phi[6*n-1]=2*n;
	Hypermap H; H.sigma=sigma; H.alpha=alpha; H.phi=phi;
	assert(H.validate());
	return H;
}

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

	// This is H2 with one additional site.
	Hypermap H67;
	H67.sigma	= { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, { 10, 11, 12, 13, 14 }, { 15, 16, 17 } };
	H67.alpha	= { { 0, 5 }, { 1, 14 }, { 2, 17 }, { 3, 7 }, { 4, 10 }, { 6, 11 }, { 8, 16 }, { 9, 12 }, { 13, 15 } };
	H67.phi  	= { { 0, 4, 14 }, { 1, 13, 17 }, { 2, 16, 7 }, { 3, 6, 10 }, { 5, 9, 11 }, { 8, 15, 12 } };

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

	// SV graph
	Hypermap SV;
	SV.sigma = { {1,2,3,4,5,6,7}, {8,9,10,11,12}, {13,14,15,16,17}, {18,19,20,21,22,23,0} };
	SV.alpha = { {0,4}, {1,10}, {2,22}, {3,15}, {5,8}, {6,19}, {7,13}, {9,23}, {11,17}, {12,20}, {14,18}, {16,21} };
	SV.phi   = { {0,3,14}, {1,9,22}, {2,21,15}, {4,23,8}, {5,12,19}, {6,18,13}, {7,17,10}, {11,16,20} };

	// Artem's graph with 4 intervals
	Hypermap A4;
	A4.sigma = { {0,12,11,10,9,8,7,20,19,18,17,16}, {1,13,4,23}, {2,14,5,22}, {3,15,6,21} };
	A4.alpha = { {0,4}, {1,5}, {2,6}, {3,7}, {8,16}, {9,23}, {10,22}, {11,21}, {12,20}, {13,19}, {14,18}, {15,17} };
	A4.phi   = { {0,13,20}, {1,14,19}, {2,15,18}, {3,8,17}, {4,16,9}, {5,23,10}, {6,22,11}, {7,21,12} };

	CL_Parser CLP (argc,argv,"n=4,r=2.6,m=17,a=4");
	Hypermap G=artem(CLP('a'));

	int n_skel = G.n_edges();
	for (int i=0; i<int(CLP('n')); ++i) G = G.split_edges(); cerr << G;

	Toroidal H(G);
	H.pack(CLP('r'));
	H.output_pdf(n_skel,CLP('m'));

	cpx q = exp(I * M_PI * H.m);
	cpx theta2(0), old_theta2(-1); for (int n=0; theta2 != old_theta2; ++n) { old_theta2=theta2; theta2 += 2.0 * pow (q, (n+.5)*(n+.5)); }
	cpx theta3(1), old_theta3(-1); for (int n=1; theta3 != old_theta3; ++n) { old_theta3=theta3; theta3 += 2.0 * pow (q, n*n); }
	cpx lambda = pow(theta2,4) / pow(theta3,4);
	cpx J = (4.0/27.0) * pow(1.0-lambda+pow(lambda,2),3) / pow(lambda,2) / pow(1.0-lambda,2);
	cpx j = 1728.0 * J;

	cerr << setprecision(15);
	cerr << "    tau = " << H.m << endl;
	cerr << "j (tau) = " << j << endl;
}
