#include <vb/Figure.h>
#include <vb/Spheroidal.h>
#include <cassert>
#include <cmath>

namespace vb {
	Spheroidal::Spheroidal (Hypermap M, Hub H) : Hypermap(M) {
		assert (genus()==0);
		from_hypermap();
		mode = H['m']; title = H.title;
	}

	void Spheroidal::pack () {
		for (auto & v : V) v.fixed = false;
		for (auto f : phi.cycles()) if (f.size()==3) {
			V[E[f[0]].src].fixed = true;	E[f[0]].a=M_PI/3; 	E[sigma[f[0]]].a = 0;
			V[E[f[1]].src].fixed = true;	E[f[1]].a=-M_PI/3;	E[sigma[f[1]]].a = -2*M_PI/3;
			V[E[f[2]].src].fixed = true;	E[f[2]].a=M_PI;   	E[sigma[f[2]]].a = 2*M_PI/3;
			V[E[f[0]].src].z=0; break;
		}

		acpa();

		int ne=sigma.size();
		bool flag=true; while (flag) { flag = false;
			for (int i=0; i<ne; ++i) {
				if (std::isnan(E[i].a))         	continue;
				if (std::isnan(E[alpha[i]].a)) {	E[alpha[i]].a = E[i].a+M_PI; flag = true; }
				if (std::isnan(E[sigma[i]].a)) {	double x = V[E[i].src].r, y = V[E[alpha[i]].src].r, z = V[E[alpha[sigma[i]]].src].r;
				                                	E[sigma[i]].a = E[i].a + alpha_xyz(x,y,z); flag = true; }
			}
		}

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

	void Spheroidal::mobiusto0 (cpx a) {
		if (a==0.0) return;
		cpx abar = conj(a);
		for (auto & v : V) {
			const cpx z1 = 1.0 - abar * v.z; const double r1 = norm(z1) - v.r*v.r*norm(a);
			v.z = ((1-norm(a)) * z1/r1 - 1.0) / abar;
			v.r *= (1-norm(a))/r1;
		}
		for (unsigned i=0; i<sigma.size(); ++i) {
			E[i].a = arg(V[E[alpha[i]].src].z - V[E[i].src].z);
			if (V[E[alpha[i]].src].r<0) E[i].a += M_PI;
		}
	}

	void Spheroidal::output_pdf () {
		for (unsigned e=0; e<sigma.size(); ++e) {
			if (!initial[e]) continue;
			V[E[e].src].bone = std::max (V[E[e].src].bone,initial[e]);
		}

		Figure F; std::vector<cpx> eee; Cycles sc = sigma.cycles();

		for (auto & v : V) {
			cpx z = v.z;
			if ( ((mode&1)&&(v.bone)) || ((mode&2)&&(!v.bone)) ) F.add (new Circle (z,fabs(v.r),Pen(0,.3)));
			for (int e : sc[v.i]) {
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

		F.title = title; F.output_pdf();
	}
}
