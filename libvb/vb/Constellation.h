#pragma once
#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/RationalFraction.h>
#include <vb/Spheroidal.h>
#include <vb/cpx.h>
#include <iomanip>
#include <sstream>

namespace vb {
	template <typename T> class Constellation;

	template <typename T> class CPixel { public:
		CPixel (Constellation<T> * C_, std::complex<T> z_ = 0) : C(C_), z(z_) {}
		operator Color() { if (prng.bernoulli(.01)) c = imag((*C)(z))>0 ? RED : BLUE; return c; }
		Constellation<T> * C;
		std::complex<T> z;
		Color c;
	};

	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		Constellation (Hypermap M, Hub H, int n=3);
		Constellation ();

		cplx	operator()	(cplx z)       	const { return l*P(z)/Q(z); }
		cplx	logder    	(cplx z, int k)	const;

		void	from_points	();
		void	normalize  	();
		void	linear     	(cplx u, cplx v = cplx(0));
		T   	belyi      	();

		T   	cost	()	const;
		T   	cost	(const std::vector<T> & xy);
		void	find	();

		void	show	();

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx             	l = T(1);
		Polynomial<cplx> 	P,Q;
		Bitmap<CPixel<T>>	*img = 0;
	};

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C);

	/***************************************************************************************************/

	template <typename T> Constellation<T>::Constellation (Hypermap M, Hub H, int n) {
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

	template <typename T> Constellation<T>::Constellation () {}

	template <typename T> void Constellation<T>::show() {
		img = new Bitmap<CPixel<T>> (600,600,"Constellation");
		for (int i=0; i<600; ++i) for (int j=0; j<600; ++j) img->put(coo(i,j), CPixel<T> (this, {(i-300)*T(2)/300,(j-300)*T(2)/300}));
		img->show();
	}

	template <typename T> void Constellation<T>::from_points () {
		P = Polynomial<cplx> (); Q = Polynomial<cplx> ();
		for (unsigned i=0; i<b.size(); ++i) for (unsigned j=0; j<bd[i]; ++j) P.add_root(b[i]);
		for (unsigned i=0; i<f.size(); ++i) for (unsigned j=0; j<fd[i]; ++j) Q.add_root(f[i]);
	}

	template <typename T> void Constellation<T>::normalize () {
		l = cplx(1); from_points();
		cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += (*this)(w[i])*cplx(wd[i]); }
		l = cplx(d)/avg;
	}

	template <typename T> void Constellation<T>::linear (cplx u, cplx v) {
		for (auto & z : b) z = u*z+v; for (auto & z : f) z = u*z+v; for (auto & z : w) z = u*z+v; normalize();
	}

	template <typename T> T Constellation<T>::belyi () {
		linear(1,-b[0]); linear(std::polar(T(1), -arg(f.size()>0 ? f[0] : w[0])));

		cplx lambda1 = pow(l,T(1)/(P.degree()-Q.degree()));                                          	linear(lambda1);
		cplx lambda2 = pow(l,T(1)/(P.degree()-Q.degree()));                                          	linear(lambda2);
		cplx sum (0); for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i])*b[i]; sum /= P.degree();	linear (1,-sum);

		unsigned i=0; T eps = sqrt(cost()); while (abs(P[i])<=eps) ++i;
		if (i<P.degree()) { cplx l = pow(P[i],T(1)/(P.degree()-i)); linear (cplx(1)/l); }

		i=0; eps = sqrt(cost()); while (abs(P[i])<=eps) ++i;
		if (i<P.degree()) { cplx l = pow(P[i],T(1)/(P.degree()-i)); linear (cplx(1)/l); }

		unsigned j=0,m=0,jm=0; while (j<2*P.degree()) {
			std::ostringstream os; os << *this; unsigned nm = os.str().size();
			if ((m==0)||(nm<m)) { m=nm; jm=j; }
			linear (std::polar (T(1), T(4)*T(atan(T(1)))/P.degree())); ++j;
		}
		linear (std::polar (T(1), jm*T(4)*T(atan(T(1)))/P.degree()));

		return abs(lambda1*lambda2);
	}

	template <typename T> auto Constellation<T>::logder (cplx z, int k) const -> cplx {
		cplx sum (0);
		for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i]) / pow (z-b[i], T(k));
		for (unsigned i=0; i<f.size(); ++i) sum -= cplx(fd[i]) / pow (z-f[i], T(k));
		return sum;
	}

	template <typename T> T Constellation<T>::cost() const {
		T out (0);
		for (unsigned i=0; i<w.size(); ++i) {
			out += norm ((*this)(w[i]) - T(1));
			for (unsigned j=1; j<wd[i]; ++j) out += norm(logder(w[i],j));
		}
		if (img) img->step();
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

		T c = cost(bw), eps = sqrt(c)/10, nc = c;
		while (eps>1e-100) {
			std::cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			for (auto & z : bw) {	z += eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { z += eps; } }
			if (!flag) eps /= 4; else eps *= 2;
		}
		std::cerr << std::endl;
	}

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C) {
		Constellation<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.l = C.l;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C) {
		double err = sqrt(C.cost()); os << std::setprecision (err<1e-6 ? log10(1/err)-3 : 3) << std::fixed;

		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
		os << std::endl;
		os << "lambda := " << C.l << std::endl;
		os << "P[z_]  := " << C.P << std::endl;
		os << "Q[z_]  := " << C.Q << std::endl;
		return os;
	}
}
