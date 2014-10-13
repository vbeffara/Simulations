#include <vb/Figure.h>
#include <vb/Spheroidal.h>
#include <cmath>

namespace vb {
	Spheroidal::Spheroidal (Hypermap M, Hub H_) : Hypermap(M), H(H_) {
		assert (genus()==0);
		from_hypermap();
		prog = H_.prog;
	}

	void Spheroidal::from_hypermap () {
		assert(is_triangulation());
		Cycles sc = sigma.cycles(); int nb = sc.size();
		V.resize(nb); E.resize(6*nb);
	    int i=0;
		for (auto & v : V) {
			v.z = NAN; v.bone=false; v.adj.clear(); v.edges = sc[i];
			for (int e : v.edges) { E[e].a = NAN; E[e].src = i; }
			++i;
		}
		for (auto & v : V) for (int e : v.edges) { v.adj.push_back(E[alpha[e]].src); }
	}

	static double alpha_xyz (double x, double y, double z) { return acos ( (x*(x+y+z) - y*z) / ((x+y)*(x+z)) ); }

	static double ccn (int n) {
		static std::vector<double> p;
		for (int i=p.size(); i<=n; ++i) p.push_back (sqrt(2/(1-cos(2*M_PI/i)))-1);
		return p[n];
	}

	void Spheroidal::acpa () {
		double e = 1, old_e = 2; unsigned i0,i1,i2,e0;
		for (auto f : phi.cycles()) if (f.size()==3) { e0=f[0]; i0 = E[f[0]].src; i1 = E[f[1]].src; i2 = E[f[2]].src; break; }
		while ((e > 1e-3) || (e < old_e)) {
			std::cerr << e << "      \r";
			old_e = e; e = 0;
			for (unsigned i=0; i<V.size(); ++i) {
				if ((i==i0)||(i==i1)||(i==i2)) continue;
				Vertex &v = V[i];
				int n = v.adj.size();
				double s=0, r0=v.r, r1, r2=V[v.adj.back()].r;
				for (int ll : v.adj) { r1=r2; r2=V[ll].r; s += alpha_xyz (r0,r1,r2); }
				double c=cos(s/n);
				double nr=ccn(n) * (1-c + sqrt(2*(1-c))) / (1+c);
				e += fabs(1-nr);
				v.r *= 1.1 * nr - .1;
			}
		}
		std::cerr << std::endl;
	}

	void Spheroidal::pack () {
		acpa();

		unsigned i0,i1,i2,e0,e1,e2;
		for (auto &f : phi.cycles()) if (f.size()==3) { e0=f[0]; i0 = E[e0].src; e1=f[1]; i1 = E[e1].src; e2=f[2]; i2 = E[e2].src; break; }

		E[e0].a=M_PI/3; E[sigma[e0]].a = 0; E[e2].a=M_PI; E[sigma[e2]].a = 2*M_PI/3; E[e1].a=-M_PI/3; E[sigma[e1]].a = -2*M_PI/3;

		int ne=n_edges();
		bool flag=true; while (flag) { flag = false;
			for (int i=0; i<ne; ++i) {
				if (std::isnan(E[i].a))         	continue;
				if (std::isnan(E[alpha[i]].a)) {	E[alpha[i]].a = E[i].a+M_PI; flag = true; }
				if (std::isnan(E[sigma[i]].a)) {	double x = V[E[i].src].r, y = V[E[alpha[i]].src].r, z = V[E[alpha[sigma[i]]].src].r;
				                                	E[sigma[i]].a = E[i].a + alpha_xyz(x,y,z); flag = true; }
			}
		}

		V[i0].z=0;

		flag=true; while (flag) { flag=false;
			for (int e=0; e<ne; ++e) {
				int i=E[e].src; if (std::isnan(real(V[i].z))) continue;
				int j=E[alpha[e]].src; double l = V[i].r + V[j].r;
				cpx z = V[i].z + std::polar(l,E[e].a);
				if (std::isnan(real(V[j].z))) { flag = true; V[E[alpha[e]].src].z = z; }
			}
		}
	}

	void Spheroidal::linear (cpx a, cpx b) {
		for (auto & v : V) { v.z = a*v.z+b; v.r *= abs(a); }
		for (auto & e : E) { e.a += arg(a); }
	}

	void Spheroidal::inversion () {
		for (auto & v : V) {
			if (v.z==0.0) { v.r = -1/v.r; continue; }
			double r1 = 1/(abs(v.z)-v.r), r2 = 1/(abs(v.z)+v.r);
			double nr = (r1-r2)/2, nz = (r1+r2)/2;
			v.z *= nz/abs(v.z); v.r = nr;
		}
		for (unsigned i=0; i<sigma.size(); ++i) {
			E[i].a = arg(V[E[alpha[i]].src].z - V[E[i].src].z);
			if (V[E[alpha[i]].src].r<0) E[i].a += M_PI;
		}
	}

	void Spheroidal::output_pdf () {
		for (int e=0; e<sigma.size(); ++e) {
			if (!initial[e]) continue;
			V[E[e].src].bone = std::max (V[E[e].src].bone,initial[e]);
		}

		Figure F; std::vector<cpx> eee; int mode = H['m'];

		for (auto v : V) {
			cpx z = v.z;
			if ( ((mode&1)&&(v.bone)) || ((mode&2)&&(!v.bone)) ) F.add (new Circle (z,fabs(v.r),Pen(0,.3)));
			for (int e : v.edges) {
				if ( ((mode&4)&&(initial[e]&1)) || ((mode&8)&&(v.bone&1)) || ((mode&16)&&(!(v.bone&1))) ) {
					eee.push_back(z);
					eee.push_back(z+std::polar(v.r,E[e].a));
					eee.push_back(NAN);
				}
			}
		}

		if (eee.size()) F.add (new Path (eee,Pen(0,.5)));

		for (auto v : V) {
			if (v.r<0) continue;
			cpx z = v.z;
			if ((mode&32)&&(v.bone&2)) F.add (new Circle (z,.01,Pen(0,2,BLACK,1)));
			if ((mode&64)&&(v.bone&4)) F.add (new Circle (z,.01,Pen(0,2,WHITE,1)));
			if ((mode&128)&&(v.bone&8)) F.add (new Circle (z,.01,Pen(0,2,RED,1)));
			if ((mode&256)&&(v.bone&8)) {
				std::vector<cpx> ast; for (int i=0; i<3; ++i) {
					ast.push_back (z+std::polar(.013,i*M_PI/3));
					ast.push_back (z+std::polar(.013,(i+3)*M_PI/3));
					ast.push_back (NAN);
				}
				F.add (new Path (ast,Pen(0,1)));
			}
		}

		F.title = H.title; F.prog = prog; F.output_pdf();
	}
}