#pragma once
#include <vb/coo.h>

namespace vb {
	class Cluster { public:
		Cluster (coo ul_ = {0,0}, int w_ = bs) : ul(ul_), w(w_), np(0) {};

		void ensure_sub ();
		void grow ();

		bool fits (coo z) const;
		bool at (coo z) const;

		void put (coo z, bool b, bool quick = false);
		void insert (coo z);
		void remove (coo z);

		void validate ();
		void dump (std::string pre = "");

		coo ul;
		long w,np;
		std::vector <bool> tile;
		std::vector <Cluster> sub;
		static int bs;
	};
}
