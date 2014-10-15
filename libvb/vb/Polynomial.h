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
	};

	std::ostream & operator<< (std::ostream & os, const Polynomial & P);
}
