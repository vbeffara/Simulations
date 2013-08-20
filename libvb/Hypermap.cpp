#include <vb/Hypermap.h>

namespace vb {
	void Permutation::s_to_c () {
		std::vector<int> done (size(), 0);
		for (int i=0; i<size(); ++i) {
			if (done[i]) continue;
			std::vector<int> v (1,i); done[i]=1;
			for (int j=at(i); done[j]==0; j=at(j)) { v.push_back(j); done[j]=1; }
			c.push_back(v);
		}
	}

	void Permutation::c_to_s () {
		int sz=0; for (std::vector<int> v : c) sz += v.size();
		std::vector<int>::operator= (std::vector<int> (sz));
		for (std::vector<int> v : c) {
			for (int i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
	}

	void Hypermap::validate () {
		assert (sigma.size() == alpha.size());
		assert (sigma.size() == phi.size());
		for (int i=0; i<sigma.size(); ++i) assert (phi[alpha[sigma[i]]] == i);
	}

	std::ostream & operator<< (std::ostream &os, Permutation &P) {
		os << "(";
		for (std::vector<int> cc : P.c) {
			os << " ("; for (int i : cc) os << " " << i; os << " )";
		}
		return os << " )" << std::endl;
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os  << "Hypermap < "
			<< H.n_black() << " black, " << H.n_white() << " white, "
			<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl
			<< "  | Black: " << H.sigma << "  | White: " << H.alpha << "  | Faces: " << H.phi;
		return os;
	}
}
