#pragma once /// @file Definitions for the vb::Image class (just an wrapper for Bitmap<Color>)
#include <vb/Bitmap.h>

namespace vb {
	class Image : public Bitmap<Color> {
	public:
		Image (int wd, int ht, const std::string &tit, Color d = NONE) : Bitmap<Color> (wd,ht,tit,d) {}
	};
}
