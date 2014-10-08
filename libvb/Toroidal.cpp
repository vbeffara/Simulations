#include <vb/Figure.h>
#include <vb/Toroidal.h>
#include <cmath>

namespace vb {
	Toroidal::Toroidal (Hypermap M, Hub H_) : Hypermap(M), m(I), n_skel(n_edges()), H(H_) {
		for (int i=0; i<int(H['o']); ++i) Hypermap::operator= (Hypermap::split_edges());
		from_hypermap();
		prog = H_.prog;
	}

	void Toroidal::split_edges () {
		std::string p = prog;
		Hypermap::operator= (Hypermap::split_edges());
		prog = p;
		from_hypermap();
		for (int i=V.size()/4; i<V.size(); ++i) {
			V[i].r = 0; int k=0;
			for (int j : V[i].adj) if (j < V.size()/4) { V[i].r += V[j].r; ++k; }
			V[i].r /= k;
		}
	}

	void Toroidal::from_hypermap () {
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

	void Toroidal::acpa (double r) {
		double e = 1, old_e = 2;
		while ((e > 1e-3) || (e < old_e)) {
			std::cerr << e << "     \r";
			old_e = e; e = 0;
			for (auto & v : V) {
				double s=-2*M_PI, r0=v.r, r1, r2=V[v.adj.back()].r;
				for (int ll : v.adj) { r1=r2; r2=V[ll].r; s += alpha_xyz (r0,r1,r2); }
				v.r *= 1 + r*s/(2*M_PI); if (v.r<0) v.r /= 1 + r*s/(2*M_PI); ;
				e += fabs(s);
			}
		}
	}

	void Toroidal::pack (double r) {
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

		V[0].z=0; std::vector<cpx> periods;
		flag=true; while (flag) { flag=false; periods.clear();
			for (int e=0; e<ne; ++e) {
				int i=E[e].src; if (std::isnan(real(V[i].z))) continue;
				int j=E[alpha[e]].src; double l = V[i].r + V[j].r;
				cpx z = V[i].z + std::polar(l,E[e].a);
				if (std::isnan(real(V[j].z))) { flag = true; V[E[alpha[e]].src].z = z; }
				else if (abs(V[j].z-z) > V[0].r/2) { periods.push_back(V[j].z-z); }
			}
		}

		cpx p1 = periods[0]; for (cpx p : periods) if (abs(p)<abs(p1)) { p1=p; }

		for (auto	& v : V)     	{ v.z /= p1; v.r /= abs(p1); }
		for (auto	& e : E)     	{ e.a -= arg(p1); }
		for (cpx 	&p : periods)	p /= p1;

		std::vector<cpx> moduli; for (cpx p : periods) if (fabs(imag(p)) > .1) moduli.push_back(p);
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

	void Toroidal::flip() {
		for (auto & e : E) e.a += M_PI;
		for (auto & v : V) v.z = 1.0+m-v.z;
	}

	void Toroidal::output_pdf () {
		for (int e=0; e<n_skel; ++e) {
			int ee=e; do {
				E[ee].bone=true; E[alpha[e]].bone=true; V[E[ee].src].bone=true; ee = sigma[sigma[sigma[alpha[ee]]]];
			} while (!E[ee].bone);
		}

		Figure F; std::vector<cpx> eee; int mode = H['m'];
		F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,200,1)));

		for (int a=-2; a<3; ++a) {
			for (int b=-1; b<2; ++b) {
				for (auto v : V) {
					cpx z = v.z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.7*std::max(1.0,imag(m)))||(real(z)<-.8)||(real(z)>2.6)) continue;
					if ( ((mode&1)&&(v.bone)) || ((mode&2)&&(!v.bone)) ) F.add (new Circle (z,v.r,Pen(0,.15)));
					for (int e : v.edges) if ( ((mode&4)&&(E[e].bone)) || ((mode&8)&&(v.bone)) || ((mode&16)&&(!v.bone)) ) {
						eee.push_back(z);
						eee.push_back(z+std::polar(v.r,E[e].a));
						eee.push_back(NAN);
					}
				}
			}
		}

		if (eee.size()) F.add (new Path (eee,Pen(0,.06)));
		F.title = H.title; F.prog = prog; F.output_pdf();
	}
}