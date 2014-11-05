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
		CPixel (Constellation<T> * C_, std::complex<T> z_ = T(0)) : C(C_), z(z_) {}
		operator Color() { if (C->slow || prng.bernoulli(.01)) c = imag((*C)(C->center + C->scale*z))>0 ? WHITE : BLUE; return c; }
		Constellation<T> * C;
		std::complex<T> z;
		Color c;
	};

	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		Constellation (Hypermap M, Hub H, int n=3, bool d=true);
		Constellation ();

		cplx	operator()	(cplx z)         	const { return l*P(z)/Q(z); }
		cplx	logder    	(cplx z, int k=0)	const;

		void	from_points	();                        	// compute P and Q, don't touch l
		void	linear     	(cplx u, cplx v = cplx(0));	// move the points, recompute P and Q, don't touch l
		void	normalize  	();                        	// choose l to make ones ones
		void	make_l_1   	();                        	// rescale to make l equal to 1
		void	make_c_0   	();                        	// shift everybody to make sum(black)=0
		void	make_p_1   	();                        	// try to have reasonable scaling
		void	belyi      	();                        	// does too many things at once

		T           	cost    	()	const;
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;

		void	find 	();
		void	findm	();
		void	findn	();

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & l)	const;
		Vector<T>   	coovec 	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & l)	const;
		void        	readcoo	(const Vector<T> & xy);
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		void	show	();

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx             	l = T(1);
		Polynomial<cplx> 	P,Q;
		Bitmap<CPixel<T>>	*img = 0;
		cplx center; T scale; bool slow=false;
	};

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C);

	template <typename T> T Constellation_fg (const Vector<T> & xy, Vector<T> & df, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		return C->fg(xy,df);
	}

	template <typename T> void Constellation_cb (const Vector<T> &, T f, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		static T er (-1); T out = f;
		if ((out<er)||(er<T(0))) { std::cerr << out << "          \r"; er = out; }
		if (C->img) C->img->step();
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C);

	/***************************************************************************************************/

	template <typename T> Constellation<T>::Constellation (Hypermap M, Hub H, int n, bool d) {
		unsigned N = M.sigma.size(); Hypermap M2=M; std::vector<int> type (N,-1);

		if (d) M2.dessin();
		else {
			type[0]=0; bool done=false; while (!done) {
				done=true; for (unsigned i=0; i<N; ++i) {
					if (type[i]==-1) continue;
					if (type[M.sigma[i]]==-1) { type[M.sigma[i]] = (type[i]+3)%6; done=false; }
					if ((type[i]<3)&&(type[M.alpha[i]]==-1)) { type[M.alpha[i]] = ((type[i]+1)%3)+3; done=false; }
				}
			}
			for (unsigned i=0; i<N; ++i) M2.initial[i] = 1;
		}

		for (int i=0; i<n; ++i) M2.split_edges();
		Spheroidal S (M2,H); S.pack(); std::cerr << std::endl;

		if (d) {
			unsigned inf=0, dinf=0; for (auto c : M.phi.cycles()) { unsigned i = S.E[c[0]+3*N].src, d = S.V[i].adj.size(); if (d>dinf) { inf=i; dinf=d; } }
			S.linear (1,-S.V[inf].z); S.inversion(); S.linear (-1/S.V[inf].r,0); S.output_pdf();
			{ cpx z; while ((z = S.V[S.E[0].src].z) != 0.0) S.mobiusto0 (z); } S.linear (std::polar(1.0,-S.E[0].a)); S.output_pdf();

			for (auto c : M.sigma.cycles())	{                                      	b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); }
			for (auto c : M.alpha.cycles())	{                                      	w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
			for (auto c : M.phi.cycles())  	{ if (S.E[c[0]+3*N].src==inf) continue;	f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }
		} else { // assumes triangulation with even degrees
			unsigned inf=0; for (auto & c : M.sigma.cycles()) if (type[c[0]]%3 == 2) { inf=c[0]; break; }
			S.linear (1,-S.V[S.E[inf].src].z); S.inversion(); S.linear (-1/S.V[S.E[inf].src].r,0); S.output_pdf();
			{ cpx z; while ((z = S.V[S.E[0].src].z) != 0.0) S.mobiusto0 (z); } S.linear (std::polar(1.0,-S.E[0].a)); S.output_pdf();

			for (auto c : M.sigma.cycles()) {
				if (type[c[0]]%3 == 0) {                         	b.push_back(cplx(S.V[S.E[c[0]].src].z));	bd.push_back(c.size()/2); }
				if (type[c[0]]%3 == 1) {                         	w.push_back(cplx(S.V[S.E[c[0]].src].z));	wd.push_back(c.size()/2); }
				if (type[c[0]]%3 == 2) { if (inf==c[0]) continue;	f.push_back(cplx(S.V[S.E[c[0]].src].z));	fd.push_back(c.size()/2); }
			}
		}

		from_points(); make_c_0(); make_l_1();
	}

	template <typename T> Constellation<T>::Constellation () {}

	template <typename T> void Constellation<T>::show() {
		img = new Bitmap<CPixel<T>> (600,600,"Constellation");
		for (int i=0; i<600; ++i) for (int j=0; j<600; ++j) img->put(coo(i,j), CPixel<T> (this, {T(i-300)/300,T(300-j)/300}));
		img->show();
	}

	template <typename T> void Constellation<T>::from_points () {
		P = Polynomial<cplx> (); Q = Polynomial<cplx> ();
		for (unsigned i=0; i<b.size(); ++i) for (unsigned j=0; j<bd[i]; ++j) P.add_root(b[i]);
		for (unsigned i=0; i<f.size(); ++i) for (unsigned j=0; j<fd[i]; ++j) Q.add_root(f[i]);
		T xmin(0), xmax(0), ymin(0), ymax(0);
		for (auto z : b) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : f) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : w) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		center = cplx { (xmin+xmax)/T(2), (ymin+ymax)/T(2) }; scale = T(.75) * std::max(xmax-xmin,ymax-ymin);
	}

	template <typename T> void Constellation<T>::normalize () {
		l = cplx(1);
		cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += (*this)(w[i])*cplx(wd[i]); }
		l = cplx(d)/avg;
	}

	template <typename T> void Constellation<T>::linear (cplx u, cplx v) {
		for (auto & z : b) z = u*z+v; for (auto & z : f) z = u*z+v; for (auto & z : w) z = u*z+v; from_points();
	}

	template <typename T> void Constellation<T>::make_l_1 () {
		normalize(); linear (pow(l,T(1)/T(P.degree()-Q.degree()))); l=T(1);
	}

	template <typename T> void Constellation<T>::make_c_0 () {
		cplx sum(0); for (unsigned i=0; i<b.size(); ++i) sum += T(bd[i])*b[i]; sum /= P.degree(); linear (T(1),-sum); normalize();
	}

	template <typename T> void Constellation<T>::make_p_1 () {
		T eps = pow(cost()-norm(l-T(1.0)),T(.25)); if (eps > T(.1)) eps = T(.1);
		unsigned i=0; while (norm(P[i])<eps) ++i;
		unsigned j=0; while (norm(Q[j])<eps) ++j;
		bool is_P; cplx l(1);

		if (Q.degree()==0) is_P=true;
		else if (i==j) is_P = (norm(P[i])>norm(Q[i]));
		else is_P = (i<j);

		if ((is_P) && (i<P.degree()))  l = pow(P[i],T(1.0)/T(P.degree()-i));
		if ((!is_P) && (j<Q.degree())) l = pow(Q[j],T(1.0)/T(Q.degree()-j));
		if ((l!=cplx(0.0))&&(norm(l)>eps)) { linear(T(1)/l); normalize(); }
	}

	template <typename T> void Constellation<T>::belyi () {
		make_c_0(); make_l_1(); normalize();
		make_p_1(); make_p_1(); make_p_1();
	}

	template <typename T> auto Constellation<T>::logder (cplx z, int k) const -> cplx {
		if (k==0) return T(10)*log((*this)(z));
		cplx sum (0);
		for (unsigned i=0; i<b.size(); ++i) sum += cplx(bd[i]) / pow (z-b[i], T(k));
		for (unsigned i=0; i<f.size(); ++i) sum -= cplx(fd[i]) / pow (z-f[i], T(k));
		return sum;
	}

	template <typename T> void Constellation<T>::readcoo (const Vector<T> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i] = cplx (xy[2*i],xy[2*i+1]);
		for (unsigned i=0; i<n2; ++i) w[i] = cplx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (unsigned i=0; i<n3; ++i) f[i] = cplx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cplx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);
		from_points();
	}

	template <typename T> void Constellation<T>::readvec (const Vector<cplx> & xy) {
		unsigned n1 = b.size(), n2 = w.size(), n3 = f.size();
		for (unsigned i=0; i<n1; ++i) b[i] = xy[i];
		for (unsigned i=0; i<n2; ++i) w[i] = xy[n1+i];
		for (unsigned i=0; i<n3; ++i) f[i] = xy[n1+n2+i];
		l = xy[n1+n2+n3];
		from_points();
	}

	template <typename T> auto Constellation<T>::vec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & l) const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()+1); unsigned i=0;
		for (auto z : b) { bw[i++] = z; } for (auto z : w) { bw[i++] = z; } for (auto z : f) { bw[i++] = z; } bw[i++] = l;
		return bw;
	}

	template <typename T> Vector<T> Constellation<T>::coovec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & l) const {
		Vector<T> bw (2*(b.size()+w.size()+f.size()+1)); unsigned i=0;
		for (auto z : b) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		for (auto z : w) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		for (auto z : f) { bw[i++] = (real(z)); bw[i++] = (imag(z)); }
		bw[i++] = (real(l)); bw[i++] = (imag(l));
		return bw;
	}

	template <typename T> auto Constellation<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (P.degree()+2); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sb(0); for (unsigned i=0; i<b.size(); ++i) sb += T(bd[i])*b[i]; out[k++] = sb;
		out[k++] = l-T(1);
		return out;
	}

	template <typename T> T Constellation<T>::cost () const	{ T out(0); for (auto z : vcost()) out += norm(z); return out; }

	template <typename T> auto Constellation<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(P.degree()+2,P.degree()+2);
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0;
			for (unsigned jj=0; jj<b.size(); ++jj)	if (id==0)     	out(i,j++) = - T(10*bd[jj]) / (w[ii]-b[jj]);
			                                      	else           	out(i,j++) = T(id*bd[jj]) / pow(w[ii]-b[jj],T(id+1));
			for (unsigned jj=0; jj<w.size(); ++jj)	if (jj!=ii)    	out(i,j++) = T(0);
			                                      	else if (id==0)	out(i,j++) = T(10) * logder(w[ii],1);
			                                      	else           	out(i,j++) = - T(id) * logder(w[ii],id+1);
			for (unsigned jj=0; jj<f.size(); ++jj)	if (id==0)     	out(i,j++) = T(10*fd[jj]) / (w[ii]-f[jj]);
			                                      	else           	out(i,j++) = - T(id*fd[jj]) / pow(w[ii]-f[jj],T(id+1));
			                                      	if (id==0)     	out(i,j++) = T(10)/l; else out(i,j++) = T(0);
			++i;
		}
		j=0; for (unsigned jj=0; jj<b.size(); ++jj) out(i,j++) = T(bd[jj]); while (j<out.size2()) out(i,j++) = T(0); ++i;
		j=0; for (unsigned jj=0; jj<out.size2()-1; ++jj) out(i,j++) = T(0); out(i,j++) = T(1); ++i;
		return out;
	}

	template <typename T> T Constellation<T>::fg (const Vector<T> & xy, Vector<T> & df) {
		readcoo(xy);
		Vector<cplx> V = vcost();
		Vector<cplx> W = prod(V,conj(jacvcost()));

		for (unsigned i=0; i<W.size(); ++i) { df(2*i) = real(W(i)); df(2*i+1) = imag(W(i)); }
		T ans(0); for (unsigned i=0; i<V.size(); ++i) ans += norm(V(i)); return ans;
	}

	template <typename T> void Constellation<T>::find () {
		Vector<T> bw = coovec(b,w,f,l);

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
		Vector<T> x = coovec(b,w,f,l);
		Minimizer<T> M (x.size(),Constellation_fg<T>,this); M.cb = Constellation_cb;
		M.minimize_qn (x);
		readcoo(M.x);
		std::cerr << std::endl;
	}

	template <typename T> void Constellation<T>::findn () {
		Vector<cplx> x = vec(b,w,f,l); T c = cost(), old_c = c + T(1);
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; auto old_x = x;
			auto J = jacvcost(); auto IJ=J; inv(J,IJ);
			auto C = vcost(); x = x - prod(IJ,C);
			readvec(x); c = cost();
			if (c > old_c) readvec(old_x);
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
		double err = C.cost() - norm(C.l-T(1)); os << std::setprecision (err<1e-9 ? -log10(err)/3 : 3) << std::fixed; if (err==T(0)) os << std::setprecision(10);

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
