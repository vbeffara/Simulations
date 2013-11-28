#pragma once /// \file
#define BOOST_DISABLE_ASSERTS 1
#include <boost/random.hpp>

namespace vb {
	class PRNG : public boost::mt19937 {
	public:
		PRNG (unsigned long s=0) { seed (s?s:time(0)); }

		bool  	bernoulli   	(double p=.5)               	{ return (boost::bernoulli_distribution<>  	(p))        	(*this); }
		int   	uniform_int 	(int mmax)                  	{ return (boost::uniform_int<>             	(0, mmax-1))	(*this); }
		int   	geometric   	(double p=.5)               	{ return (boost::geometric_distribution<>  	(p))        	(*this); }
		int   	poisson     	(double lambda=1)           	{ return (boost::poisson_distribution<>    	(lambda))   	(*this); }
		double	uniform_real	(double min=0, double max=1)	{ return (boost::uniform_real<>            	(min, max)) 	(*this); }
		double	exponential 	(double lambda=1)           	{ return (boost::exponential_distribution<>	(lambda))   	(*this); }
		double	gaussian    	(double m=0, double sigma=1)	{ return (boost::normal_distribution<>     	(m,sigma))  	(*this); }

		std::string	state ()                    	{ std::ostringstream os;    	os << (*this); return os.str();	}
		void       	state (const std::string &s)	{ std::istringstream is (s);	is >> (*this);                 	}
	};

	extern PRNG prng;
}
