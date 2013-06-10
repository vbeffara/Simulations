#pragma once /// @file
#include <vb/Bitmap.h>

namespace vb {
	class Image : public Bitmap<Color> {
	public:
		Image (int wd, int ht, const std::string &tit, Color d = NONE) : Bitmap<Color> (wd,ht,tit,d) {}
	};
}
