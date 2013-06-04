#pragma once /// @file
#include <complex>
#include <string>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace vb {
	typedef std::complex<double> cpx; const cpx I(0,1);

	typedef std::complex<int> coo;
	const int dx[8] = {1,0,-1,0,1,-1,-1,1};
	const int dy[8] = {0,1,0,-1,1,-1,1,-1};
	const coo dz[8] = {coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1)};

	class fmt : public boost::format {
	public:
		fmt (const std::string &s) : boost::format(s) { }
		operator std::string() const { return boost::format::str(); }
	};

	#define foreach BOOST_FOREACH
};
