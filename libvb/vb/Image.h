#pragma once /// @file
#include <vb/Bitmap.h>

namespace vb {
    using Image = Bitmap<Color>;

    #ifdef UNIT_TESTS
    TEST_CASE ("vb::Image") {
        Image img (256,256);
        img.show();
        for (int i=0; i<256; ++i)
            for (int j=0; j<256; ++j)
                img.put (coo(i,j), Color(i,j,(8*(i+j))%256));
        img.hide();
    }
    #endif
}
