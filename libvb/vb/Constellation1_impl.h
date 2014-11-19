#pragma once
#include <vb/Constellation1.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Toroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation1<T>::Constellation1 (Hypermap M, Hub H, int n) {
		Hypermap M2 (M); M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Toroidal S (M2,H); S.pack(); std::cerr << std::endl; S.output_pdf();

		unsigned N = M.sigma.size(); d=0;
		for (auto c : M.sigma.cycles())	{ b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); d+=bd.back(); }
		for (auto c : M.alpha.cycles())	{ w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
		for (auto c : M.phi.cycles())  	{ f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }
		tau = S.m;
		from_points(); normalize();
	}

	template <typename T> Constellation1<T>::Constellation1 () {}

	template <typename T> void Constellation1<T>::from_points () {
		q = q_(tau); et1 = eta1_(q); th1p0 = theta1prime_(0,q);
		cplx sz(0); for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i])*b[i];
		cplx sp(0); for (unsigned i=0; i<f.size(); ++i) sp += T(fd[i])*f[i];
		dy = round(imag(sz-sp)/imag(tau)); dx = round(real(sz-sp-T(dy)*tau));
	}

	template <typename T> void Constellation1<T>::normalize () {
		cplx sf(0); int sdf(0); for (unsigned i=0; i<f.size(); ++i) { sdf+=fd[i]; sf += f[i] * T(fd[i]); }
		for (auto & z : b) z -= sf/T(sdf); for (auto & z : w) z -= sf/T(sdf); for (auto & z : f) z -= sf/T(sdf);
		ll = cplx(0); cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += logder(w[i],0) * T(wd[i]); }
		ll = - avg/T(d);
	}

	template <typename T> auto Constellation1<T>::my_lsigma	(cplx z) const -> cplx { return et1*z*z + log(theta1_(M_PI*z,q)); }
	template <typename T> auto Constellation1<T>::my_zeta  	(cplx z) const -> cplx { return theta1prime_(M_PI*z,q) / theta1_(M_PI*z,q); }

	template <typename T> auto Constellation1<T>::logderp (cplx z, int k) const -> cplx {
		if (k==0) return my_lsigma (z);	 // 0th : sum(log(sigma)) = log(prod(sigma))
		if (k==1) return my_zeta (z);  	 // 1st : sum(sigma'/sigma) = sum(zeta)
		assert (!"Derivatives starting from 2 not implemented!");
	}

	template <typename T> auto Constellation1<T>::logder (cplx z, int k) const -> cplx {
		cplx out (logderp (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp (z-f[i], k) * T(fd[i]);
		if (k==0) { out += ll; out -= cplx(0,2*M_PI) * T(round(real(out/cplx(0,2*M_PI)))); }
		if (k==1) { out *= M_PI; }
		return out;
	}

	template <typename T> auto Constellation1<T>::operator() (cplx z) const -> cplx { return exp(logder(z,0)); }

	template <typename T> void Constellation1<T>::readvec (const Vector<cplx> & xy) {
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
		static bool first=true; if (first) { first=false; std::cerr << bw.size() << " variables" << std::endl; }
		return bw;
	}

	template <typename T> auto Constellation1<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (d+2); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sz (-T(dx)-T(dy)*tau); for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i]) * b[i]; out[k++] = sz;
		cplx sf (0); for (unsigned i=0; i<f.size(); ++i) sf -= T(fd[i]) * f[i]; out[k++] = sf;
		static bool first=true; if (first) { first=false; std::cerr << out.size() << " equations" << std::endl; }
		return out;
	}

	template <typename T> T Constellation1<T>::cost () const	{ T out(0); for (auto z : vcost()) out += norm(z); return out; }

	template <typename T> auto Constellation1<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(d+2,d+2);
		// unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0; // f_i is logder(w[ii],id)
		// //	for (unsigned jj=0; jj<b.size(); ++jj)	if (id==0)     	out(i,j++) = T(- T(10*bd[jj])) / (w[ii]-b[jj]);
		// //	                                      	else           	out(i,j++) = T(id*bd[jj]) / pow(w[ii]-b[jj],cplx(id+1));
		// //	for (unsigned jj=0; jj<w.size(); ++jj)	if (jj!=ii)    	out(i,j++) = T(0);
		// //	                                      	else if (id==0)	out(i,j++) = T(10) * logder(w[ii],1);
		// //	                                      	else           	out(i,j++) = T(- T(id)) * logder(w[ii],id+1);
		// //	for (unsigned jj=0; jj<f.size(); ++jj)	if (id==0)     	out(i,j++) = T(10*fd[jj]) / (w[ii]-f[jj]);
		// //	                                      	else           	out(i,j++) = T(- T(id*fd[jj])) / pow(w[ii]-f[jj],cplx(id+1));
		// //	++i;
		// ++i; } // f_i is sum(z*dz) recentered
		// ++i; // f_i is sum(f*df)
		// ++i; // assert (i==d+2);
		return out;
	}

	template <typename T> auto Constellation1<T>::jacnum  () -> Matrix<cplx> {
		Vector<cplx> x = vec (b,w,f,tau,ll), c = vcost(); Matrix<cplx> out (d+2,d+2);
		T eps = std::min (.1, sqrt(cost())/1000);
		for (unsigned j=0; j<x.size(); ++j) {
			x[j] += eps; readvec(x); Vector<cplx> dc = vcost() - c; x[j] -= eps;
			for (unsigned i=0; i<c.size(); ++i) out(i,j) = dc[i] / eps;
		}
		return out;
	}

	template <typename T> void Constellation1<T>::find () {
		Vector<cplx> bw = vec(b,w,f,tau,ll);
		T c = cost(), eps = std::min(.1,c), nc = c;

		while (eps>1e-100) {
			std::cerr << "\r" << c << " (" << eps << ") [" << tau << "]          ";
			bool flag = false;
			for (auto & z : bw) {	z += eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += eps; }
			                     	z += cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= cplx(0,eps); }
			                     	z -= cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += cplx(0,eps); } }
			if (!flag) eps /= 1.618; else eps *= 1.1;
		}
		std::cerr << std::endl;
	}

	template <typename T> T Constellation1_fg (const Vector<T> & xy, Vector<T> & df, void * c) {
		Constellation1<T> * C = (Constellation1<T> *) c; return C->fg(xy,df);
	}

	template <typename T> void Constellation1_cb (const Vector<T> &, T f, void *) {
		static T er (-1); if ((f<er)||(er<T(0))) { std::cerr << f << "          \r"; er = f; }
	}

	template <typename T> void Constellation1<T>::findn () {
		Vector<cplx> x = vec(b,w,f,tau,ll); Matrix<cplx> IJ (x.size(),x.size());
		T c = cost(), old_c = c + T(1);
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; auto old_x = x;
			// std::cerr << jacnum() << std::endl << det(jacnum()) << std::endl;
			inv(jacnum(),IJ); x -= prod(IJ,vcost());
			// std::cerr << x << std::endl;
			readvec(x); c = cost();
			if (c > old_c) readvec(old_x);
		}
		std::cerr << std::endl;
	}

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C) {
		Constellation1<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.l = C.l;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(15) << std::fixed;
		os << "tau = " << C.tau << std::endl;
		os << "log(lambda) = " << C.ll << std::endl;
		os << "log(cost) = " << log(C.cost()) << std::endl;
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

	template <typename T> Image * Constellation1<T>::draw (unsigned l) const {
		T xmin(0), xmax(0), ymin(0), ymax(0);
		for (auto z : b) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : f) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : w) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		cplx center { (xmin+xmax)/T(2), (ymin+ymax)/T(2) }; T scale = T(.75) * std::max(xmax-xmin,ymax-ymin);

		Image * img = new Image (l,l,"Constellation1"); img->show();
		for (unsigned j=0; j<l; ++j) for (unsigned i=0; i<l; ++i) {
			cplx z {T(i),T(j)}; z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z;
			img->put(coo(i,j), imag((*this)(z))>0 ? Color(200,255,255) : Color(200,200,255));
		}
		for (auto z : b) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), BLACK); }
		for (auto z : w) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), WHITE); }
		for (auto z : f) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), RED); }
		return img;
	};
}
