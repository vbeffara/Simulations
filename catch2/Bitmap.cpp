#include "catch2/catch_test_macros.hpp"
#include <vb/Bitmap.h>

using vb::Image, vb::Color;

TEST_CASE("vb::Image") {
    Image img("Testing Image", {256, 256});
    img.show();
    for (size_t i = 0; i < 256; ++i)
        for (size_t j = 0; j < 256; ++j) img.put({i, j}, Color(uint8_t(i), uint8_t(j), uint8_t((8 * (i + j)) % 256)));
    img.hide();
}
