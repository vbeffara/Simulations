#pragma once /// @file
#include <iostream>
#include <vector>

namespace vb {
	using Cycles = std::vector<std::vector<unsigned long>>;

	class Passport : public std::vector<std::pair<unsigned long,unsigned long>> {};

	class Permutation : public std::vector<unsigned long> {
	public:
		Permutation (unsigned long n = 0)         	: std::vector<unsigned long> (n)           	{ for (unsigned long i=0; i<n; ++i) at(i)=i; }
		Permutation (std::vector<unsigned long> s)	: std::vector<unsigned long> (std::move(s))	{ }
		Permutation (Cycles & c);

		bool is_identity () const;

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $this * o = o \circ this$.
		Permutation conjugate	(const Permutation & s) const;

		Cycles                    	cycles ()   	const;
		std::vector<unsigned long>	signature ()	const;
		Passport                  	passport () 	const;
	};

	Permutation Transposition (unsigned long n, unsigned long i, unsigned long j);

	bool connected (const Permutation & s, const Permutation & a);

	std::ostream & operator<< (std::ostream &os, const Passport &P);
	std::ostream & operator<< (std::ostream &os, const Permutation &P);
}
