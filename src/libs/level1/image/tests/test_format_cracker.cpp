#include "core/core.h"
#include "catch/catch.hpp"

#include "image/format.h"
#include "image/format_cracker.h"

TEST_CASE("Format Cracker IsDepth (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_D16_UNORM:
      case Image_Format_X8_D24_UNORM_PACK32:
      case Image_Format_D32_SFLOAT:
      case Image_Format_D16_UNORM_S8_UINT:
      case Image_Format_D24_UNORM_S8_UINT:
      case Image_Format_D32_SFLOAT_S8_UINT: {
        REQUIRE(Image_Format_IsDepth(fmt));
        break;
      };
      default:REQUIRE(Image_Format_IsDepth(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsStencil (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_S8_UINT:
      case Image_Format_D16_UNORM_S8_UINT:
      case Image_Format_D24_UNORM_S8_UINT:
      case Image_Format_D32_SFLOAT_S8_UINT: {
        REQUIRE(Image_Format_IsStencil(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsStencil(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsDepthStencil (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_D16_UNORM_S8_UINT:
      case Image_Format_D24_UNORM_S8_UINT:
      case Image_Format_D32_SFLOAT_S8_UINT: {
        REQUIRE(Image_Format_IsDepthStencil(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsDepthStencil(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsFloat (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_R16_SFLOAT:
      case Image_Format_R16G16_SFLOAT:
      case Image_Format_R16G16B16_SFLOAT:
      case Image_Format_R16G16B16A16_SFLOAT:
      case Image_Format_R32_SFLOAT:
      case Image_Format_R32G32_SFLOAT:
      case Image_Format_R32G32B32_SFLOAT:
      case Image_Format_R32G32B32A32_SFLOAT:
      case Image_Format_R64_SFLOAT:
      case Image_Format_R64G64_SFLOAT:
      case Image_Format_R64G64B64_SFLOAT:
      case Image_Format_R64G64B64A64_SFLOAT:
      case Image_Format_D32_SFLOAT:
      case Image_Format_D32_SFLOAT_S8_UINT:
      case Image_Format_BC6H_UFLOAT_BLOCK:
      case Image_Format_BC6H_SFLOAT_BLOCK: {
        REQUIRE(Image_Format_IsFloat(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsFloat(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsNormalised (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_R4G4_UNORM_PACK8:
      case Image_Format_R4G4B4A4_UNORM_PACK16:
      case Image_Format_B4G4R4A4_UNORM_PACK16:
      case Image_Format_R5G6B5_UNORM_PACK16:
      case Image_Format_B5G6R5_UNORM_PACK16:
      case Image_Format_R5G5B5A1_UNORM_PACK16:
      case Image_Format_B5G5R5A1_UNORM_PACK16:
      case Image_Format_A1R5G5B5_UNORM_PACK16:
      case Image_Format_R8_UNORM:
      case Image_Format_R8_SNORM:
      case Image_Format_R8G8_UNORM:
      case Image_Format_R8G8_SNORM:
      case Image_Format_R8G8B8_UNORM:
      case Image_Format_R8G8B8_SNORM:
      case Image_Format_B8G8R8_UNORM:
      case Image_Format_B8G8R8_SNORM:
      case Image_Format_R8G8B8A8_UNORM:
      case Image_Format_R8G8B8A8_SNORM:
      case Image_Format_B8G8R8A8_UNORM:
      case Image_Format_B8G8R8A8_SNORM:
      case Image_Format_A8B8G8R8_UNORM_PACK32:
      case Image_Format_A8B8G8R8_SNORM_PACK32:
      case Image_Format_A2R10G10B10_UNORM_PACK32:
      case Image_Format_A2B10G10R10_UNORM_PACK32:
      case Image_Format_R16_UNORM:
      case Image_Format_R16_SNORM:
      case Image_Format_R16G16_UNORM:
      case Image_Format_R16G16_SNORM:
      case Image_Format_R16G16B16_UNORM:
      case Image_Format_R16G16B16_SNORM:
      case Image_Format_R16G16B16A16_UNORM:
      case Image_Format_R16G16B16A16_SNORM:
      case Image_Format_X8_D24_UNORM_PACK32:
      case Image_Format_D16_UNORM:
      case Image_Format_D16_UNORM_S8_UINT:
      case Image_Format_D24_UNORM_S8_UINT:
      case Image_Format_BC1_RGB_UNORM_BLOCK:
      case Image_Format_BC1_RGBA_UNORM_BLOCK:
      case Image_Format_BC2_UNORM_BLOCK:
      case Image_Format_BC3_UNORM_BLOCK:
      case Image_Format_BC4_UNORM_BLOCK:
      case Image_Format_BC4_SNORM_BLOCK:
      case Image_Format_BC5_UNORM_BLOCK:
      case Image_Format_BC5_SNORM_BLOCK:
      case Image_Format_BC7_UNORM_BLOCK:
      case Image_Format_PVR_2BPP_BLOCK:
      case Image_Format_PVR_2BPPA_BLOCK:
      case Image_Format_PVR_4BPP_BLOCK:
      case Image_Format_PVR_4BPPA_BLOCK: {
        REQUIRE(Image_Format_IsNormalised(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsNormalised(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsSigned (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_R8_SNORM:
      case Image_Format_R8_SSCALED:
      case Image_Format_R8_SINT:
      case Image_Format_R8G8_SNORM:
      case Image_Format_R8G8_SSCALED:
      case Image_Format_R8G8_SINT:
      case Image_Format_R8G8B8_SNORM:
      case Image_Format_R8G8B8_SSCALED:
      case Image_Format_R8G8B8_SINT:
      case Image_Format_B8G8R8_SNORM:
      case Image_Format_B8G8R8_SINT:
      case Image_Format_B8G8R8_SSCALED:
      case Image_Format_R8G8B8A8_SNORM:
      case Image_Format_R8G8B8A8_SINT:
      case Image_Format_R8G8B8A8_SSCALED:
      case Image_Format_B8G8R8A8_SNORM:
      case Image_Format_B8G8R8A8_SINT:
      case Image_Format_B8G8R8A8_SSCALED:
      case Image_Format_A8B8G8R8_SNORM_PACK32:
      case Image_Format_A8B8G8R8_SINT_PACK32:
      case Image_Format_A8B8G8R8_SSCALED_PACK32:
      case Image_Format_R16_SNORM:
      case Image_Format_R16_SINT:
      case Image_Format_R16_SSCALED:
      case Image_Format_R16_SFLOAT:
      case Image_Format_R16G16_SNORM:
      case Image_Format_R16G16_SINT:
      case Image_Format_R16G16_SSCALED:
      case Image_Format_R16G16_SFLOAT:
      case Image_Format_R16G16B16_SNORM:
      case Image_Format_R16G16B16_SINT:
      case Image_Format_R16G16B16_SSCALED:
      case Image_Format_R16G16B16_SFLOAT:
      case Image_Format_R16G16B16A16_SNORM:
      case Image_Format_R16G16B16A16_SINT:
      case Image_Format_R16G16B16A16_SSCALED:
      case Image_Format_R16G16B16A16_SFLOAT:
      case Image_Format_R32_SINT:
      case Image_Format_R32_SFLOAT:
      case Image_Format_R32G32_SINT:
      case Image_Format_R32G32_SFLOAT:
      case Image_Format_R32G32B32_SINT:
      case Image_Format_R32G32B32_SFLOAT:
      case Image_Format_R32G32B32A32_SINT:
      case Image_Format_R32G32B32A32_SFLOAT:
      case Image_Format_R64_SINT:
      case Image_Format_R64_SFLOAT:
      case Image_Format_R64G64_SINT:
      case Image_Format_R64G64_SFLOAT:
      case Image_Format_R64G64B64_SINT:
      case Image_Format_R64G64B64_SFLOAT:
      case Image_Format_R64G64B64A64_SINT:
      case Image_Format_R64G64B64A64_SFLOAT:
      case Image_Format_D32_SFLOAT:
      case Image_Format_D32_SFLOAT_S8_UINT:
      case Image_Format_BC4_SNORM_BLOCK:
      case Image_Format_BC5_SNORM_BLOCK:
      case Image_Format_BC6H_SFLOAT_BLOCK: {
        REQUIRE(Image_Format_IsSigned(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsSigned(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsSRGB (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_R8_SRGB:
      case Image_Format_R8G8_SRGB:
      case Image_Format_R8G8B8_SRGB:
      case Image_Format_B8G8R8_SRGB:
      case Image_Format_R8G8B8A8_SRGB:
      case Image_Format_B8G8R8A8_SRGB:
      case Image_Format_A8B8G8R8_SRGB_PACK32:
      case Image_Format_BC1_RGB_SRGB_BLOCK:
      case Image_Format_BC1_RGBA_SRGB_BLOCK:
      case Image_Format_BC2_SRGB_BLOCK:
      case Image_Format_BC3_SRGB_BLOCK:
      case Image_Format_BC7_SRGB_BLOCK:
      case Image_Format_PVR_2BPP_SRGB_BLOCK:
      case Image_Format_PVR_2BPPA_SRGB_BLOCK:
      case Image_Format_PVR_4BPP_SRGB_BLOCK:
      case Image_Format_PVR_4BPPA_SRGB_BLOCK: {
        REQUIRE(Image_Format_IsSRGB(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsSRGB(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker IsCompressed (C)", "[Image]") {

#define IF_START_MACRO int formatCount = 0;
#define IF_MOD_MACRO(x) formatCount++;
#define IF_END_MACRO
#include "image/format.h"

  for (int i = 0; i < formatCount; ++i) {
    enum Image_Format fmt = (Image_Format) i;
    switch (fmt) {
      case Image_Format_BC1_RGB_UNORM_BLOCK:
      case Image_Format_BC1_RGB_SRGB_BLOCK:
      case Image_Format_BC1_RGBA_UNORM_BLOCK:
      case Image_Format_BC1_RGBA_SRGB_BLOCK:
      case Image_Format_BC2_UNORM_BLOCK:
      case Image_Format_BC2_SRGB_BLOCK:
      case Image_Format_BC3_UNORM_BLOCK:
      case Image_Format_BC3_SRGB_BLOCK:
      case Image_Format_BC4_UNORM_BLOCK:
      case Image_Format_BC4_SNORM_BLOCK:
      case Image_Format_BC5_UNORM_BLOCK:
      case Image_Format_BC5_SNORM_BLOCK:
      case Image_Format_BC6H_UFLOAT_BLOCK:
      case Image_Format_BC6H_SFLOAT_BLOCK:
      case Image_Format_BC7_UNORM_BLOCK:
      case Image_Format_BC7_SRGB_BLOCK:
      case Image_Format_PVR_2BPP_BLOCK:
      case Image_Format_PVR_2BPPA_BLOCK:
      case Image_Format_PVR_4BPP_BLOCK:
      case Image_Format_PVR_4BPPA_BLOCK:
      case Image_Format_PVR_2BPP_SRGB_BLOCK:
      case Image_Format_PVR_2BPPA_SRGB_BLOCK:
      case Image_Format_PVR_4BPP_SRGB_BLOCK:
      case Image_Format_PVR_4BPPA_SRGB_BLOCK: {
        REQUIRE(Image_Format_IsCompressed(fmt));
        break;
      };

      default:REQUIRE(Image_Format_IsCompressed(fmt) == false);
        break;
    }
  }
}

TEST_CASE("Format Cracker Min (C)", "[Image]") {

  // random sample a few to check
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_UINT_PACK32, 0) == 0);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_UINT_PACK32, 1) == 0);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_UINT_PACK32, 2) == 0);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_UINT_PACK32, 3) == 0);

  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_SINT_PACK32, 0) == -128);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_SINT_PACK32, 1) == -128);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_SINT_PACK32, 2) == -128);
  REQUIRE(Image_Format_Min(Image_Format_A8B8G8R8_SINT_PACK32, 3) == -128);

  REQUIRE(Image_Format_Min(Image_Format_R32G32B32A32_SFLOAT, 0) == Approx(FLT_MIN));
  REQUIRE(Image_Format_Min(Image_Format_R32G32B32A32_SFLOAT, 1) == Approx(FLT_MIN));
  REQUIRE(Image_Format_Min(Image_Format_R32G32B32A32_SFLOAT, 2) == Approx(FLT_MIN));
  REQUIRE(Image_Format_Min(Image_Format_R32G32B32A32_SFLOAT, 3) == Approx(FLT_MIN));

}

TEST_CASE("Format Cracker Max (C)", "[Image]") {

  // random sample a few to check
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_UINT_PACK32, 0) == 255);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_UINT_PACK32, 1) == 255);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_UINT_PACK32, 2) == 255);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_UINT_PACK32, 3) == 255);

  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_SINT_PACK32, 0) == 127);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_SINT_PACK32, 1) == 127);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_SINT_PACK32, 2) == 127);
  REQUIRE(Image_Format_Max(Image_Format_A8B8G8R8_SINT_PACK32, 3) == 127);

  REQUIRE(Image_Format_Max(Image_Format_R32G32B32A32_SFLOAT, 0) == Approx(FLT_MAX));
  REQUIRE(Image_Format_Max(Image_Format_R32G32B32A32_SFLOAT, 1) == Approx(FLT_MAX));
  REQUIRE(Image_Format_Max(Image_Format_R32G32B32A32_SFLOAT, 2) == Approx(FLT_MAX));
  REQUIRE(Image_Format_Max(Image_Format_R32G32B32A32_SFLOAT, 3) == Approx(FLT_MAX));

  REQUIRE(Image_Format_Max(Image_Format_D32_SFLOAT_S8_UINT, 0) == Approx(FLT_MAX));
  REQUIRE(Image_Format_Max(Image_Format_D32_SFLOAT_S8_UINT, 1) == 255);

}

TEST_CASE("Format Cracker Swizzle (C)", "[Image]") {

  // random sample a few to check
  REQUIRE(Image_Format_Swizzle(Image_Format_R32G32B32A32_SFLOAT) == Image_Format_Swizzle_RGBA);
  REQUIRE(Image_Format_Swizzle(Image_Format_R4G4_UNORM_PACK8) == Image_Format_Swizzle_RGBA);
  REQUIRE(Image_Format_Swizzle(Image_Format_R5G5B5A1_UNORM_PACK16) == Image_Format_Swizzle_RGBA);
  REQUIRE(Image_Format_Swizzle(Image_Format_A1R5G5B5_UNORM_PACK16) == Image_Format_Swizzle_ARGB);
  REQUIRE(Image_Format_Swizzle(Image_Format_A2R10G10B10_USCALED_PACK32) == Image_Format_Swizzle_ARGB);
  REQUIRE(Image_Format_Swizzle(Image_Format_B4G4R4A4_UNORM_PACK16) == Image_Format_Swizzle_BGRA);
  REQUIRE(Image_Format_Swizzle(Image_Format_B8G8R8_SRGB) == Image_Format_Swizzle_BGRA);
  REQUIRE(Image_Format_Swizzle(Image_Format_B8G8R8A8_USCALED) == Image_Format_Swizzle_BGRA);
  REQUIRE(Image_Format_Swizzle(Image_Format_A8B8G8R8_SINT_PACK32) == Image_Format_Swizzle_ABGR);
}