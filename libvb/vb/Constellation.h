#pragma once
#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/RationalFraction.h>
#include <vb/Spheroidal.h>
#include <iomanip>

namespace vb {
	class Constellation : public Image { public:
		Constellation (Hypermap M, Hub H, int n=3);

		void  	from_points	();
		void  	normalize  	();
		double	belyi      	();

		double	cost	()	const;
		double	cost	(const std::vector<double> & xy);
		void  	find	();

		virtual Color compute (coo c);

		std::vector<cpx>     	b,w,f;
		std::vector<unsigned>	bd,wd,fd;
		cpx                  	l=1.0;

		std::vector<RationalFraction<cpx>>	Rs;
	};

	std::ostream & operator<< (std::ostream & os, const Constellation & C);
}
