#include <vb/Hypermap.h>
#include <cassert>
#include <sstream>

namespace vb {
	bool Hypermap::validate () const {
		if (sigma.size() != alpha.size()) return false;
		if (sigma.size() != phi.size()) return false;
		return (sigma*alpha*phi).is_identity();
	}

	void Hypermap::from_hypermap () {
		Cycles sc = sigma.cycles();
		int nb = sc.size(); V.resize(nb); E.resize(6*nb);
		for (int i=0; i<E.size(); ++i) E[i].i=i;
		for (int i=0; i<V.size(); ++i) V[i].i=i;
		for (auto & v : V) {
			v.z = NAN; v.bone=false; v.adj.clear();
			for (int e : sc[v.i]) { E[e].a = NAN; E[e].src = v.i; }
		}
		for (auto & v : V) for (int e : sc[v.i]) { v.adj.push_back(E[alpha[e]].src); }
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

	bool Hypermap::is_simple (int d) const {
		for (auto s : sigma.cycles())	if (s.size()<=d) return false;
		for (auto f : phi.cycles())  	if (f.size()<=d) return false;
		return true;
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
		initial.resize (sigma.size(),0);
		for (int i=0; i<N; ++i) {
			initial[alpha[i]] = (initial[i]&1 ? 1 : 0);
			if ((initial[i]&2) && (initial[alpha[sigma[sigma[sigma[alpha[i]]]]]]&2)) initial[alpha[i]] |= 4;
		}
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
		int n=alpha.size(), m=0; std::vector<unsigned> s1(n,n), s2(n,n);
		auto go = [&] (int i) { while (s1[i]==n) { s1[i]=m; s2[m]=i; ++m; i=alpha[i]; } };
		go(i); for (int x : s2) go(phi[x]); return s1;
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

	std::string Hypermap::name () const {
		assert(is_triangulation());
		std::ostringstream os;
		for (auto v : phi.cycles()) { os << char('a' + v[1] - v[0] - 1) << char('a' + v[2] - v[0] - 1); }
		return os.str();
	}

	void Hypermap::mirror () {
		alpha = sigma*phi; sigma = sigma.inverse(); phi = phi.inverse();
	}

	void Hypermap::dual () {
		sigma = sigma.inverse(); alpha = alpha.inverse(); phi = phi.inverse(); swap(sigma,phi);
	}

	void Hypermap::simplify1 () {
		int n = sigma.size();
		bool finished = false;
		while (!finished) {
			finished = true;
			for (auto v : sigma.cycles()) {
				if (v.size() != 1) continue;
				finished = false;

				int jj=v[0], ii=alpha[jj]; Permutation p(n);
				if (jj==n-2) { p[ii] = n-2; p[n-2] = n-1; p[n-1] = ii; } else { p[ii]=n-2; p[n-2]=ii; p[jj]=n-1; p[n-1]=jj; }
				relabel(p);

				unsigned i=phi[n-1], j=sigma[n-2], k=alpha[j];
				sigma[i]=j; phi[k]=i; initial[j] |= initial[n-1]; initial[i] |= initial[n-2];
				sigma.resize(n-2); alpha.resize(n-2); phi.resize(n-2); initial.resize(n-2); n-=2;
				assert(validate());

				break;
			}
		}
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
				initial[ii] |= initial[n-2]; initial[jj] |= initial[n-1];
				sigma.resize(n-2); alpha.resize(n-2); phi.resize(n-2); initial.resize(n-2); n-=2;
				phi = (sigma*alpha).inverse();

				break;
			}
		}
	}

	void Hypermap::simplify (int d) {
		while (!is_simple(d)) {
			if (d>=1) { simplify1(); dual(); simplify1(); dual(); }
			if (d>=2) { simplify2(); dual(); simplify2(); dual(); }
		}
	}

	void Hypermap::relabel (const Permutation & p) {
		sigma = sigma.conjugate(p); alpha = alpha.conjugate(p); phi = phi.conjugate(p);
		std::vector<unsigned> b (sigma.size()); for (int i=0; i<sigma.size(); ++i) b[p[i]] = initial[i]; initial=b;
	}

	void Hypermap::dessin () {
		Cycles new_a, new_f; int n=sigma.size(); initial.resize(6*n);
		for (unsigned i=0; i<n; ++i) {
			new_a.push_back({i,i+n}); new_a.push_back({i+2*n,i+3*n}); new_a.push_back({i+4*n,i+5*n});
			new_f.push_back({i,i+2*n,i+4*n}); new_f.push_back({alpha[i]+n,phi[i]+5*n,i+3*n});
			initial[i]=3; initial[i+n]=5; initial[i+2*n]=4; initial[i+3*n]=8; initial[i+4*n]=8; initial[i+5*n]=2;
		}
		alpha = new_a; phi = new_f; sigma = (alpha*phi).inverse();
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os	<< "Hypermap < "
		  	<< H.n_black() << " black, " << H.n_white() << " white, "
		  	<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl;
		return os;
	}

	double Hypermap::alpha_xyz (double x, double y, double z) const {
		return acos ( (x*(x+y+z) - y*z) / ((x+y)*(x+z)) );
	}

	double Hypermap::ccn (int n) const {
		static std::vector<double> p;
		for (int i=p.size(); i<=n; ++i) p.push_back (sqrt(2/(1-cos(2*M_PI/i)))-1);
		return p[n];
	}

	void Hypermap::acpa () {
		double e = 1, old_e = 2;
		for (auto & v : V) if (v.adj.size()==2) v.r=0;
		while ((e > 1e-3) || (e < old_e)) {
			std::cerr << e << "      \r"; old_e = e; e = 0;
			for (auto & v : V) {
				int n = v.adj.size(); if ((v.fixed) || (n==2)) continue;
				double s=0, r0=v.r, r1, r2=V[v.adj.back()].r;
				for (int ll : v.adj) { r1=r2; r2=V[ll].r; s += alpha_xyz (r0,r1,r2); }
				double c=cos(s/n), nr=ccn(n) * (1-c + sqrt(2*(1-c))) / (1+c);
				e += fabs(1-nr);
				v.r *= 1.1 * nr - .1;
			}
		}
	}
}
