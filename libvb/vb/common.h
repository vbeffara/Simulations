#pragma once /// @file
#include <complex>
#include <string>
#include <boost/format.hpp>

namespace vb {
	typedef std::complex<double> cpx; const cpx I(0,1);

	const int dx[8] = {1,0,-1,0,1,-1,-1,1};
	const int dy[8] = {0,1,0,-1,1,-1,1,-1};

	class fmt : public boost::format {
	public:
		fmt (const std::string &s) : boost::format(s) { }
		operator std::string() const { return boost::format::str(); }
	};
};
