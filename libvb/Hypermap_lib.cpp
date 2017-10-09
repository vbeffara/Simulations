#include <vb/Hypermap_lib.h>
#include <fstream>

namespace vb {
	HLib::HLib () {
		using msh = map<std::string,Hypermap>;
		msh::operator= (YAML::LoadFile("hypermaps.yaml").as<msh>());

		{ Hypermap M = at("house"); M.dessin(); emplace ("house_d", M); }
		{ Hypermap M = at("house_d"); M.dessin(); emplace ("house_dd", M); }
	}

	Hypermap H_artem (long n) {
		std::vector<int> sigma(6*n), alpha(6*n), phi(6*n);
		for (long i=0; i<n; ++i) {
			sigma[i]=2*n+i; sigma[n+i]=5*n+i+1; sigma[2*n+i]=n+i-1; sigma[3*n+i]=3*n+i+1; sigma[4*n+i]=4*n+i-1; sigma[5*n+i]=i;
			alpha[i]=n+i; alpha[n+i]=i; alpha[2*n+i]=3*n+i; alpha[3*n+i]=2*n+i; alpha[4*n+i]=5*n+i; alpha[5*n+i]=4*n+i;
			phi[i]=2*n+i+1; phi[n+i]=5*n+i; phi[2*n+i]=3*n+i-1; phi[3*n+i]=i; phi[4*n+i]=n+i-1; phi[5*n+i]=4*n+i+1;
		}
		sigma[2*n-1]=3*n; sigma[2*n]=5*n-1; sigma[4*n-1]=5*n; sigma[4*n]=2*n-1;
		phi[n-1]=4*n; phi[2*n]=2*n-1; phi[3*n]=0; phi[4*n]=4*n-1; phi[6*n-1]=2*n;
		return Hypermap (sigma, alpha, phi);
	}

	Hypermap H_genus0 (long n) {
		std::vector<int> sigma(6*n), alpha(6*n), phi(6*n);
		for (long i=0; i<n; ++i) { long j = (i+1)%n, k = (i+n-1)%n;
			sigma[i]=j; sigma[n+i]=3*n+k; sigma[2*n+i]=n+i; sigma[3*n+i]=4*n+j; sigma[4*n+i]=2*n+i; sigma[5*n+i]=5*n+k;
			alpha[i]=n+i; alpha[n+i]=i; alpha[2*n+i]=3*n+i; alpha[3*n+i]=2*n+i; alpha[4*n+i]=5*n+i; alpha[5*n+i]=4*n+i;
			phi[i]=2*n+i; phi[n+i]=k; phi[2*n+i]=n+j; phi[3*n+i]=4*n+i; phi[4*n+i]=5*n+j; phi[5*n+i]=3*n+k;
		}
		return Hypermap (sigma, alpha, phi);
	}

	Hypermap H_genus1 (long n) {
		std::vector<int> sigma(6*n), alpha(6*n), phi(6*n);
		for (long i=0; i<n; ++i) { long j = (i+1)%n, k = (i+n-1)%n;
			sigma[6*i]=6*i+1; sigma[6*i+1]=6*i+2; sigma[6*i+2]=6*i+3; sigma[6*i+3]=6*i+4; sigma[6*i+4]=6*i+5; sigma[6*i+5]=6*i;
			alpha[6*i]=6*j+3; alpha[6*i+1]=6*j+4; alpha[6*i+2]=6*i+5; alpha[6*i+3]=6*k; alpha[6*i+4]=6*k+1; alpha[6*i+5]=6*i+2;
			phi[6*i]=6*j+2; phi[6*i+1]=6*j+3; phi[6*i+2]=6*i+4; phi[6*i+3]=6*k+5; phi[6*i+4]=6*k; phi[6*i+5]=6*i+1;
		}
		return Hypermap (sigma, alpha, phi);
	}
}
