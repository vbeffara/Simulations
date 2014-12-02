#pragma once
#include <vb/Constellation1.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Toroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation1<T>::Constellation1 (Hypermap M, Hub H, int n) {
		Hypermap M2 (M); M2.dessin();
		do {
			M2.split_edges(); Toroidal S (M2,H); S.pack(); S.output_pdf();
			unsigned N = M.sigma.size(); d=0;
			b.clear(); for (auto c : M.sigma.cycles())	{ b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); d+=bd.back(); }
			w.clear(); for (auto c : M.alpha.cycles())	{ w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
			f.clear(); for (auto c : M.phi.cycles())  	{ f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }
			tau = S.m; shift(-b[0]); normalize(); findn();
		} while (cost()>T(1e-6));
	}

	template <typename T> Constellation1<T>::Constellation1 () {}

	template <typename T> void Constellation1<T>::from_points () {
		q = q_(tau); qt = q_t(tau); E = Elliptic<T> { q };
		cplx sz(0); for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i])*b[i];
		cplx sp(0); for (unsigned i=0; i<f.size(); ++i) sp += T(fd[i])*f[i];
		dy = round(double(T(imag(sz-sp)/imag(tau)))); dx = round(double(T(real(sz-sp-T(dy)*tau))));
		for (unsigned i=0; i<f.size(); ++i) {
			int ddx = round(double(dx)/fd[i]); if (ddx != 0) { f[i] += T(ddx); dx -= int(fd[i])*ddx; }
			int ddy = round(double(dy)/fd[i]); if (ddy != 0) { f[i] += T(ddy)*tau; dy -= int(fd[i])*ddy; }
		}
	}

	template <typename T> void Constellation1<T>::normalize () {
		cplx sf(0); int sdf(0); for (unsigned i=0; i<f.size(); ++i) { sdf+=fd[i]; sf += f[i] * T(fd[i]); }
		for (auto & z : b) z -= sf/T(sdf); for (auto & z : w) z -= sf/T(sdf); for (auto & z : f) z -= sf/T(sdf);
		ll = cplx(0); cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += logder(w[i],0) * T(wd[i]); }
		ll = - avg/T(d);
	}

	template <typename T> void Constellation1<T>::shift (cplx z) {
		for (auto & u : b) u += z; for (auto & u : w) u += z; for (auto & u : f) u += z;
		from_points();
	}

	template <typename T> auto Constellation1<T>::logderp (cplx z, int k) const -> cplx {
		if (k==0) return log(E.sigma(z));
		if (k==1) return E.zeta(z);
		if (k==2) return E.wp(z);
		if (k==3) return E.wp_z(z);
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logderp_z (cplx z, int k) const -> cplx {
		if (k==0) return E.zeta(z);
		if (k==1) return - E.wp(z);
		if (k==2) return E.wp_z(z);
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logderp_t (cplx z, int k) const -> cplx {
		if (k==0) return qt * E.sigma_q(z) / E.sigma(z);
		if (k==1) return qt * E.zeta_q(z);
		if (k==2) return qt * E.wp_q(z);
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logder (cplx z, int k) const -> cplx {
		cplx out (logderp (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp (z-f[i], k) * T(fd[i]);
		if (k==0) { out += ll; out -= cplx(0,T(2)*pi_<T>()) * T(round(real(out/cplx(0,T(2)*pi_<T>())))); }
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_z (cplx z, int k) const -> cplx {
		cplx out (logderp_z (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp_z (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp_z (z-f[i], k) * T(fd[i]);
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_t (cplx z, int k) const -> cplx {
		cplx out (logderp_t (z-b[0]+T(dx)+tau*T(dy), k) + T(dy) * logderp_z (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp_t (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp_t (z-f[i], k) * T(fd[i]);
		return out;
	}

	template <typename T> auto Constellation1<T>::operator() (cplx z) const -> cplx { return exp(logder(z,0)); }

	template <typename T> void Constellation1<T>::readvec (const Vector<cplx> & xy) {
		if (imag(xy[d])<0) return;
		unsigned i=0;
		for (auto & z : b) z = xy[i++];
		for (auto & z : w) z = xy[i++];
		for (auto & z : f) z = xy[i++];
		tau = xy[i++]; ll = xy[i++];
		from_points();
	}

	template <typename T> auto Constellation1<T>::vec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t, const cplx & l) const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()+2); unsigned i=0;
		for (auto z : b) { bw[i++] = z; }
		for (auto z : w) { bw[i++] = z; }
		for (auto z : f) { bw[i++] = z; }
		bw[i++] = t; bw[i++] = l;
		return bw;
	}

	template <typename T> auto Constellation1<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (d+2); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sz (T(-dx)+cplx(T(-dy)*tau));
		for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i]) * b[i];
		for (unsigned i=0; i<f.size(); ++i) sz -= T(fd[i]) * f[i]; out[k++] = sz;
		out[k++] = b[0];
		return out;
	}

	template <typename T> T Constellation1<T>::cost () const	{ T out(0); for (auto z : vcost()) out += norm(z); return out; }

	template <typename T> auto Constellation1<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(d+2,d+2,cplx(0));
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0; // f_i is logder(w[ii],id)
			out(i,j) = - logderp_z (w[ii]-b[0]+T(dx)+tau*T(dy), id);
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) -= T(bd[jj] - (jj==0?1:0)) * logderp_z (w[ii]-b[jj],id);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = (ii==jj) ? logder_z (w[ii],id) : cplx(0);
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = T(fd[jj]) * logderp_z (w[ii]-f[jj], id);
			out(i,j++) = logder_t (w[ii],id);
			out(i,j++) = cplx(id==0 ? 1 : 0);
			assert (j==unsigned(d+2));
		++i; } { j=0; // f_i is sum(z*dz) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = T(bd[jj]);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = -T(fd[jj]);
			out(i,j++) = - T(dy);
			out(i,j++) = cplx(0);
			assert (j==unsigned(d+2));
		++i; } { j=0; // f_i is -sum(f*df) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = jj==0 ? T(1) : T(0);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = 0;
			out(i,j++) = cplx(0);
			out(i,j++) = cplx(0);
			assert (j==unsigned(d+2));
		++i; }
		assert (i==unsigned(d+2));
		return out;
	}

	template <typename T> auto Constellation1<T>::jacnum  () -> Matrix<cplx> {
		Vector<cplx> x = vec (b,w,f,tau,ll), c = vcost(); Matrix<cplx> out (d+2,d+2);
		T eps (.00001);
		for (unsigned j=0; j<x.size(); ++j) {
			x[j] += eps; readvec(x); Vector<cplx> dc = vcost() - c; x[j] -= eps;
			for (unsigned i=0; i<c.size(); ++i) out(i,j) = dc[i] / eps;
		}
		return out;
	}

	template <typename T> T Constellation1<T>::find (T e) {
		Vector<cplx> bw = vec(b,w,f,tau,ll);
		T c = cost(), eps = std::min(T(.1),c), nc = c;

		while (eps>e) {
			std::cerr << "\r" << c << " (" << eps << ")        ";
			bool flag = false;
			for (auto & z : bw) {	z += eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += eps; }
			                     	z += cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= cplx(0,eps); }
			                     	z -= cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += cplx(0,eps); } }
			if (!flag) eps /= 1.618; else eps *= 1.1;
		}
		return c;
	}

	template <typename T> T Constellation1<T>::findn () {
		Vector<cplx> x = vec(b,w,f,tau,ll); Matrix<cplx> IJ (x.size(),x.size());
		T c = cost(), old_c = c + T(1); auto old_x = x;
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; old_x = x;
			inv(jacvcost(),IJ); x -= prod(IJ,vcost());
			readvec(x); c = cost();
		}
		readvec(old_x); return old_c;
	}

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C) {
		Constellation1<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.ll = C.ll; CC.tau = C.tau; CC.d = C.d;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-10); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "log(lambda) = " << C.ll << std::endl;
		os << "tau         = " << C.tau << std::endl;
		std::complex<T> jj = C.E.j();
		os << "invariant j = " << jj << std::endl;
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Constellation1<gmp100> & C) {
		using T = gmp100;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-12); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		T eps = pow(T(.1),nd-5);

		os << "log(lambda) = " << C.ll << std::endl;
		os << "tau         = " << C.tau << std::endl;
		{ Polynomial<cpxint> P = guess(C.tau,eps); if (P.degree()>0) os << "\t\troot of " << P << std::endl; }
		std::complex<T> jj = C.E.j();
		os << "invariant j = " << jj << std::endl;
		{ Polynomial<cpxint> P = guess(jj,eps); if (P.degree()>0) os << "\t\troot of " << P << std::endl; }
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) {
			os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
			Polynomial<cpxint> P = guess (C.b[i],eps);
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) {
			os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
			Polynomial<cpxint> P = guess (C.w[i],eps);
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) {
			os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
			Polynomial<cpxint> P = guess (C.f[i],eps);
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		return os;
	}

	template <typename T> void Constellation1<T>::draw (Image & img) const {
		T xmin = std::min(T(0),real(tau)), xmax = std::max(T(1),real(T(1)+tau)), ymin = T(0), ymax = imag(tau);
		cplx center { (xmin+xmax)/T(2), (ymin+ymax)/T(2) }; T scale = T(.75) * std::max(xmax-xmin,ymax-ymin);

		int l = img.w(); img.start = img.now();
		for (auto & c : img) c = Color(0);
		img.tessel(0,0,img.w()-1,img.h()-1,[&](coo c) {
			cplx z {T(c.x),T(c.y)}; z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z;
			if ((T(0)<=imag(z)) && (imag(z)<=imag(tau)) && (real(z) >= real(tau)*imag(z)/imag(tau)) && (real(z) <= T(1)+real(tau)*imag(z)/imag(tau)))
				return imag((*this)(z))>0 ? Color(150,200,200) : Color(150,150,200);
			return imag((*this)(z))>0 ? Color(200,250,250) : Color(200,200,250);
		});
		img.update();
	};
}
