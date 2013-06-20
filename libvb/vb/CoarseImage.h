#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
	class CoarseCell { public:
		CoarseCell (int l) : fill(0), LL(l*l) { }
		operator Color() { return fill*255/LL; }

		int fill;              ///< The number of pixels with value 1 in the cell.
		std::vector<char> sub; ///< The actual contents of the cell, if not constant.
		int LL;                ///< The number of vertices in the cell.
	};

	class CoarseImage : public Bitmap<CoarseCell> { public:
		CoarseImage (int wd, int ht, const std::string &title, int l) :
			Bitmap<CoarseCell> (1+(wd-1)/l,1+(ht-1)/l,title,l),
			true_width(wd), true_height(ht), L(l), LL(l*l), z0(0) {}

		char at (coo z) const {
			z += z0; const CoarseCell & d = Bitmap<CoarseCell> :: at (coo(z.x/L,z.y/L));
			if (d.fill==0) return 0; if (d.fill==LL) return 1;
			return d.sub[(z.x%L) + L*(z.y%L)];
		}

		void put (coo z, int c) {
			step(); z += z0; CoarseCell & d = Bitmap<CoarseCell> :: at (coo(z.x/L,z.y/L));
			if (d.fill == c*LL)	return;
			if (d.fill == (1-c)*LL) d.sub.resize (LL,1-c);
			int sub_xy = (z.x%L) + L * (z.y%L);
			if (d.sub[sub_xy] != c) { d.sub[sub_xy] = c; d.fill += 2*c-1; }
			if ((d.fill==0)||(d.fill==LL)) std::vector<char>().swap(d.sub);
		}

		int LL;
		int true_width;    ///< The true width of the image, in pixels.
		int true_height;   ///< The true height of the image, in pixels.
		int L;             ///< The size of a block, in pixels.
		coo z0;            ///< The coordinates of the origin (hides that of vb::Bitmap).
	};
}
