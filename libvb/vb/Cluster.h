#pragma once
#include <vb/Image.h>

namespace vb {
	class Cluster { public:
		Cluster (coo ul_ = {-bs/2,-bs/2}, int w_ = bs) : ul(ul_), w(w_), np(0) {};

		void ensure_sub ();
		void grow ();

		bool fits (coo z) const;
		bool at (coo z) const;

		void put (coo z, bool b, bool quick = false);
		void insert (coo z);
		void remove (coo z);

		void validate ();
		void paint (Image & I, coo ul = 0, coo br = 0);
		void dump (std::string pre = "");

		coo ul;
		int w;
		long np;
		std::vector <bool> tile;
		std::vector <Cluster> sub;
		static int bs;
	};
}
