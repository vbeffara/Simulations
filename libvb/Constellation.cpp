#include <vb/Constellation.h>

namespace vb {
	Constellation::Constellation (Hypermap M, Hub H, int n) : Image (600,600,"Constellation") {
		Hypermap M2=M; M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Spheroidal T (M2,H); T.pack(); std::cerr << std::endl;

		int N = M.sigma.size();

		unsigned inf=0, dinf=0; for (auto c : M.phi.cycles()) { unsigned i = T.E[c[0]+3*N].src, d = T.V[i].adj.size(); if (d>dinf) { inf=i; dinf=d; } }
		T.linear (1,-T.V[inf].z); T.inversion(); T.linear (-1/T.V[inf].r,0); T.output_pdf();
		{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); } T.linear (std::polar(1.0,-T.E[0].a)); T.output_pdf();

		for (auto c : M.sigma.cycles())	{                                      	b.push_back(T.V[T.E[c[0]].src].z);    	bd.push_back(c.size()); }
		for (auto c : M.alpha.cycles())	{                                      	w.push_back(T.V[T.E[c[0]+N].src].z);  	wd.push_back(c.size()); }
		for (auto c : M.phi.cycles())  	{ if (T.E[c[0]+3*N].src==inf) continue;	f.push_back(T.V[T.E[c[0]+3*N].src].z);	fd.push_back(c.size()); }

		from_points(); long double l = belyi(); T.linear(l); T.output_pdf();
	}

	void Constellation::from_points () {
		RationalFraction<lcpx> R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		Rs = {R};
		for (auto & c : Rs[0].P) c *= l;
		for (int i=0; i<b.size(); ++i) for (int j=Rs.size(); j<bd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<w.size(); ++i) for (int j=Rs.size(); j<wd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<f.size(); ++i) for (int j=Rs.size(); j<fd[i]; ++j) Rs.push_back(Rs.back().derivative());
	}

	Color Constellation::compute (coo c) {
		lcpx z { (c.x-300)*2.0/300, (c.y-300)*2.0/300 };
		return imag(Rs[0](z))>0 ? RED : BLUE;
	}

	void Constellation::normalize () {
		RationalFraction<lcpx> R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		lcpx avg = 0; int d=0;
		for (int i=0; i<w.size(); ++i) { d += wd[i]; avg += R(w[i])*lcpx(wd[i]); }
		l = lcpx(d)/avg;
		from_points();
	}

	long double Constellation::belyi () {
		lcpx z0 = b[0];                                           	for (auto & z : b) z -= z0; for (auto & z : w) z -= z0; for (auto & z : f) z -= z0;
		lcpx r0 = std::polar(1.0L,-arg(f.size()>0 ? f[0] : w[0]));	for (auto & z : b) z *= r0; for (auto & z : w) z *= r0; for (auto & z : f) z *= r0;
		normalize();

		long double lambda = pow(abs(Rs[0].P.back()),1.0/Rs[0].degree());
		for (auto & z : b) z *= lambda; for (auto & z : w) z *= lambda; for (auto & z : f) z *= lambda; normalize();

		return lambda;
	}

	long double Constellation::cost() const {
		long double out=0;
		for (int i=0; i<w.size(); ++i) for (int j=0; j<wd[i]; ++j) out += norm (Rs[j](w[i]) - (j==0?1.0L:0.0L));
		return out;
	}

	long double Constellation::cost (const std::vector<long double> & xy) {
		int n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (int i=0; i<n1; ++i) b[i] = lcpx (xy[2*i],xy[2*i+1]);
		for (int i=0; i<n2; ++i) w[i] = lcpx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (int i=0; i<n3; ++i) f[i] = lcpx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = lcpx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		from_points(); return cost();
	}

	void Constellation::find () {
		std::vector<long double> bw;
		for (auto z : b) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : w) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : f) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		bw.push_back(real(l)); bw.push_back(imag(l));

		long double c = cost(bw), eps = sqrt(c), nc = c;
		while (eps>1e-100) {
			std::cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			for (auto & z : bw) {	z += eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { z += eps; } }
			if (!flag) eps /= 4;

			if (visible()) {
				for (int i=0; i<600; ++i) for (int j=0; j<600; ++j) put (coo(i,j),0);
				tessel (0,0,599,599); update();
			}
		}
		std::cerr << std::endl;
	}

	std::ostream & operator<< (std::ostream & os, const Constellation & C) {
		long double err = sqrt(C.cost()); os << std::setprecision (err<1e-3 ? log10(1/err) : 3) << std::fixed;

		os << "Black vertices / zeros: " << std::endl;
		for (int i=0; i<C.b.size(); ++i) { os << "| " << C.bd[i] << "\t" << C.b[i]; for (int j=0; j<C.bd[i]; ++j) os << "\t" << C.Rs[j](C.b[i]); os << std::endl; }
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (int i=0; i<C.w.size(); ++i) { os << "| " << C.wd[i] << "\t" << C.w[i]; for (int j=0; j<C.wd[i]; ++j) os << "\t" << C.Rs[j](C.w[i]); os << std::endl; }
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (int i=0; i<C.f.size(); ++i) { os << "| " << C.fd[i] << "\t" << C.f[i]; for (int j=0; j<C.fd[i]; ++j) os << "\t" << C.Rs[j](C.f[i]); os << std::endl; }
		os << std::endl;
		os << C.Rs[0] << std::endl;
		return os;
	}
}
