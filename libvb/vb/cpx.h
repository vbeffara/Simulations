#pragma once /// @file

#include <complex>

namespace vb {
	typedef std::complex<double> cpx;
	const cpx I(0,1);
    const cpx dzc[4] = { cpx(1,0), cpx(0,1), cpx(-1,0), cpx(0,-1) };
};
