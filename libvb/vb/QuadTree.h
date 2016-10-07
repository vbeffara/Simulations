#pragma once
#include <vb/Image.h>

namespace vb {
	struct QuadIndex { coo z; long d; };

	class QuadTree {
	public:
		QuadTree (coo UL, coo BR, int M);

		void insert (coo z);

		void nn (coo z, QuadIndex & qi) const;

		void paint (Image & img, coo ul, int w);

		int n;

	private:
		static std::vector <std::unique_ptr<QuadTree>> store;

		int index (coo z) const;
		int idist (coo z) const;
		int odist (coo z) const;

		void split ();

		coo ul,br,center,iul,ibr;
		long m, ch;
		std::vector <coo> pts;
	};
}
