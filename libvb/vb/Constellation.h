#pragma once
#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/RationalFraction.h>
#include <vb/Spheroidal.h>
#include <iomanip>

namespace vb {
	class Constellation : public Image { public:
		Constellation (Hypermap M, Hub H, int n=3);

		void       	from_points	();
		void       	normalize  	();
		long double	belyi      	();

		long double	cost	()	const;
		long double	cost	(const std::vector<long double> & xy);
		void       	find	();

		virtual Color compute (coo c);

		std::vector<lcpx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;
		lcpx                 	l=1.0;

		std::vector<RationalFraction<lcpx>>	Rs;
	};

	std::ostream & operator<< (std::ostream & os, const Constellation & C);
}
