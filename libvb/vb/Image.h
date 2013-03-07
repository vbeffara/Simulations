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

			void putpoint_safe (int x, int y, Color c, int dt=1) __attribute__ ((deprecated)) {
				if ( (x>=0) && (y>=0) && (x<w()) && (y<h()) ) {
					if (dt) put (coo(x,y),c);
					else at(x,y) = c;
				}
			}

			virtual Color compute (coo z) { return 0; }

			Color lazy (coo z);

			void tessel (int xmin, int ymin, int xmax, int ymax);

			void fill (int x, int y, Color c, unsigned char adj = 85) __attribute__ ((deprecated));
	};
}

#endif
