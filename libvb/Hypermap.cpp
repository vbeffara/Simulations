#include <vb/Hypermap.h>

namespace vb {
	bool Hypermap::validate () const {
		if (sigma.size() != alpha.size()) return false;
		if (sigma.size() != phi.size()) return false;
		return (sigma*alpha*phi).is_identity();
	}

	bool Hypermap::is_graph () const {
		for (auto v : alpha.cycles()) if (v.size() != 2) return false;
		return true;
	}

	bool Hypermap::is_triangulation () const {
		if (!(is_graph())) return false;
		for (auto f : phi.cycles()) if (f.size() != 3) return false;
		return true;
	}

	bool Hypermap::is_simple () const {
		for (auto s : sigma.cycles())	if (s.size()<=2) return false;
		for (auto f : phi.cycles())  	if (f.size()<=2) return false;
		return true;
	}

	void Hypermap::output_dot (std::ostream & os) {
		os << "graph { graph [dimen=3]" << std::endl;
		for (unsigned i=0; i<n_black(); ++i) os << "  b" << i << " [shape=point];" << std::endl;
		for (unsigned i=0; i<n_white(); ++i) os << "  w" << i << " [shape=point,fillcolor=white];" << std::endl;
		for (unsigned i=0; i<n_edges(); ++i) os << "  b" << sigma.l[i] << " -- w" << alpha.l[i] << ";" << std::endl;
		os << "}" << std::endl;
	}

	void Hypermap::output_graph_dot (std::ostream & os) {
		os << "graph { graph [dimen=3]" << std::endl;
		for (unsigned i=0; i<n_black(); ++i)
			os << "  " << i << " [shape=point];" << std::endl;
		for (unsigned i=0; i<n_edges(); ++i)
			if (i<alpha[i])
				os << "  " << sigma.l[i] << " -- " << sigma.l[alpha[i]] << ";" << std::endl;
		os << "}" << std::endl;
	}

	void Hypermap::split_edges () {
		unsigned N = n_edges();
		Cycles sigma_c, alpha_c, phi_c;
		sigma_c = sigma.cycles();

		for (unsigned a=0; a<N; ++a) {
			unsigned b=alpha[a], c=phi[a], f=sigma[a], x=phi[b];
			if (a<b) sigma_c.push_back ({a+N,x+3*N,b+2*N,b+N,c+3*N,a+2*N});
			alpha_c.push_back ({a,a+N}); alpha_c.push_back ({a+2*N,a+3*N});
			phi_c.push_back ({a,a+2*N,f+N});
		}
		for (auto F : phi.cycles()) {
			std::vector<unsigned> FF = F;
			for (unsigned &i : FF) i += 3*N;
			phi_c.push_back (FF);
		}

		sigma = sigma_c; alpha = alpha_c; phi = phi_c;
		initial.resize (sigma.size(),false); for (int i=0; i<N; ++i) initial[alpha[i]] = initial[i];
	}

	void Hypermap::flip (unsigned e) {
		unsigned b=sigma[e], a=alpha[b], c=sigma[a], d=alpha[c], f=alpha[e], g=phi[f], h=alpha[g], i=phi[g], j=alpha[i];
		if (alpha[phi[alpha[phi[e]]]]==e) return;
		if (phi[alpha[phi[alpha[e]]]]==e) return;
		if ((e==sigma[e])||(f==sigma[f])) return;
		sigma[a]=e; sigma[e]=c; sigma[d]=j; sigma[g]=b; sigma[i]=f; sigma[f]=h;
		phi[a]=g; phi[g]=f; phi[f]=a; phi[d]=e; phi[e]=i; phi[i]=d;
	}

	Permutation Hypermap::rebasing (unsigned i) const {
		int n = alpha.size(), m = 0;
		std::vector<unsigned> s1(n,n), s2(n,n);
		s1[i]=m; s2[m]=i; ++m;
		for (unsigned k=0; k<n; ++k) {
			int x = s2[k];
			if (s1[alpha[x]]==n) { s1[alpha[x]] = m; s2[m]=alpha[x]; ++m; }
			if (s1[phi[x]]==n) { s1[phi[x]] = m; s2[m]=phi[x]; ++m; }
		}
		return s1;
	}

	Permutation Hypermap::rebasing () const {
		Permutation s = rebasing(0), a = alpha.conjugate(s), p = phi.conjugate(s);
		for (int i=1; i<alpha.size(); ++i) {
			Permutation s2 = rebasing(i), a2 = alpha.conjugate(s2), p2 = phi.conjugate(s2);
			if ((a2<a) || ((a2==a) && (p2<p))) { s=s2; a=a2; p=p2; }
		}
		return s;
	}

	void Hypermap::normalize () { relabel(rebasing()); }

	void Hypermap::mirror () {
		alpha = sigma*phi; sigma = sigma.inverse(); phi = phi.inverse();
	}

	void Hypermap::dual () {
		sigma = sigma.inverse(); alpha = alpha.inverse(); phi = phi.inverse(); swap(sigma,phi);
	}

	void Hypermap::simplify2 () {
		int n = sigma.size();
		bool finished = false;
		while (!finished) {
			finished = true;
			for (auto v : sigma.cycles()) {
				if (v.size() != 2) continue;
				finished = false;

				int i=std::min(v[0],v[1]), j=std::max(v[0],v[1]); Permutation p(n);
				if (j==n-2) { p[i] = n-2; p[n-2] = n-1; p[n-1] = i; } else { p[i]=n-2; p[n-2]=i; p[j]=n-1; p[n-1]=j; }
				relabel(p);

				int ii=alpha[n-2], jj=alpha[n-1]; alpha[ii]=jj; alpha[jj]=ii;
				sigma.resize(n-2); alpha.resize(n-2); phi.resize(n-2); n -= 2;
				phi = (sigma*alpha).inverse();

				break;
			}
		}
	}

	void Hypermap::simplify () {
		while (!is_simple()) {
			simplify2();                	// Remove vertices of degree 2
			dual(); simplify2(); dual();	// Remove pairs of adjacent edges
		}
	}

	void Hypermap::relabel (const Permutation & p) {
		sigma = sigma.conjugate(p); alpha = alpha.conjugate(p); phi = phi.conjugate(p);
		std::vector<bool> b (sigma.size()); for (int i=0; i<sigma.size(); ++i) b[p[i]] = initial[i]; initial=b;
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os   	<< "Hypermap < "
		     	<< H.n_black() << " black, " << H.n_white() << " white, "
		     	<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl;
		// os	<< "  | Black: " << H.sigma << "  | White: " << H.alpha << "  | Faces: " << H.phi;
		return os;
	}
}
