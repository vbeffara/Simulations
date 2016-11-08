#pragma once
#include <vb/Permutation.h>

namespace vb {
	class Pairings_Iterator {
	public:
		Pairings_Iterator (int n, int i, bool d);

		bool         	operator!=	(const Pairings_Iterator &o)	const;
		void         	operator++	();
		Permutation &	operator* 	();

	private:
		void next ();

		std::vector<std::vector<unsigned>> todo_c, todo_p;
		Permutation current;
		unsigned n,i;
	};

	class Pairings {
	public:
		Pairings (int n);

		unsigned size() const;

		Pairings_Iterator	begin () const;
		Pairings_Iterator	end	() const;

		Permutation rand ();

	private:
		unsigned n;
	};
}
