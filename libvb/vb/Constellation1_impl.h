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
		l = cplx(1);
		cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += (*this)(w[i])*cplx(wd[i]); }
		l = cplx(d)/avg;
	}

	// template <typename T> void Constellation1<T>::linear (cplx u, cplx v) {
	//	for (auto & z : b) z = u*z+v; for (auto & z : f) z = u*z+v; for (auto & z : w) z = u*z+v; from_points();
	// }

	// template <typename T> void Constellation1<T>::make_l_1 () {
	//	normalize(); linear (pow(l,cplx(T(1)/T(P.degree()-Q.degree())))); l=T(1);
	// }

	// template <typename T> void Constellation1<T>::make_c_0 () {
	//	cplx sum(0); for (unsigned i=0; i<b.size(); ++i) sum += T(bd[i])*b[i]; sum /= P.degree(); linear (T(1),-sum); normalize();
	// }

	// template <typename T> void Constellation1<T>::make_p_1 () {
	//	T eps = pow(cost(),T(.25)); if (eps > T(.1)) eps = T(.1);
	//	unsigned i=0; while (norm(P[i])<eps) ++i;
	//	unsigned j=0; while (norm(Q[j])<eps) ++j;
	//	bool is_P; cplx l(1);

	//	if (Q.degree()==0) is_P=true;
	//	else if (i==j) is_P = (norm(P[i])>norm(Q[i]));
	//	else is_P = (i<j);

	//	if ((is_P) && (i<P.degree()))  l = pow(P[i],cplx(T(1)/T(P.degree()-i)));
	//	if ((!is_P) && (j<Q.degree())) l = pow(Q[j],cplx(T(1)/T(Q.degree()-j)));
	//	if ((l!=cplx(0.0))&&(norm(l)>eps)) { linear(T(1)/l); normalize(); }
	//	if (norm(l-cplx(1))>eps) make_p_1();
	// }

	// template <typename T> void Constellation1<T>::belyi () {
	//	make_c_0(); make_l_1(); normalize(); make_p_1();
	// }

	template <typename T> auto Constellation1<T>::my_lsigma	(cplx z) const -> cplx { return et1*z*z + log(theta1_(M_PI*z,q) / (M_PI*th1p0)); }
	template <typename T> auto Constellation1<T>::my_zeta  	(cplx z) const -> cplx { return theta1prime_(M_PI*z,q) / theta1_(M_PI*z,q); }

	template <typename T> auto Constellation1<T>::logder (cplx z, int k) const -> cplx {
		if (k==0) { // 0th : sum(log(sigma)) = log(prod(sigma))
			cplx out (l * exp(my_lsigma(z-b[0]+T(dx)+tau*T(dy))));
			for (unsigned i=0; i<b.size(); ++i) out *= exp(my_lsigma(z-b[i]) * T(bd[i] - (i==0?1:0)));
			for (unsigned i=0; i<f.size(); ++i) out /= exp(my_lsigma(z-f[i]) * T(fd[i]));
			return log(out);
		}
		if (k==1) { // 1st : sum(sigma'/sigma) = sum(zeta)
			cplx out (my_zeta(z-b[0]+T(dx)+tau*T(dy)));
			for (unsigned i=0; i<b.size(); ++i) out += my_zeta(z-b[i]) * T(bd[i] - (i==0?1:0));
			for (unsigned i=0; i<f.size(); ++i) out -= my_zeta(z-f[i]) * T(fd[i]);
			return M_PI * out;
		}
		assert (!"Derivatives starting from 2 not implemented!");
	}

	template <typename T> auto Constellation1<T>::operator() (cplx z) const -> cplx { return exp(logder(z,0)); }

	template <typename T> void Constellation1<T>::readvec (const Vector<cplx> & xy) {
		unsigned i=0;
		for (auto & z : b) z = xy[i++];
		for (auto & z : w) z = xy[i++];
		for (auto & z : f) z = xy[i++];
		tau = xy[i++];
		from_points(); normalize();
	}

	template <typename T> auto Constellation1<T>::vec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t) const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()+1); unsigned i=0;
		for (auto z : b) { bw[i++] = z; }
		for (auto z : w) { bw[i++] = z; }
		for (auto z : f) { bw[i++] = z; }
		bw[i++] = t;
		return bw;
	}

	template <typename T> auto Constellation1<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (d+1); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sz (-T(dx)-T(dy)*tau);
		for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i]) * b[i];
		for (unsigned i=0; i<f.size(); ++i) sz -= T(fd[i]) * f[i];
		out[k++] = sz;
		return out;
	}

	template <typename T> T Constellation1<T>::cost () const	{ T out(0); for (auto z : vcost()) out += norm(z); return out; }

	template <typename T> auto Constellation1<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		// Matrix<cplx> out(P.degree()+1,P.degree()+1);
		// unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0;
		//	for (unsigned jj=0; jj<b.size(); ++jj)	if (id==0)     	out(i,j++) = T(- T(10*bd[jj])) / (w[ii]-b[jj]);
		//	                                      	else           	out(i,j++) = T(id*bd[jj]) / pow(w[ii]-b[jj],cplx(id+1));
		//	for (unsigned jj=0; jj<w.size(); ++jj)	if (jj!=ii)    	out(i,j++) = T(0);
		//	                                      	else if (id==0)	out(i,j++) = T(10) * logder(w[ii],1);
		//	                                      	else           	out(i,j++) = T(- T(id)) * logder(w[ii],id+1);
		//	for (unsigned jj=0; jj<f.size(); ++jj)	if (id==0)     	out(i,j++) = T(10*fd[jj]) / (w[ii]-f[jj]);
		//	                                      	else           	out(i,j++) = T(- T(id*fd[jj])) / pow(w[ii]-f[jj],cplx(id+1));
		//	++i;
		// }
		// j=0; for (unsigned jj=0; jj<b.size(); ++jj) out(i,j++) = T(bd[jj]); while (j<out.size2()) out(i,j++) = T(0); ++i;
		// return out;
	}

	template <typename T> void Constellation1<T>::find () {
		Vector<cplx> bw = vec(b,w,f,tau);
		T c = cost(), eps = sqrt(c)/10, nc = c; int t=0;

		while (eps>1e-100) {
			// std::cerr << std::endl << *this;
			std::cerr << "\r" << t++ << ": " << c << " (" << eps << ") [" << tau << "]          ";
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

	template <typename T> void Constellation1<T>::findm () {
		// make_l_1(); Vector<T> x = coovec(b,w,f);
		// Minimizer<T> M (x.size(),Constellation1_fg<T>,this); M.cb = Constellation1_cb;
		// M.minimize_qn (x);
		// readcoo(M.x);
		// std::cerr << std::endl;
	}

	template <typename T> void Constellation1<T>::findn () {
		// make_l_1(); Vector<cplx> x = vec(b,w,f); Matrix<cplx> IJ (P.degree()+1,P.degree()+1);
		// T c = cost(), old_c = c + T(1);
		// while (c<old_c) {
		//	std::cerr << c << "             \r"	;
		//	old_c = c; auto old_x = x;
		//	inv(jacvcost(),IJ); x -= prod(IJ,vcost());
		//	readvec(x); c = cost();
		//	if (c > old_c) readvec(old_x);
		// }
		// std::cerr << std::endl;
	}

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C) {
		Constellation1<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.l = C.l;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C) {
		os << "tau = " << C.tau << std::endl;
		os << "log(lambda) = " << log(C.l) << std::endl;
		os << "log(cost) = " << log(C.cost()) << std::endl;
		os << std::endl;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(10) << std::fixed;
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
			img->put(coo(i,j), imag((*this)(z))>0 ? Color(200,255,200) : Color(200,200,255));
		}
		for (auto z : b) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), BLACK); }
		for (auto z : w) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), WHITE); }
		for (auto z : f) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), RED); }
		return img;
	};
}
