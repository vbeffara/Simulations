#pragma once
#include <vb/Hub.h>
#include <vb/RationalFraction.h>
#include <vb/Spheroidal.h>
#include <iomanip>

namespace vb {
	class Constellation { public:
		Constellation (Hypermap M, Hub H, int n=3);

		void  	compute  	();
		void  	normalize	();
		double	belyi    	();

		double	cost	()	const;
		double	cost	(const std::vector<double> & xy);
		void  	find	();

		std::vector<cpx>     	b,w,f;
		std::vector<unsigned>	bd,wd,fd;
		cpx                  	l=1.0;

		std::vector<RationalFraction>	Rs;
	};

	std::ostream & operator<< (std::ostream & os, const Constellation & C);
}
