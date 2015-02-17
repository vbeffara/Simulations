#pragma once
#include <vb/Constellation0.h>
#include <vb/NumberTheory.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation0<T>::Constellation0 (Hypermap M, Hub H, int n) {
		p = { T(1) };
		Hypermap M2 (M); M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Spheroidal S (M2,H); S.pack(); std::cerr << std::endl;

		unsigned N = M.sigma.size(), inf = 0, dinf = 0;
		for (auto c : M.phi.cycles()) { unsigned i = S.E[c[0]+3*N].src, d = S.V[i].adj.size(); if (d>dinf) { inf=i; dinf=d; } }

		S.linear (1,-S.V[inf].z); S.inversion(); S.linear (-1/S.V[inf].r,0); S.mobiusto0 (S.V[S.E[0].src].z);

		for (auto c : M.sigma.cycles())	{                              	b.push_back( { S.V[S.E[c[0]].src].z,    	c.size() } ); }
		for (auto c : M.alpha.cycles())	{                              	w.push_back( { S.V[S.E[c[0]+N].src].z,  	c.size() } ); }
		for (auto c : M.phi.cycles())  	{ if (S.E[c[0]+3*N].src != inf)	f.push_back( { S.V[S.E[c[0]+3*N].src].z,	c.size() } ); }

		make_c_0();
		// linear (T(1),-b[0].z);
	}

	template <typename T> Constellation0<T>::Constellation0 () { p = { T(1) }; }

	template <typename T> auto Constellation0<T>::operator() (cplx z) const -> cplx {
		cplx out (p[0]);
		for (auto zd : b) out *= pow (z-zd.z, zd.d);
		for (auto zd : f) out /= pow (z-zd.z, zd.d);
		return out;
	}

	template <typename T> void Constellation0<T>::normalize () {
		p[0] = cplx(1);
		cplx avg (0); unsigned d=0;
		for (auto zd : w) { d += zd.d; avg += (*this)(zd.z)*cplx(zd.d); }
		p[0] = cplx(d)/avg;
	}

	template <typename T> void Constellation0<T>::linear (cplx u, cplx v) {
		for (auto & zd : b) zd.z = u*zd.z+v; for (auto & zd : f) zd.z = u*zd.z+v; for (auto & zd : w) zd.z = u*zd.z+v;
	}

	template <typename T> void Constellation0<T>::make_l_1 () {
		normalize();
		int deg=0; for (auto zd : b) deg += zd.d; for (auto zd : f) deg -= zd.d;
		linear (pow(p[0],cplx(T(1)/T(deg)))); p[0]=T(1);
	}

	template <typename T> void Constellation0<T>::make_c_0 () {
		int deg=0; for (auto zd : b) deg += zd.d;
		cplx sum(0); for (auto zd : b) sum += T(zd.d)*zd.z; sum /= deg; linear (T(1),-sum); normalize();
	}

	template <typename T> void Constellation0<T>::make_p_1 () {
		T eps = pow(cost(),T(.25)); if (eps > T(.1)) eps = T(.1);
		Polynomial<cplx> P; for (auto zd : b) for (unsigned j=0; j<zd.d; ++j) P.add_root(zd.z);
		Polynomial<cplx> Q; for (auto zd : f) for (unsigned j=0; j<zd.d; ++j) Q.add_root(zd.z);
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
		for (auto zd : b) sum += cplx(zd.d) / pow (z-zd.z, k);
		for (auto zd : f) sum -= cplx(zd.d) / pow (z-zd.z, k);
		return sum;
	}

	template <typename T> void Constellation0<T>::readcoo (const Vector<T> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i].z = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i].z = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i].z = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
	}

	template <typename T> void Constellation0<T>::readvec (const Vector<cplx> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i].z = xy[i];
		for (unsigned i=0; i<n2; ++i) w[i].z = xy[n1+i];
		for (unsigned i=0; i<n3; ++i) f[i].z = xy[n1+n2+i];
	}

	template <typename T> auto Constellation0<T>::vec () const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()); unsigned i=0;
		for (auto zd : b) { bw[i++] = zd.z; }
		for (auto zd : w) { bw[i++] = zd.z; }
		for (auto zd : f) { bw[i++] = zd.z; }
		return bw;
	}

	template <typename T> Vector<T> Constellation0<T>::coovec (const std::vector<Star<T>> & b, const std::vector<Star<T>> & w, const std::vector<Star<T>> & f) const {
		Vector<T> bw (2*(b.size()+w.size()+f.size())); unsigned i=0;
		for (auto z : b) { bw[i++] = (real(z.z)); bw[i++] = (imag(z.z)); }
		for (auto z : w) { bw[i++] = (real(z.z)); bw[i++] = (imag(z.z)); }
		for (auto z : f) { bw[i++] = (real(z.z)); bw[i++] = (imag(z.z)); }
		return bw;
	}

	template <typename T> auto Constellation0<T>::vcost() const -> Vector<cplx> {
		int deg=0; for (auto zd : b) deg += zd.d;
		Vector<cplx> out (deg+1); int k=0;
		for (auto zd : w) for (unsigned j=0; j<zd.d; ++j) out[k++] = logder(zd.z,j);
		cplx sb(0); for (auto zd : b) sb += T(zd.d)*zd.z; out[k++] = sb;
		// cplx sb(0); sb += b[0].z; out[k++] = sb;
		return out;
	}

	template <typename T> auto Constellation0<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		int deg=0; for (auto zd : b) deg += zd.d;
		Matrix<cplx> out(deg+1,deg+1);
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<w[ii].d; ++id) { j=0;
			for (unsigned jj=0; jj<b.size(); ++jj)	if (id==0)     	out(i,j++) = T(- T(10*b[jj].d)) / (w[ii].z-b[jj].z);
			                                      	else           	out(i,j++) = T(id*b[jj].d) / pow(w[ii].z-b[jj].z,cplx(id+1));
			for (unsigned jj=0; jj<w.size(); ++jj)	if (jj!=ii)    	out(i,j++) = T(0);
			                                      	else if (id==0)	out(i,j++) = T(10) * logder(w[ii].z,1);
			                                      	else           	out(i,j++) = T(- T(id)) * logder(w[ii].z,id+1);
			for (unsigned jj=0; jj<f.size(); ++jj)	if (id==0)     	out(i,j++) = T(10*f[jj].d) / (w[ii].z-f[jj].z);
			                                      	else           	out(i,j++) = T(- T(id*f[jj].d)) / pow(w[ii].z-f[jj].z,cplx(id+1));
			++i;
		}
		j=0; for (unsigned jj=0; jj<b.size(); ++jj) out(i,j++) = T(b[jj].d); while (j<out.size2()) out(i,j++) = T(0); ++i;
		// j=0; for (unsigned jj=0; jj<b.size(); ++jj) out(i,j++) = (jj==0 ? T(1) : T(0)); while (j<out.size2()) out(i,j++) = T(0); ++i;
		return out;
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

	template <typename T, typename U> Constellation0<U> cconvert (Constellation0<T> & C) {
		Constellation0<U> CC;
		for (auto zd : C.b) CC.b.push_back({std::complex<U>(zd.z), zd.d});
		for (auto zd : C.w) CC.w.push_back({std::complex<U>(zd.z), zd.d});
		for (auto zd : C.f) CC.f.push_back({std::complex<U>(zd.z), zd.d});
		for (auto z : C.p)  CC.p.push_back(std::complex<U>(z));
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation0<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "Keeping " << nd << " digits." << std::endl;

		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.b[i].d << "\t" << C.b[i].z << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.w[i].d << "\t" << C.w[i].z << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.f[i].d << "\t" << C.f[i].z << std::endl;
		os << std::endl;
		os << u8"λ     := " << C.p[0] << std::endl;
		Polynomial<std::complex<T>> P; for (auto zd : C.b) for (unsigned j=0; j<zd.d; ++j) P.add_root(zd.z); os << "P[z_] := " << P << std::endl;
		Polynomial<std::complex<T>> Q; for (auto zd : C.f) for (unsigned j=0; j<zd.d; ++j) Q.add_root(zd.z); os << "Q[z_] := " << Q << std::endl;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Constellation0<gmp100> & C) {
		using T = gmp100;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "Keeping " << nd << " digits." << std::endl << std::endl;

		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) {
			os << "| " << C.b[i].d << "\t" << C.b[i].z << std::endl;
			Polynomial<cpxint> P = guess (C.b[i].z,T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) {
			os << "| " << C.w[i].d << "\t" << C.w[i].z << std::endl;
			Polynomial<cpxint> P = guess (C.w[i].z,T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) {
			os << "| " << C.f[i].d << "\t" << C.f[i].z << std::endl;
			Polynomial<cpxint> P = guess (C.f[i].z,T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << u8"λ     := " << C.p[0] << std::endl;
		Polynomial<cpxint> L = guess (C.p[0],T(pow(T(.1),nd))); if (L.degree()>0) os << u8"Λ[z_] := " << L << std::endl;
		Polynomial<cpx100> P; for (auto zd : C.f) for (unsigned j=0; j<zd.d; ++j) P.add_root(zd.z); os << "P[z_] := " << P << std::endl;
		Polynomial<cpx100> Q; for (auto zd : C.f) for (unsigned j=0; j<zd.d; ++j) Q.add_root(zd.z); os << "Q[z_] := " << Q << std::endl;
		return os;
	}

	template <typename T> void Constellation0<T>::draw (Image & img, bool smooth) const {
		T xmin(0), xmax(0), ymin(0), ymax(0);
		for (auto z : b) { xmin=std::min(xmin,real(z.z)); xmax=std::max(xmax,real(z.z)); ymin=std::min(ymin,imag(z.z)); ymax=std::max(ymax,imag(z.z)); }
		for (auto z : f) { xmin=std::min(xmin,real(z.z)); xmax=std::max(xmax,real(z.z)); ymin=std::min(ymin,imag(z.z)); ymax=std::max(ymax,imag(z.z)); }
		for (auto z : w) { xmin=std::min(xmin,real(z.z)); xmax=std::max(xmax,real(z.z)); ymin=std::min(ymin,imag(z.z)); ymax=std::max(ymax,imag(z.z)); }
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
