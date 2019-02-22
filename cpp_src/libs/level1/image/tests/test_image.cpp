#include "core/core.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char *argv[]) {
  return Catch::Session().run(argc, argv);
}

#include "image/image.h"
#include "image/format_cracker.h"

TEST_CASE("Image create/destroy (C)", "[Image]") {
  Image_Header_t *image0 = Image_Create2D(256, 256, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0);
  REQUIRE(image0->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0->width == 256);
  REQUIRE(image0->height == 256);
  REQUIRE(image0->depth == 1);
  REQUIRE(image0->slices == 1);
  REQUIRE(image0->dataSize == (256 * 256 * Image_Format_BitWidth(image0->format)) / 8);

  Image_Header_t *image1 = Image_Create2DNoClear(64, 128, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1);
  REQUIRE(image1->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1->width == 64);
  REQUIRE(image1->height == 128);
  REQUIRE(image1->depth == 1);
  REQUIRE(image1->slices == 1);
  REQUIRE(image1->dataSize == (64 * 128 * Image_Format_BitWidth(image1->format)) / 8);

  Image_Destroy(image1);
  Image_Destroy(image0);
}

TEST_CASE("Image create clear (C)", "[Image]") {
  Image_Header_t *image = Image_Create2D(256, 256, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image);
  REQUIRE(image->dataSize == (256 * 256 * Image_Format_BitWidth(image->format)) / 8);

  uint64_t const pixelSize = Image_Format_BitWidth(image->format);
  REQUIRE(pixelSize / 8 == 4);

  // check the image is cleared
  uint64_t accum = 0;
  uint8_t *ptr = (uint8_t *) Image_RawDataPtr(image);
  for (uint32_t y = 0; y < image->height; ++y) {
    for (uint32_t x = 0; x < image->width; ++x) {
      size_t index = Image_CalculateIndex(image, x, y, 0, 0);
      accum += *(ptr + ((index * pixelSize) / 8));
    }
  }
  REQUIRE(accum == 0);

  Image_Destroy(image);
}