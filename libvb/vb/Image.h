/// @file
/// Definitions for the vb::Image class

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <vb/Bitmap.h>

namespace vb {
	class Image : public Bitmap<Color> {
		public:
			Image (int wd, int ht, const std::string &tit);

			void putpoint (int x, int y, Color c, int dt=1) __attribute__ ((deprecated)) {
				at(x,y) = c;
				if (dt) step();
			}

			virtual Color compute (coo z) { return 0; }

			Color lazy (coo z);

			void tessel (int xmin, int ymin, int xmax, int ymax);

			void fill (coo z, Color c, int adj = 4);
	};
}

#endif
