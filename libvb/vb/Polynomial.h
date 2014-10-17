#pragma once
#include <vb/cpx.h>
#include <iostream>
#include <vector>

namespace vb {
	class Polynomial : public std::vector<cpx> {
	public:
		Polynomial (const std::vector<cpx> V);

		unsigned  	degree    	() const;
		Polynomial	derivative	() const;

		cpx operator() (cpx z) const;

		void add_root (cpx l);

		std::string v = "z";

		double precision = 0.0;
	};

	Polynomial operator- (const Polynomial & P, const Polynomial & Q);
	Polynomial operator* (const Polynomial & P, const Polynomial & Q);

	std::ostream & operator<< (std::ostream & os, const Polynomial & P);
}
