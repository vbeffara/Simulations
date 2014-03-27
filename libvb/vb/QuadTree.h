#pragma once

#include <vb/coo.h>
#include <vector>

namespace vb {
	struct QuadIndex { coo z; int d; };

	class QuadTree {
	public:
		QuadTree (coo UL, coo BR, int M);
		~QuadTree ();

		void insert (coo z);

		void nn (coo z, QuadIndex & qi) const;

		int n;

	private:
		int index (coo z) const;
		int idist (coo z) const;
		int odist (coo z) const;

		void split ();

		coo ul,br,center,iul,ibr;
		std::vector <coo> pts;
		QuadTree * children[4];
		int m;
		bool leaf;
	};
}
