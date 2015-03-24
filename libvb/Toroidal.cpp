#include <vb/Figure.h>
#include <vb/Toroidal.h>
#include <cassert>
#include <cmath>

namespace vb {
	Toroidal::Toroidal (Hypermap M, Hub H) : Hypermap(M), m(I) {
		assert (genus()==1);
		from_hypermap();
		mode = H['m']; title = H.title;
	}

	void Toroidal::pack () {
		acpa();

		E[0].a=0; int ne=sigma.size();
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

		unsigned mdeg=0; cpx mpos=0;
		for (auto v : V) if (v.adj.size()>mdeg) { mdeg = v.adj.size(); mpos = v.z; }

		double slope = real(m) / imag(m);
		for (auto & v : V) {
			v.z -= mpos;
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
		for (unsigned e=0; e<sigma.size(); ++e) {
			if (!initial[e]) continue;
			V[E[e].src].bone = std::max (V[E[e].src].bone,initial[e]);
		}

		Figure F; std::vector<cpx> eee; Cycles sc = sigma.cycles();

		for (int a=-2; a<3; ++a) {
			for (int b=-1; b<2; ++b) {
				for (auto & v : V) {
					cpx z = v.z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.7*std::max(1.0,imag(m)))||(real(z)<-.8)||(real(z)>2.6)) continue;
					if ( ((mode&1)&&(v.bone)) || ((mode&2)&&(!v.bone)) ) F.add (new Circle (z,v.r,Pen(0,.3)));
					for (int e : sc[v.i]) {
						if ( ((mode&4)&&(initial[e]&1)) || ((mode&8)&&(v.bone&1)) || ((mode&16)&&(!(v.bone&1))) ) {
							eee.push_back(z);
							eee.push_back(z+std::polar(v.r,E[e].a));
							eee.push_back(NAN);
						}
					}
				}
			}
		}

		if (eee.size()) F.add (new Path (eee,Pen(0,.5)));

		for (int a=-2; a<3; ++a) {
			for (int b=-1; b<2; ++b) {
				for (auto v : V) {
					cpx z = v.z + cpx(a) + cpx(b)*m;
					if ((imag(z)<-.6)||(imag(z)>1.7*std::max(1.0,imag(m)))||(real(z)<-.8)||(real(z)>2.6)) continue;
					if ((mode&32)&&(v.bone&2)) F.add (new Circle (z,.015,Pen(0,.5,0,1)));
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
			}
		}

		F.add (new Polygon ({0,1,cpx(1)+m,m}, Pen(0,0,Color(0,0,0,50),1)));
		F.title = title; F.output_pdf();
	}
}
