#pragma once /// \file
#include <iostream>
#include <png++/png.hpp>
#include <vb/Bitmap.h>

namespace vb {
	class CoarseCell { public:
		explicit CoarseCell (int l = 1) : fill(0), LL(l*l) { }
		explicit operator Color() { return Grey(fill*255/LL); }

		int fill;              ///< The number of pixels with value 1 in the cell.
		std::vector<char> sub; ///< The actual contents of the cell, if not constant.
		int LL;                ///< The number of vertices in the cell.
	};

	class CoarseImage : public Bitmap<CoarseCell> { public:
		CoarseImage (int wd, int ht, int l) :
			Bitmap<CoarseCell> (1+(wd-1)/l, 1+(ht-1)/l, CoarseCell(l)),
			true_width(wd), true_height(ht), L(l), LL(l*l), z0(0,0) {}

		char at (coo z) const {
			z += z0; const CoarseCell & d = Bitmap<CoarseCell> :: at (coo(z.x/L,z.y/L));
			if (d.fill==0) return 0;
			if (d.fill==LL) return 1;
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

		bool contains (coo z) const { z += z0; return (z.x>=0) && (z.y>=0) && (z.x<true_width) && (z.y<true_height); }

		void output_fine (const std::string & fn) {
		    using row = png::packed_pixel_row <png::gray_pixel_1>;

		    png::image_info m_info (png::make_image_info<png::gray_pixel_1>());
	        m_info.set_width (true_width);
	        m_info.set_height (true_height);

		    std::ofstream stream (fn, std::ios::binary);
	        png::writer <std::ostream> wr (stream);
	        wr.set_image_info(m_info);
	        wr.write_info();

			row m_row (true_width);
	        for (int j=0; j<true_height; ++j) {
		        for (int i=0; i<true_width; ++i) m_row[i] = ! at(coo(i,j)-z0);
				wr.write_row(png::row_traits<row>::get_data(m_row));
	        }

	        wr.write_end_info();
		}

		int true_width;    ///< The true width of the image, in pixels.
		int true_height;   ///< The true height of the image, in pixels.
		int L;             ///< The size of a block, in pixels.
		int LL;            ///< The square of L
		coo z0;            ///< The coordinates of the origin (hides that of vb::Bitmap).
	};
} // namespace vb
