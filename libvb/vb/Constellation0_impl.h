#pragma once
#include <vb/Constellation0.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Spheroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation0<T>::Constellation0 (Hypermap M, Hub H, int n) {
		Hypermap M2 (M); M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Spheroidal S (M2,H); S.pack(); std::cerr << std::endl;

		unsigned N = M.sigma.size(), inf = 0, dinf = 0;
		for (auto c : M.phi.cycles()) { unsigned i = S.E[c[0]+3*N].src, d = S.V[i].adj.size(); if (d>dinf) { inf=i; dinf=d; } }

		S.linear (1,-S.V[inf].z); S.inversion(); S.linear (-1/S.V[inf].r,0); S.output_pdf();
		{ cpx z; while ((z = S.V[S.E[0].src].z) != 0.0) S.mobiusto0 (z); } S.linear (std::polar(1.0,-S.E[0].a));
		S.output_pdf();

		for (auto c : M.sigma.cycles())	{                                      	b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); }
		for (auto c : M.alpha.cycles())	{                                      	w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
		for (auto c : M.phi.cycles())  	{ if (S.E[c[0]+3*N].src==inf) continue;	f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }

		from_points(); make_c_0(); make_l_1();
	}

	template <typename T> Constellation0<T>::Constellation0 () {}

	template <typename T> void Constellation0<T>::from_points () {
		P = Polynomial<cplx> (); Q = Polynomial<cplx> ();
		for (unsigned i=0; i<b.size(); ++i) for (unsigned j=0; j<bd[i]; ++j) P.add_root(b[i]);
		for (unsigned i=0; i<f.size(); ++i) for (unsigned j=0; j<fd[i]; ++j) Q.add_root(f[i]);
	}

	template <typename T> void Constellation0<T>::normalize () {
		l = cplx(1);
		cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += (*this)(w[i])*cplx(wd[i]); }
		l = cplx(d)/avg;
	}

	template <typename T> void Constellation0<T>::linear (cplx u, cplx v) {
		for (auto & z : b) z = u*z+v; for (auto & z : f) z = u*z+v; for (auto & z : w) z = u*z+v; from_points();
	}

	template <typename T> void Constellation0<T>::make_l_1 () {
		normalize(); linear (pow(l,cplx(T(1)/T(P.degree()-Q.degree())))); l=T(1);
	}

	template <typename T> void Constellation0<T>::make_c_0 () {
		cplx sum(0); for (unsigned i=0; i<b.size(); ++i) sum += T(bd[i])*b[i]; sum /= P.degree(); linear (T(1),-sum); normalize();
	}

	template <typename T> void Constellation0<T>::make_p_1 () {
		T eps = pow(cost(),T(.25)); if (eps > T(.1)) eps = T(.1);
		unsigned i=0; while (norm(P[i])<eps) ++i;
		unsigned j=0; while (norm(Q[j])<eps) ++j;
		bool is_P; cplx l(1);

		if (Q.degree()==0) is_P=true;
		else if (i==j) is_P = (norm(P[i])>norm(Q[i]));
		else is_P = (i<j);

		if ((is_P) && (i<P.degree()))  l = pow(P[i],cplx(T(1)/T(P.degree()-i)));
		if ((!is_P) && (j<Q.degree())) l = pow(Q[j],cplx(T(1)/T(Q.degree()-j)));
		if ((l!=cplx(0.0))&&(norm(l)>eps)) { linear(T(1)/l); normalize(); }
		if (norm(l-cplx(1))>eps) make_p_1();
	}

	template <typename T> void Constellation0<T>::belyi () {
		make_c_0(); make_l_1(); normalize(); make_p_1();
	}

	template <typename T> auto Constellation0<T>::logder (cplx z, int k) const -> cplx {
		if (k==0) return T(10)*log((*this)(z));
		cplx sum (0);
		for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i]) / pow (z-b[i], cplx(k));
		for (unsigned i=0; i<f.size(); ++i) sum -= cplx(fd[i]) / pow (z-f[i], cplx(k));
		return sum;
	}

	template <typename T> void Constellation0<T>::readcoo (const Vector<T> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i] = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i] = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i] = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		from_points();
	}

	template <typename T> void Constellation0<T>::readvec (const Vector<cplx> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i] = xy[i];
		for (unsigned i=0; i<n2; ++i) w[i] = xy[n1+i];
		for (unsigned i=0; i<n3; ++i) f[i] = xy[n1+n2+i];
		from_points();
	}

	template <typename T> auto Constellation0<T>::vec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f) const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()); unsigned i=0;
		for (auto z : b) { bw[i++] = z; } for (auto z : w) { bw[i++] = z; } for (auto z : f) { bw[i++] = z; }
		return bw;
	}

	template <typename T> Vector<T> Constellation0<T>::coovec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f) const {
		Vector<T> bw (2*(b.size()+w.size()+f.size())); unsigned i=0;
		for (auto z : b) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		for (auto z : w) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		for (auto z : f) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		return bw;
	}

	template <typename T> auto Constellation0<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (P.degree()+1); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sb(0); for (unsigned i=0; i<b.size(); ++i) sb += T(bd[i])*b[i]; out[k++] = sb;
		return out;
	}

	template <typename T> T Constellation0<T>::cost () const	{
	    T out(0); for (auto z : vcost()) out += norm(z);
	    if (std::isnan(double(out))) out = T(1.234567e89);
	    return out;
	}

	template <typename T> auto Constellation0<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(P.degree()+1,P.degree()+1);
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0;
			for (unsigned jj=0; jj<b.size(); ++jj)	if (id==0)     	out(i,j++) = T(- T(10*bd[jj])) / (w[ii]-b[jj]);
			                                      	else           	out(i,j++) = T(id*bd[jj]) / pow(w[ii]-b[jj],cplx(id+1));
			for (unsigned jj=0; jj<w.size(); ++jj)	if (jj!=ii)    	out(i,j++) = T(0);
			                                      	else if (id==0)	out(i,j++) = T(10) * logder(w[ii],1);
			                                      	else           	out(i,j++) = T(- T(id)) * logder(w[ii],id+1);
			for (unsigned jj=0; jj<f.size(); ++jj)	if (id==0)     	out(i,j++) = T(10*fd[jj]) / (w[ii]-f[jj]);
			                                      	else           	out(i,j++) = T(- T(id*fd[jj])) / pow(w[ii]-f[jj],cplx(id+1));
			++i;
		}
		j=0; for (unsigned jj=0; jj<b.size(); ++jj) out(i,j++) = T(bd[jj]); while (j<out.size2()) out(i,j++) = T(0); ++i;
		return out;
	}

	template <typename T> T Constellation0<T>::fg (const Vector<T> & xy, Vector<T> & df) {
		readcoo(xy);
		Vector<cplx> V = vcost();
		Vector<cplx> W = prod(V,conj(jacvcost()));

		for (unsigned i=0; i<W.size(); ++i) { df(2*i) = real(W(i)); df(2*i+1) = imag(W(i)); }
		T ans(0); for (unsigned i=0; i<V.size(); ++i) ans += norm(V(i)); return ans;
	}

	template <typename T> void Constellation0<T>::find (T t) {
		make_l_1(); Vector<T> bw = coovec(b,w,f);

		readcoo(bw); T c = cost(), eps = sqrt(c)/10, nc = c;
		while ((c>t)&&(eps>1e-100)) {
			std::cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			for (auto & z : bw) {	z += eps; readcoo(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; readcoo(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += eps; } }
			if (!flag) eps /= 4; else eps *= 2;
		}
		std::cerr << std::endl;
	}

	template <typename T> T Constellation0_fg (const Vector<T> & xy, Vector<T> & df, void * c) {
		Constellation0<T> * C = (Constellation0<T> *) c; return C->fg(xy,df);
	}

	template <typename T> void Constellation0_cb (const Vector<T> &, T f, void *) {
		static T er (-1); if ((f<er)||(er<T(0))) { std::cerr << f << "          \r"; er = f; }
	}

	template <typename T> void Constellation0<T>::findm () {
		make_l_1(); Vector<T> x = coovec(b,w,f);
		Minimizer<T> M (x.size(),Constellation0_fg<T>,this); M.cb = Constellation0_cb;
		M.minimize_qn (x);
		readcoo(M.x);
		std::cerr << std::endl;
	}

	template <typename T> T Constellation0<T>::findn () {
		// make_l_1();
		Vector<cplx> x = vec(b,w,f); Matrix<cplx> IJ (P.degree()+1,P.degree()+1);
		T c = cost(), old_c = c + T(1);
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; auto old_x = x;
			inv(jacvcost(),IJ); x -= prod(IJ,vcost());
			readvec(x); c = cost();
			if (c > old_c) readvec(old_x);
		}
		std::cerr << std::endl;
		return old_c;
	}

	template <typename T, typename U> Constellation0<U> cconvert (Constellation0<T> & C) {
		Constellation0<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.l = C.l;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation0<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "Keeping " << nd << " digits." << std::endl;

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

	template <> std::ostream & operator<< (std::ostream & os, const Constellation0<gmp100> & C) {
		using T = gmp100;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "Keeping " << nd << " digits." << std::endl << std::endl;

		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) {
			os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
			Polynomial<cpxint> P = guess (C.b[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) {
			os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
			Polynomial<cpxint> P = guess (C.w[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) {
			os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
			Polynomial<cpxint> P = guess (C.f[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << u8"λ     := " << C.l << std::endl;
		Polynomial<cpxint> L = guess (C.l,T(pow(T(.1),nd))); if (L.degree()>0) os << u8"Λ[z_] := " << L << std::endl;
		os << "P[z_] := " << C.P << std::endl;
		os << "Q[z_] := " << C.Q << std::endl;
		return os;
	}

	template <typename T> void Constellation0<T>::draw (Image & img, bool smooth) const {
		T xmin(0), xmax(0), ymin(0), ymax(0);
		for (auto z : b) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : f) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : w) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		cplx center { (xmin+xmax)/T(2), (ymin+ymax)/T(2) }; T scale = T(.75) * std::max(xmax-xmin,ymax-ymin);

		int l = img.w(); img.start = img.now();
		for (auto & c : img) c = Color(0);

		auto f = [&](cplx z) {
			z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z;
			return imag((*this)(z))>0 ? Color(200,250,250) : Color(200,200,250);
		};

		img.tessel(0,0,img.w()-1,img.h()-1, smooth ? aa<T>(f) : [&](coo c){ return f(cplx(c.x,c.y)); });
		img.update();
	};
}
