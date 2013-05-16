#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
	class CoarseCell {
		public:
			CoarseCell (int l) : fill(0), LL(l*l) { }

			operator Color() { return fill*255/LL; }

			int fill;              ///< The number of pixels with value 1 in the cell.
			std::vector<char> sub; ///< The actual contents of the cell, if not constant.

		private:
			int LL;                ///< The number of vertices in the cell.
	};

	class CoarseImage : public Bitmap<CoarseCell> {
		public:
			int true_width;    ///< The true width of the image, in pixels.
			int true_height;   ///< The true height of the image, in pixels.
			int L;             ///< The size of a block, in pixels.
			coo z0;            ///< The coordinates of the origin (hides that of vb::Bitmap).

			CoarseImage (int wd, int ht, const std::string &tit, int l);

			void put       	(coo z, int c);
			char at        	(coo z) const;
			char operator()	(coo z) const { return at(z); }

		private:
			int LL;
	};
}
