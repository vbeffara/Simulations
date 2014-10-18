#pragma once
#include <vb/Polynomial.h>

namespace vb {
	class RationalFraction {
	public:
		RationalFraction (Polynomial P_ = {{1}}, Polynomial Q_ = {{1}});

		int             	degree    	() const;
		RationalFraction	derivative	() const;

		cpx operator() (cpx z) const;

		void add_root (cpx z);
		void add_pole (cpx z);

		Polynomial P,Q;
	};

	std::ostream & operator<< (std::ostream & os, const RationalFraction & R);
}
