#include <vb/Hypermap.h>

namespace vb {
	bool Hypermap::validate () {
		if (sigma.size() != alpha.size()) return false;
		if (sigma.size() != phi.size()) return false;
		return (sigma*alpha*phi).is_identity();
	}

	void Hypermap::output_dot (std::ostream & os) {
		os << "graph { graph [dimen=3]" << std::endl;
		for (int i=0; i<n_black(); ++i) os << "  b" << i << " [shape=point];" << std::endl;
		for (int i=0; i<n_white(); ++i) os << "  w" << i << " [shape=point,fillcolor=white];" << std::endl;
		for (int i=0; i<n_edges(); ++i) os << "  b" << sigma.l[i] << " -- w" << alpha.l[i] << ";" << std::endl;
		os << "}" << std::endl;
	}

	void Hypermap::output_graph_dot (std::ostream & os) {
		os << "graph { graph [dimen=3]" << std::endl;
		for (int i=0; i<n_black(); ++i)
			os << "  " << i << " [shape=point];" << std::endl;
		for (int i=0; i<n_edges(); ++i)
			if (i<alpha[i])
				os << "  " << sigma.l[i] << " -- " << sigma.l[alpha[i]] << ";" << std::endl;
		os << "}" << std::endl;
	}

	Hypermap Hypermap::split_edges () {
		Hypermap H; H.sigma = sigma;
		int N = n_edges();

		for (int a=0; a<N; ++a) {
			int b=alpha[a], c=phi[a], f=sigma[a], x=phi[b];
			if (a<b) H.sigma.cycles().push_back ({a+N,x+3*N,b+2*N,b+N,c+3*N,a+2*N});         	// TODO modify the permutation differently
			H.alpha.cycles().push_back ({a,a+N}); H.alpha.cycles().push_back ({a+2*N,a+3*N});	// ... and here too
			H.phi.cycles().push_back ({a,a+2*N,f+N});                                        	// ... and here as well
		}
		for (auto F : phi.cycles()) {
			std::vector<int> FF = F;
			for (int &i : FF) i += 3*N;
			H.phi.cycles().push_back (FF); // ... and also here
		}
		H.sigma.use_c(); H.alpha.use_c(); H.phi.use_c();
		return H;
	}

	void Hypermap::flip (int e, bool fast) {
		int b=sigma[e], a=alpha[b], c=sigma[a], d=alpha[c], f=alpha[e], g=phi[f], h=alpha[g], i=phi[g], j=alpha[i];
		if (alpha[phi[alpha[phi[e]]]]==e) return;
		if (phi[alpha[phi[alpha[e]]]]==e) return;
		if ((e==sigma[e])||(f==sigma[f])) return;
		sigma[a]=e; sigma[e]=c; sigma[d]=j; sigma[g]=b; sigma[i]=f; sigma[f]=h;
		phi[a]=g; phi[g]=f; phi[f]=a; phi[d]=e; phi[e]=i; phi[i]=d;
		if (!fast) { sigma.use_s(); phi.use_s(); }
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os   	<< "Hypermap < "
		     	<< H.n_black() << " black, " << H.n_white() << " white, "
		     	<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl;
		// os	<< "  | Black: " << H.sigma << "  | White: " << H.alpha << "  | Faces: " << H.phi;
		return os;
	}
}
