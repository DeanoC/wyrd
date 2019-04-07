#include "core/core.h"
#include "cmdlineshell/cmdlineshell.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int Main(int argc, char const *argv[]) {
  return Catch::Session().run(argc, (char**)argv);
}

#include "image/image.h"
#include "image/format_cracker.h"
#include "image/create.h"

TEST_CASE("Image create/destroy 1D (C)", "[Image]") {
  Image_ImageHeader *image0 = Image_Create1D(256, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0);
  REQUIRE(image0->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0->width == 256);
  REQUIRE(image0->height == 1);
  REQUIRE(image0->depth == 1);
  REQUIRE(image0->slices == 1);
  REQUIRE(image0->dataSize == (256 * Image_Format_BitWidth(image0->format)) / 8);
  REQUIRE(image0->nextImage == nullptr);
  REQUIRE(image0->nextType == Image_IT_None);
  REQUIRE(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader *image1 = Image_Create1DNoClear(64, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1);
  REQUIRE(image1->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1->width == 64);
  REQUIRE(image1->height == 1);
  REQUIRE(image1->depth == 1);
  REQUIRE(image1->slices == 1);
  REQUIRE(image1->dataSize == (64 * Image_Format_BitWidth(image1->format)) / 8);
  REQUIRE(image1->nextImage == nullptr);
  REQUIRE(image1->nextType == Image_IT_None);
  REQUIRE(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader *image2 = Image_Create1DArray(256, 20, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2);
  REQUIRE(image2->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2->width == 256);
  REQUIRE(image2->height == 1);
  REQUIRE(image2->depth == 1);
  REQUIRE(image2->slices == 20);
  REQUIRE(image2->dataSize == (256 * 20 * Image_Format_BitWidth(image2->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader *image3 = Image_Create1DArrayNoClear(64, 100, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3);
  REQUIRE(image3->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3->width == 64);
  REQUIRE(image3->height == 1);
  REQUIRE(image3->depth == 1);
  REQUIRE(image3->slices == 100);
  REQUIRE(image3->dataSize == (64 * 100 * Image_Format_BitWidth(image3->format)) / 8);
  REQUIRE(image3->nextImage == nullptr);
  REQUIRE(image3->nextType == Image_IT_None);
  REQUIRE(image3->flags == 0);
  Image_Destroy(image3);

}

TEST_CASE("Image create/destroy 2D (C)", "[Image]") {
  Image_ImageHeader *image0 = Image_Create2D(256, 256, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0);
  REQUIRE(image0->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0->width == 256);
  REQUIRE(image0->height == 256);
  REQUIRE(image0->depth == 1);
  REQUIRE(image0->slices == 1);
  REQUIRE(image0->dataSize == (256 * 256 * Image_Format_BitWidth(image0->format)) / 8);
  REQUIRE(image0->nextImage == nullptr);
  REQUIRE(image0->nextType == Image_IT_None);
  REQUIRE(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader *image1 = Image_Create2DNoClear(64, 128, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1);
  REQUIRE(image1->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1->width == 64);
  REQUIRE(image1->height == 128);
  REQUIRE(image1->depth == 1);
  REQUIRE(image1->slices == 1);
  REQUIRE(image1->dataSize == (64 * 128 * Image_Format_BitWidth(image1->format)) / 8);
  REQUIRE(image1->nextImage == nullptr);
  REQUIRE(image1->nextType == Image_IT_None);
  REQUIRE(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader *image2 = Image_Create2DArray(256, 256, 20, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2);
  REQUIRE(image2->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2->width == 256);
  REQUIRE(image2->height == 256);
  REQUIRE(image2->depth == 1);
  REQUIRE(image2->slices == 20);
  REQUIRE(image2->dataSize == (256 * 256 * 20 * Image_Format_BitWidth(image2->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader *image3 = Image_Create2DArrayNoClear(64, 32, 100, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3);
  REQUIRE(image3->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3->width == 64);
  REQUIRE(image3->height == 32);
  REQUIRE(image3->depth == 1);
  REQUIRE(image3->slices == 100);
  REQUIRE(image3->dataSize == (64 * 32 * 100 * Image_Format_BitWidth(image3->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image3->flags == 0);
  Image_Destroy(image3);
}

TEST_CASE("Image create/destroy 3D (C)", "[Image]") {
  Image_ImageHeader *image0 = Image_Create3D(256, 256, 16, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0);
  REQUIRE(image0->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0->width == 256);
  REQUIRE(image0->height == 256);
  REQUIRE(image0->depth == 16);
  REQUIRE(image0->slices == 1);
  REQUIRE(image0->dataSize == (256 * 256 * 16 * Image_Format_BitWidth(image0->format)) / 8);
  REQUIRE(image0->nextImage == nullptr);
  REQUIRE(image0->nextType == Image_IT_None);
  REQUIRE(image0->flags == 0);
  Image_Destroy(image0);

  Image_ImageHeader *image1 = Image_Create3DNoClear(64, 128, 256, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1);
  REQUIRE(image1->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1->width == 64);
  REQUIRE(image1->height == 128);
  REQUIRE(image1->depth == 256);
  REQUIRE(image1->slices == 1);
  REQUIRE(image1->dataSize == (64 * 128 * 256 * Image_Format_BitWidth(image1->format)) / 8);
  REQUIRE(image1->nextImage == nullptr);
  REQUIRE(image1->nextType == Image_IT_None);
  REQUIRE(image1->flags == 0);
  Image_Destroy(image1);

  Image_ImageHeader *image2 = Image_Create3DArray(256, 256, 256, 20, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2);
  REQUIRE(image2->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2->width == 256);
  REQUIRE(image2->height == 256);
  REQUIRE(image2->depth == 256);
  REQUIRE(image2->slices == 20);
  REQUIRE(image2->dataSize == (256 * 256 * 256 * 20 * Image_Format_BitWidth(image2->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image2->flags == 0);
  Image_Destroy(image2);

  Image_ImageHeader *image3 = Image_Create3DArrayNoClear(64, 32, 16, 100, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3);
  REQUIRE(image3->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3->width == 64);
  REQUIRE(image3->height == 32);
  REQUIRE(image3->depth == 16);
  REQUIRE(image3->slices == 100);
  REQUIRE(image3->dataSize == (64 * 32 * 16 * 100 * Image_Format_BitWidth(image3->format)) / 8);
  REQUIRE(image3->nextImage == nullptr);
  REQUIRE(image3->nextType == Image_IT_None);
  REQUIRE(image3->flags == 0);
  Image_Destroy(image3);

  // HUGE test
  Image_ImageHeader *image4 = Image_Create3DArrayNoClear(1024, 1024, 16, 10, Image_Format_R64G64B64A64_SFLOAT);
  REQUIRE(image4);
  REQUIRE(image4->format == Image_Format_R64G64B64A64_SFLOAT);
  REQUIRE(image4->width == 1024);
  REQUIRE(image4->height == 1024);
  REQUIRE(image4->depth == 16);
  REQUIRE(image4->slices == 10);
  REQUIRE(image4->dataSize == (1024 * 1024 * 16 * 100 * Image_Format_BitWidth(image4->format)) / 8);
  REQUIRE(image4->nextImage == nullptr);
  REQUIRE(image4->nextType == Image_IT_None);
  REQUIRE(image4->flags == 0);
  Image_Destroy(image4);
}

TEST_CASE("Image create/destroy 2D Cubemap (C)", "[Image]") {
  Image_ImageHeader *image0 = Image_CreateCubemap(256, 256, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0);
  REQUIRE(image0->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image0->width == 256);
  REQUIRE(image0->height == 256);
  REQUIRE(image0->depth == 1);
  REQUIRE(image0->slices == 6);
  REQUIRE(image0->dataSize == (256 * 256 * 6 * Image_Format_BitWidth(image0->format)) / 8);
  REQUIRE(image0->nextImage == nullptr);
  REQUIRE(image0->nextType == Image_IT_None);
  REQUIRE(image0->flags == Image_Flag_Cubemap);
  Image_Destroy(image0);

  Image_ImageHeader *image1 = Image_CreateCubemapNoClear(64, 128, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1);
  REQUIRE(image1->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image1->width == 64);
  REQUIRE(image1->height == 128);
  REQUIRE(image1->depth == 1);
  REQUIRE(image1->slices == 6);
  REQUIRE(image1->dataSize == (64 * 128 * 6 * Image_Format_BitWidth(image1->format)) / 8);
  REQUIRE(image1->nextImage == nullptr);
  REQUIRE(image1->nextType == Image_IT_None);
  REQUIRE(image1->flags == Image_Flag_Cubemap);
  Image_Destroy(image1);

  Image_ImageHeader *image2 = Image_CreateCubemapArray(256, 256, 20, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2);
  REQUIRE(image2->format == Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image2->width == 256);
  REQUIRE(image2->height == 256);
  REQUIRE(image2->depth == 1);
  REQUIRE(image2->slices == 20 * 6);
  REQUIRE(image2->dataSize == (256 * 256 * 20 * 6 * Image_Format_BitWidth(image2->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image2->flags == Image_Flag_Cubemap);
  Image_Destroy(image2);

  Image_ImageHeader *image3 = Image_CreateCubemapArrayNoClear(64, 32, 100, Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3);
  REQUIRE(image3->format == Image_Format_R5G5B5A1_UNORM_PACK16);
  REQUIRE(image3->width == 64);
  REQUIRE(image3->height == 32);
  REQUIRE(image3->depth == 1);
  REQUIRE(image3->slices == 100 * 6);
  REQUIRE(image3->dataSize == (64 * 32 * 100 * 6 * Image_Format_BitWidth(image3->format)) / 8);
  REQUIRE(image2->nextImage == nullptr);
  REQUIRE(image2->nextType == Image_IT_None);
  REQUIRE(image3->flags == Image_Flag_Cubemap);
  Image_Destroy(image3);
}

TEST_CASE("Calculate Index (C)", "[Image]") {
  Image_ImageHeader *image = Image_Create3DArray(17, 17, 17, 17, Image_Format_A8B8G8R8_UNORM_PACK32);
  REQUIRE(image);
  uint64_t const pixelSize = Image_Format_BitWidth(image->format);
  REQUIRE(image->dataSize == (17 * 17 * 17 * 17 * pixelSize) / 8);

  REQUIRE(pixelSize / 8 == 4);

  // check the image is cleared

  uint8_t *ptr = (uint8_t *) Image_RawDataPtr(image);
  REQUIRE(ptr);
  REQUIRE(ptr == (uint8_t *) (image + 1));

  uint64_t accum0 = 0;
  for (auto i = 0u; i < image->dataSize; ++i) {
    accum0 += ptr[i];
  }
  REQUIRE(accum0 == 0);

  for (auto i = 0u; i < image->dataSize; ++i) {
    ptr[i] = (uint8_t) (i & 0xFFu);
  }

  for (uint32_t s = 0; s < image->slices; ++s) {
    for (uint32_t d = 0; d < image->depth; ++d) {
      for (uint32_t y = 0; y < image->height; ++y) {
        for (uint32_t x = 0; x < image->width; ++x) {
          size_t index = Image_CalculateIndex(image, x, y, d, s);
          REQUIRE(ptr[index] == (uint8_t) (index & 0xFFu));
        }
      }
    }
  }

  Image_Destroy(image);
}

void ImageTester(uint32_t w_, uint32_t h_, uint32_t d_, uint32_t s_, enum Image_Format fmt_, bool doLog_) {
  using namespace Catch::literals;
  if (fmt_ == Image_Format_UNDEFINED) { return; }

  auto img = Image_Create(w_, h_, d_, s_, fmt_);
  if (doLog_) {
    LOGINFOF("Testing %s", Image_Format_Name(fmt_));
  }

  REQUIRE(img);
  REQUIRE(img->width == w_);
  REQUIRE(img->height == h_);
  REQUIRE(img->depth == d_);
  REQUIRE(img->slices == s_);
  REQUIRE(img->format == fmt_);
  uint64_t const pixelSize = Image_Format_BitWidth(img->format);
  REQUIRE(img->dataSize == (w_ * h_ * d_ * s_ * pixelSize) / 8);

  // check the image is cleared
  uint8_t *ptr = (uint8_t *) Image_RawDataPtr(img);
  REQUIRE(ptr);
  REQUIRE(ptr == (uint8_t *) (img + 1));
  for (auto i = 0u; i < img->dataSize; ++i) {
    REQUIRE(ptr[i] == 0);
  }

  double const mins[4] = {
      Image_Format_Min(img->format, Image_Red),
      Image_Format_Min(img->format, Image_Green),
      Image_Format_Min(img->format, Image_Blue),
      Image_Format_Min(img->format, Image_Alpha)
  };
  double const maxs[4] = {
      Image_Format_Max(img->format, Image_Red),
      Image_Format_Max(img->format, Image_Green),
      Image_Format_Max(img->format, Image_Blue),
      Image_Format_Max(img->format, Image_Alpha)
  };

  for (auto s = 0u; s < img->slices; ++s) {
    for (auto z = 0u; z < img->depth; ++z) {
      for (auto y = 0u; y < img->height; ++y) {
        for (auto x = 0u; x < img->width; ++x) {
          Image_PixelD pixel = {double(x), double(y), double(z), double(s)};
          if (Image_Format_IsSigned(img->format)) {
            pixel.r = mins[0] + pixel.r;
            pixel.g = mins[1] + pixel.g;
            pixel.b = mins[2] + pixel.b;
            pixel.a = mins[3] + pixel.a;
          }

          Image_PixelClamp(&pixel, mins, maxs);

          if (Image_Format_IsNormalised(img->format)) {
            pixel.r = pixel.r / maxs[0];
            pixel.g = pixel.g / maxs[1];
            pixel.b = pixel.b / maxs[2];
            pixel.a = pixel.a / maxs[3];
          }
          if (doLog_) {
            LOGINFOF("set<RGBA> %d %d %d %d = %d %d %d %d ",
                     x, y, z, s,
                     pixel.r, pixel.g, pixel.b, pixel.a);
          }
          size_t const index = Image_CalculateIndex(img, x, y, z, s);
          Image_SetPixelAt(img, &pixel, index);
        }
      }
    }
  }
  for (auto s = 0u; s < img->slices; ++s) {
    for (auto z = 0u; z < img->depth; ++z) {
      for (auto y = 0u; y < img->height; ++y) {
        for (auto x = 0u; x < img->width; ++x) {
          Image_PixelD pixel;

          size_t const index = Image_CalculateIndex(img, x, y, z, s);
          Image_GetPixelAt(img, &pixel, index);
          if (doLog_) {
            LOGINFOF("get<RGBA> %d %d %d %d = %d %d %d %d ",
                     x, y, z, s,
                     pixel.r, pixel.g, pixel.b, pixel.a);
          }
          Image_PixelD expected = {double(x), double(y), double(z), double(s)};

          if (Image_Format_IsSigned(img->format)) {
            expected.r = mins[0] + expected.r;
            expected.g = mins[1] + expected.g;
            expected.b = mins[2] + expected.b;
            expected.a = mins[3] + expected.a;
          }
          Image_PixelClamp(&expected, mins, maxs);

          if (Image_Format_IsNormalised(img->format)) {
            expected.r = expected.r / maxs[0];
            expected.g = expected.g / maxs[1];
            expected.b = expected.b / maxs[2];
            expected.a = expected.a / maxs[3];
          }

          // delibrate fallthrough
          switch (Image_Format_ChannelCount(fmt_)) {
            case 4: REQUIRE(expected.a == Approx(pixel.a));
            case 3: REQUIRE(expected.b == Approx(pixel.b));
            case 2: REQUIRE(expected.g == Approx(pixel.g));
            case 1: REQUIRE(expected.r == Approx(pixel.r));
              break;
            default:REQUIRE(Image_Format_ChannelCount(fmt_) <= 4);
          }
        }
      }
    }
  }
  Image_Destroy(img);
}

void ImageTesterFiltered(uint32_t w_, uint32_t h_, uint32_t d_, uint32_t s_, enum Image_Format fmt_, bool doLog_) {
  // Filter the known failures for the generic test runs
  // this formats will emit failures due to 'lossy'ness over the range

  // skip block compressed testing the pixel read/write tests
  if (Image_Format_IsCompressed(fmt_)) { return; }

  // sRGB is a form of lossy compression need to rethink tests in this case
  if (Image_Format_IsSRGB(fmt_)) { return; }

  // small floats are also lossy over the ranges we test against
  if (Image_Format_IsFloat(fmt_) &&
      Image_Format_ChannelBitWidth(fmt_, Image_Red) <= 16) {
    return;
  }

  // mixed normalised / unnormalised format which the tester doesn't handle yet
  if (fmt_ == Image_Format_D24_UNORM_S8_UINT) { return; }
  if (fmt_ == Image_Format_D16_UNORM_S8_UINT) { return; }

  // low precision normalised alpha not working yet
  if (fmt_ == Image_Format_A1R5G5B5_UNORM_PACK16) { return; }
  if (fmt_ == Image_Format_A2R10G10B10_UNORM_PACK32) { return; }
  if (fmt_ == Image_Format_A2B10G10R10_UNORM_PACK32) { return; }

  ImageTester(w_, h_, d_, s_, fmt_, doLog_);
}

#define IMAGE_TEST_CASE(FMT, W, H, D, S, DOLOG) \
TEST_CASE( "Image 2D "#FMT" "#W"_"#H"_"#D"_"#S, "[Image]" ) \
{ \
    ImageTester((W), (H), (D), (S), Image_Format_##FMT, DOLOG); \
}
#define IMAGE_TEST_CASE_FILTERED(FMT, W, H, D, S, DOLOG) \
TEST_CASE( "Image 2D "#FMT" "#W"_"#H"_"#D"_"#S, "[Image]" ) \
{ \
    ImageTesterFiltered((W), (H), (D), (S), Image_Format_##FMT, DOLOG); \
}
#define DO_BASIC_TESTS 1

#if DO_BASIC_TESTS == 1

// basic suffix tests
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_SINT, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_USCALED, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_SSCALED, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UNORM, 10, 1, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_SNORM, 10, 1, 1, 1, false)

// basic dimension tests
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 1, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 10, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 10, 10, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 10, 1, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 1, 1, 10, false)
IMAGE_TEST_CASE(R8G8B8A8_UINT, 10, 10, 1, 10, false)

#endif

// format tests
#define TEST_ALL_FORMATS 1
#if TEST_ALL_FORMATS == 0
IMAGE_TEST_CASE(D24_UNORM_S8_UINT, 16, 1, 1, 1, true)
#else

//IMAGE_TEST_CASE(D24_UNORM_S8_UINT, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(D16_UNORM_S8_UINT, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A1R5G5B5_UNORM_PACK16, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A2R10G10B10_UNORM_PACK32, 16, 1, 1, 1, true)
//IMAGE_TEST_CASE(A2B10G10R10_UNORM_PACK32, 16, 1, 1, 1, true)

#define IF_START_MACRO
#define IF_MOD_MACRO(x) IMAGE_TEST_CASE_FILTERED(x, 256, 4, 2, 2, false)
#define IF_END_MACRO

#endif