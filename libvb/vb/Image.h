/// @file
/// Definitions for the vb::Image class

#pragma once
#include <vb/Bitmap.h>

namespace vb {
	class Image : public Bitmap<Color> {
		public:
			Image (int wd, int ht, const std::string &tit);

			void putpoint (int x, int y, Color c) __attribute__ ((deprecated)) {
				put (coo(x,y),c);
			}

			virtual Color compute (coo z) { return 0; }

			Color lazy (coo z);

			void tessel (int xmin, int ymin, int xmax, int ymax);

			void fill (coo z, Color c, int adj = 4);
	};
}
