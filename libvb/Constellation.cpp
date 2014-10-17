#include <vb/Constellation.h>

namespace vb {
	Constellation::Constellation (const Spheroidal & T) {
		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&2)) continue;
			auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : b) if (zz==z) there=true;
			if (!there) { b.push_back(z); bd.push_back(d); }
		}

		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&4)) continue;
			auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : w) if (zz==z) there=true;
			if (!there) { w.push_back(z); wd.push_back(d); }
		}

		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&8)) continue;
			auto & v = T.V[T.E[i].src]; if (v.r<0) continue;
			cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : f) if (zz==z) there=true;
			if (!there) { f.push_back(z); fd.push_back(d); }
		}

		compute();
	}

	void Constellation::compute () {
		RationalFraction R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		Rs = {R};
		for (auto & c : Rs[0].P) c *= l;
		for (int i=0; i<b.size(); ++i) for (int j=Rs.size(); j<bd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<w.size(); ++i) for (int j=Rs.size(); j<wd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<f.size(); ++i) for (int j=Rs.size(); j<fd[i]; ++j) Rs.push_back(Rs.back().derivative());
	}

	void Constellation::normalize () {
		RationalFraction R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		cpx avg = 0; int d=0;
		for (int i=0; i<w.size(); ++i) { d += wd[i]; avg += R(w[i])*cpx(wd[i]); }
		l = cpx(d)/avg;
		compute();
	}

	double Constellation::belyi () {
		cpx z0 = b[0];                      	for (auto & z : b) z -= z0; for (auto & z : w) z -= z0; for (auto & z : f) z -= z0;
		cpx r0 = std::polar(1.0,-arg(w[0]));	for (auto & z : b) z *= r0; for (auto & z : w) z *= r0; for (auto & z : f) z *= r0;
		normalize();

		double lambda = pow(abs(Rs[0].P.back()),1.0/Rs[0].degree());
		for (auto & z : b) z *= lambda; for (auto & z : w) z *= lambda; for (auto & z : f) z *= lambda; normalize();

		double err = sqrt(cost());
		std::cout << std::setprecision (log10(1/err)-1) << std::fixed; Rs[0].P.precision = err; Rs[0].Q.precision = err;
		return lambda;
	}

	double Constellation::cost() const {
		double out=0;
		for (int i=0; i<w.size(); ++i) for (int j=0; j<wd[i]; ++j) out += norm (Rs[j](w[i]) - (j==0?1.0:0.0));
		return out;
	}

	double Constellation::cost (const std::vector<double> & xy) {
		int n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (int i=0; i<n1; ++i) b[i] = cpx (xy[2*i],xy[2*i+1]);
		for (int i=0; i<n2; ++i) w[i] = cpx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (int i=0; i<n3; ++i) f[i] = cpx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cpx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		compute(); return cost();
	}

	void Constellation::find () {
		std::vector<double> bw;
		for (auto z : b) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : w) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : f) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		bw.push_back(real(l)); bw.push_back(imag(l));

		double c = cost(bw), eps = sqrt(c), nc = c;
		while (eps>1e-100) {
			std::cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			for (int i=0; i<bw.size(); ++i) {
				bw[i] += eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { bw[i] -= eps; }
				bw[i] -= eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { bw[i] += eps; }
			}
			if (!flag) eps /= 2;
		}
		std::cerr << std::endl;
	}

	std::ostream & operator<< (std::ostream & os, const Constellation & C) {
		os << "Black vertices / zeros: " << std::endl;
		for (int i=0; i<C.b.size(); ++i) { os << "| " << C.bd[i] << "\t" << C.b[i]; for (int j=0; j<C.bd[i]; ++j) os << "\t" << C.Rs[j](C.b[i]); os << std::endl; }
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (int i=0; i<C.w.size(); ++i) { os << "| " << C.wd[i] << "\t" << C.w[i]; for (int j=0; j<C.wd[i]; ++j) os << "\t" << C.Rs[j](C.w[i]); os << std::endl; }
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (int i=0; i<C.f.size(); ++i) { os << "| " << C.fd[i] << "\t" << C.f[i]; for (int j=0; j<C.fd[i]; ++j) os << "\t" << C.Rs[j](C.f[i]); os << std::endl; }
		os << std::endl;
		os << C.Rs[0] << std::endl;
		return os;
	}
}
