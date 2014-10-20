#pragma once
#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/RationalFraction.h>
#include <vb/Spheroidal.h>
#include <vb/cpx.h>
#include <iomanip>

namespace vb {
	template <typename T> class Constellation : public Image { public:
		using cplx = std::complex<T>;

		Constellation (Hypermap M, Hub H, int n=3);

		void	from_points	();
		void	normalize  	();
		T   	belyi      	();

		cplx logder (cplx z, int k) const;

		T   	cost	()	const;
		T   	cost	(const std::vector<T> & xy);
		void	find	();

		virtual Color compute (coo c);

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;
		cplx                 	l = T(1);

		RationalFraction<cplx> R;
	};

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C);

	/***************************************************************************************************/

	template <typename T> Constellation<T>::Constellation (Hypermap M, Hub H, int n) : Image (600,600,"Constellation") {
		Hypermap M2=M; M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Spheroidal S (M2,H); S.pack(); std::cerr << std::endl;

		int N = M.sigma.size();

		unsigned inf=0, dinf=0; for (auto c : M.phi.cycles()) { unsigned i = S.E[c[0]+3*N].src, d = S.V[i].adj.size(); if (d>dinf) { inf=i; dinf=d; } }
		S.linear (1,-S.V[inf].z); S.inversion(); S.linear (-1/S.V[inf].r,0); S.output_pdf();
		{ cpx z; while ((z = S.V[S.E[0].src].z) != 0.0) S.mobiusto0 (z); } S.linear (std::polar(1.0,-S.E[0].a)); S.output_pdf();

		for (auto c : M.sigma.cycles())	{                                      	b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); }
		for (auto c : M.alpha.cycles())	{                                      	w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
		for (auto c : M.phi.cycles())  	{ if (S.E[c[0]+3*N].src==inf) continue;	f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }

		from_points(); T l = belyi(); S.linear(double(l)); S.output_pdf();
	}

	template <typename T> void Constellation<T>::from_points () {
		R = RationalFraction<cplx> ();
		for (unsigned i=0; i<b.size(); ++i) for (unsigned j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (unsigned i=0; i<f.size(); ++i) for (unsigned j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		for (auto & c : R.P) c *= l;
	}

	template <typename T> Color Constellation<T>::compute (coo c) {
		cplx z { (c.x-300)*2.0/300, (c.y-300)*2.0/300 };
		return imag(R(z))>0 ? RED : BLUE;
	}

	template <typename T> void Constellation<T>::normalize () {
		l = cplx(1); from_points();
		cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += R(w[i])*cplx(wd[i]); }
		l = cplx(d)/avg;
		from_points();
	}

	template <typename T> T Constellation<T>::belyi () {
		cplx z0 = b[0];                                            	for (auto & z : b) z -= z0; for (auto & z : w) z -= z0; for (auto & z : f) z -= z0;
		cplx r0 = std::polar(T(1), -arg(f.size()>0 ? f[0] : w[0]));	for (auto & z : b) z *= r0; for (auto & z : w) z *= r0; for (auto & z : f) z *= r0;
		normalize();

		T lambda1 = pow(abs(R.P.back()),1.0/R.degree());
		for (auto & z : b) z *= lambda1; for (auto & z : w) z *= lambda1; for (auto & z : f) z *= lambda1; normalize();
		T lambda2 = pow(abs(R.P.back()),1.0/R.degree());
		for (auto & z : b) z *= lambda2; for (auto & z : w) z *= lambda2; for (auto & z : f) z *= lambda2; normalize();

		return lambda1*lambda2;
	}

	template <typename T> auto Constellation<T>::logder (cplx z, int k) const -> cplx {
		cplx sum (0);
		for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i]) / pow (z-b[i], k);
		for (unsigned i=0; i<f.size(); ++i) sum -= cplx(fd[i]) / pow (z-f[i], k);
		return sum;
	}

	template <typename T> T Constellation<T>::cost() const {
		T out (0);
		for (unsigned i=0; i<w.size(); ++i) { out += norm (R(w[i]) - T(1)); for (unsigned j=1; j<wd[i]; ++j) out += norm(logder(w[i],j)); };
		return out;
	}

	template <typename T> T Constellation<T>::cost (const std::vector<T> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (unsigned i=0; i<n1; ++i) b[i] = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i] = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i] = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cplx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		from_points(); return cost();
	}

	template <typename T> void Constellation<T>::find () {
		std::vector<T> bw;
		for (auto z : b) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : w) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : f) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		bw.push_back(real(l)); bw.push_back(imag(l));

		T c = cost(bw), eps = sqrt(c), nc = c;
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

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C) {
		double err = sqrt(C.cost()); os << std::setprecision (err<1e-3 ? log10(1/err) : 3) << std::fixed;

		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
		os << std::endl;
		os << C.R << std::endl;
		return os;
	}
}
