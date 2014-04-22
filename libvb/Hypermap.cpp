#include <vb/Hypermap.h>

namespace vb {
	bool Hypermap::validate () {
		if (sigma.size() != alpha.size()) return false;
		if (sigma.size() != phi.size()) return false;
		return (sigma*alpha*phi).is_identity();
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

	Hypermap Hypermap::split_edges () {
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
		return Hypermap (sigma_c, alpha_c, phi_c);
	}

	void Hypermap::flip (unsigned e) {
		unsigned b=sigma[e], a=alpha[b], c=sigma[a], d=alpha[c], f=alpha[e], g=phi[f], h=alpha[g], i=phi[g], j=alpha[i];
		if (alpha[phi[alpha[phi[e]]]]==e) return;
		if (phi[alpha[phi[alpha[e]]]]==e) return;
		if ((e==sigma[e])||(f==sigma[f])) return;
		sigma[a]=e; sigma[e]=c; sigma[d]=j; sigma[g]=b; sigma[i]=f; sigma[f]=h;
		phi[a]=g; phi[g]=f; phi[f]=a; phi[d]=e; phi[e]=i; phi[i]=d;
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os   	<< "Hypermap < "
		     	<< H.n_black() << " black, " << H.n_white() << " white, "
		     	<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl;
		// os	<< "  | Black: " << H.sigma << "  | White: " << H.alpha << "  | Faces: " << H.phi;
		return os;
	}
}
