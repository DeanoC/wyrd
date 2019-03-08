#include "core/core.h"
#include "catch/catch.hpp"

#include "image/pixel.h"

TEST_CASE("Pixel clamp (C)", "[Image]") {
  static Image_Pixel_t pixel = {1.0, 1.0, 1.0, 1.0f};
  static double const minTest[4] = {0.1, 1.1, 1.3, 0.0};
  static double const maxTest[4] = {1.1, 0.5, 1.2, 1.0};

  Image_PixelClamp(&pixel, minTest, maxTest);
  REQUIRE(pixel.r == Approx(1.0));
  REQUIRE(pixel.g == Approx(0.5));
  REQUIRE(pixel.b == Approx(1.2));
  REQUIRE(pixel.a == Approx(1.0));

}
