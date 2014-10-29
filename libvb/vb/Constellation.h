#pragma once
#include <vb/Bitmap.h>
#include <vb/Minimizer.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
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
		using Vector = typename Minimizer<T>::Vector;

		Constellation (Hypermap M, Hub H, int n=3);
		Constellation ();

		cplx	operator()	(cplx z)       	const { return l*P(z)/Q(z); }
		cplx	logder    	(cplx z, int k)	const;

		void	from_points	();
		void	normalize  	();
		void	linear     	(cplx u, cplx v = cplx(0));
		T   	belyi      	();

		T   	cost 	()	const;
		T   	cost 	(const Vector & xy);
		void	find 	();
		void	findg	();
		void	findm	();

		Vector	gradcost	()	const;
		Vector	gradcost	(const Vector & xy);

		void	show	();

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx             	l = T(1);
		Polynomial<cplx> 	P,Q;
		Bitmap<CPixel<T>>	*img = 0;
	};

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C);

	template <typename T> T Constellation_f (const typename Minimizer<T>::Vector & x, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		static T er (-1); T out = C->cost(x);
		if ((out<er)||(er<T(0))) { std::cerr << out << "          \r"; er = out; }
		return out;
	}

	template <typename T> typename Minimizer<T>::Vector Constellation_g (const typename Minimizer<T>::Vector & x, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		return C->gradcost(x);
	}

	template <typename T> void Constellation_cb (const typename Minimizer<T>::Vector &, T, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		if (C->img) C->img->step();
	}

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
		img->start = img->now(); img->show();
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
		linear(T(1),-b[0]); linear(std::polar(T(1), -arg(f.size()>0 ? f[0] : w[0])));

		cplx lambda1 = pow(l,T(1)/(P.degree()-Q.degree()));                                          	linear (lambda1);
		cplx lambda2 = pow(l,T(1)/(P.degree()-Q.degree()));                                          	linear (lambda2);
		cplx sum (0); for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i])*b[i]; sum /= P.degree();	linear (T(1),-sum);

		{ Polynomial<cplx> & PQ = ( ((abs(P[0])>abs(Q[0])) || (Q.degree()==0)) ? P : Q);
		unsigned i=0; T eps = std::min(sqrt(cost()),T(.01)); while ((i<PQ.degree())&&(abs(PQ[i])<=eps)) ++i;
		if (i<PQ.degree()) { cplx l = pow(PQ[i],T(1)/(PQ.degree()-i)); linear (cplx(1)/l); } }

		Polynomial<cplx> & PQ = ( ((abs(P[0])>abs(Q[0])) || (Q.degree()==0)) ? P : Q);
		unsigned i=0; T eps = std::min(sqrt(cost()),T(.01)); while ((i<PQ.degree())&&(abs(PQ[i])<=eps)) ++i;
		if (i<PQ.degree()) { cplx l = pow(PQ[i],T(1)/(PQ.degree()-i)); linear (cplx(1)/l); }

		unsigned j=0,m=0,jm=0; while (j<2*PQ.degree()) {
			std::ostringstream os; os << *this; unsigned nm = os.str().size();
			if ((m==0)||(nm<m)) { m=nm; jm=j; }
			linear (std::polar (T(1), T(4)*T(atan(T(1)))/PQ.degree())); ++j;
		}
		linear (std::polar (T(1), jm*T(4)*T(atan(T(1)))/PQ.degree()));

		return abs(lambda1*lambda2);
	}

	template <typename T> auto Constellation<T>::logder (cplx z, int k = 0) const -> cplx {
		if (k==0) return T(10)*log((*this)(z));
		cplx sum (0);
		for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i]) / pow (z-b[i], T(k));
		for (unsigned i=0; i<f.size(); ++i) sum -= cplx(fd[i]) / pow (z-f[i], T(k));
		return sum;
	}

	template <typename T> T Constellation<T>::cost() const {
		T out (0);
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out += norm(logder(w[i],j));
		// if (img) img->step();
		return out;
	}

	template <typename T> T Constellation<T>::cost (const Vector & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (unsigned i=0; i<n1; ++i) b[i] = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i] = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i] = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cplx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		from_points(); return cost();
	}

	template <typename T> auto Constellation<T>::gradcost () const -> Vector {
		static std::vector<cplx> gradb(b.size()), gradw(w.size()), gradf(f.size());
		for (auto & z : gradb) z = cplx(0); for (auto & z : gradw) z = cplx(0); for (auto & z : gradf) z = cplx(0);
		cplx gradl(0);

		for (unsigned k=0; k<w.size(); ++k) {
			cplx lwk0 ( T(20) * logder(w[k],0) );
			for (unsigned j=0; j<b.size(); ++j) { gradb[j] -= lwk0 * T(bd[j]) / conj(w[k]-b[j]); }
			for (unsigned j=0; j<f.size(); ++j) { gradf[j] += lwk0 * T(fd[j]) / conj(w[k]-f[j]); }

			gradw[k] += lwk0 * conj(logder(w[k],1)); gradl += lwk0 / conj(l);

			for (unsigned ll=1; ll<wd[k]; ++ll) {
				cplx lwkll ( T(2*ll) * logder(w[k],ll) );
				for (unsigned j=0; j<b.size(); ++j) { gradb[j] += lwkll * T(bd[j]) / pow(conj(w[k]-b[j]),T(ll+1)); }
				for (unsigned j=0; j<f.size(); ++j) { gradf[j] -= lwkll * T(fd[j]) / pow(conj(w[k]-f[j]),T(ll+1)); }
				gradw[k] -= lwkll * conj(logder(w[k],ll+1));
			}
		}

		std::vector<T> out;
		for (unsigned i=0; i<b.size(); ++i) { out.push_back(real(gradb[i])); out.push_back(imag(gradb[i])); }
		for (unsigned i=0; i<w.size(); ++i) { out.push_back(real(gradw[i])); out.push_back(imag(gradw[i])); }
		for (unsigned i=0; i<f.size(); ++i) { out.push_back(real(gradf[i])); out.push_back(imag(gradf[i])); }
		out.push_back(real(gradl)); out.push_back(imag(gradl));

		Vector vout (out.size()); for (unsigned i=0; i<out.size(); ++i) vout[i]=out[i];

		return vout;
	}

	template <typename T> auto Constellation<T>::gradcost (const Vector & xy) -> Vector {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (unsigned i=0; i<n1; ++i) b[i] = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i] = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i] = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cplx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		from_points(); return gradcost();
	}

	template <typename T> void Constellation<T>::findg () {
		std::vector<T> vbw;
		for (auto z : b) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : w) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : f) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		vbw.push_back(real(l)); vbw.push_back(imag(l));

		Vector bw (vbw.size()); for (unsigned i=0; i<vbw.size(); ++i) bw[i]=vbw[i];

		T c = cost(bw), eps = sqrt(c), nc = c;
		while (eps>1e-100) {
			std::cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			c = cost(bw);
			Vector grad = gradcost();
			for (unsigned i=0; i<bw.size(); ++i) bw[i] -= eps * grad[i];
			nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { for (unsigned i=0; i<bw.size(); ++i) bw[i] += eps * grad[i]; }
			if (!flag) eps /= 2; else eps *= 1.01;
		}
		std::cerr << std::endl;
	}

	template <typename T> void Constellation<T>::find () {
		std::vector<T> vbw;
		for (auto z : b) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : w) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : f) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		vbw.push_back(real(l)); vbw.push_back(imag(l));

		Vector bw (vbw.size()); for (unsigned i=0; i<vbw.size(); ++i) bw[i]=vbw[i];

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

	template <typename T> void Constellation<T>::findm () {
		std::vector<T> vbw;
		for (auto z : b) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : w) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		for (auto z : f) { vbw.push_back(real(z)); vbw.push_back(imag(z)); }
		vbw.push_back(real(l)); vbw.push_back(imag(l));

		Constellation<T>::Vector x (vbw.size()); for (unsigned i=0; i<vbw.size(); ++i) { x[i] = vbw[i]; }
		Minimizer<T> M (vbw.size(),Constellation_f<T>,Constellation_g<T>,this); M.cb = Constellation_cb;
		M.minimize_qn (x);
		cost(M.x);
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
		// double err = C.cost(); os << std::setprecision (err<1e-9 ? -log10(err)/3 : 3) << std::fixed; if (err==T(0)) os << std::setprecision(10);

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
