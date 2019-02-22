#pragma once
#ifndef WYRD_IMAGE_FORMAT_CRACKER_H
#define WYRD_IMAGE_FORMAT_CRACKER_H

#include "core/core.h"
#include "core/logger.h"
#include "image/format.h"
#include <limits.h> // for max/min types
#include <float.h> // for max/min float types

EXTERN_C char const *Image_Format_Name(enum Image_Format_t const fmt);

EXTERN_C inline bool Image_Format_IsDepth(enum Image_Format_t const fmt) {
  switch (fmt) {
    case Image_Format_D16_UNORM:
    case Image_Format_X8_D24_UNORM_PACK32:
    case Image_Format_D32_SFLOAT:
    case Image_Format_D16_UNORM_S8_UINT:
    case Image_Format_D24_UNORM_S8_UINT:
    case Image_Format_D32_SFLOAT_S8_UINT:return true;
    default:return false;
  }
}

EXTERN_C inline bool Image_Format_IsStencil(enum Image_Format_t const fmt) {
  switch (fmt) {
    case Image_Format_S8_UINT:
    case Image_Format_D16_UNORM_S8_UINT:
    case Image_Format_D24_UNORM_S8_UINT:
    case Image_Format_D32_SFLOAT_S8_UINT:return true;
    default:return false;
  }
}

//! is this texture format a depth stencil format?
EXTERN_C inline bool Image_Format_IsDepthStencil(enum Image_Format_t const fmt) {
  switch (fmt) {
    case Image_Format_D16_UNORM_S8_UINT:
    case Image_Format_D24_UNORM_S8_UINT:
    case Image_Format_D32_SFLOAT_S8_UINT:return true;
    default:return false;
  }
}

EXTERN_C inline bool Image_Format_IsFloat(enum Image_Format_t const fmt) {
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
    case Image_Format_BC6H_SFLOAT_BLOCK:return true;
    default:return false;
  }
}

EXTERN_C inline bool Image_Format_IsNormalised(enum Image_Format_t const fmt) {
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
    case Image_Format_BC7_UNORM_BLOCK:return true;
    default:return false;
  }
}

EXTERN_C inline bool Image_Format_IsSigned(enum Image_Format_t const fmt) {
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
    case Image_Format_BC6H_SFLOAT_BLOCK:return true;

    default: return false;
  }
}
EXTERN_C inline bool Image_Format_IsSRGB(enum Image_Format_t const fmt) {
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
    case Image_Format_BC7_SRGB_BLOCK:return true;

    default: return false;
  }
}

EXTERN_C inline bool Image_Format_IsCompressed(enum Image_Format_t const fmt) {
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
    case Image_Format_BC7_SRGB_BLOCK:return true;
    default:return false;
  }
}

//! returns the number of channels per gl format
EXTERN_C inline bool Image_Format_ChannelCount(enum Image_Format_t const fmt) {
  switch (fmt) {
    case Image_Format_R4G4B4A4_UNORM_PACK16:
    case Image_Format_B4G4R4A4_UNORM_PACK16:
    case Image_Format_R5G5B5A1_UNORM_PACK16:
    case Image_Format_B5G5R5A1_UNORM_PACK16:
    case Image_Format_A1R5G5B5_UNORM_PACK16:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_SRGB:
    case Image_Format_B8G8R8A8_UNORM:
    case Image_Format_B8G8R8A8_SNORM:
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_B8G8R8A8_UINT:
    case Image_Format_B8G8R8A8_SINT:
    case Image_Format_B8G8R8A8_SRGB:
    case Image_Format_A8B8G8R8_UNORM_PACK32:
    case Image_Format_A8B8G8R8_SNORM_PACK32:
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_A8B8G8R8_UINT_PACK32:
    case Image_Format_A8B8G8R8_SINT_PACK32:
    case Image_Format_A8B8G8R8_SRGB_PACK32:
    case Image_Format_A2R10G10B10_UNORM_PACK32:
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
    case Image_Format_A2B10G10R10_UNORM_PACK32:
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
    case Image_Format_R16G16B16A16_UNORM:
    case Image_Format_R16G16B16A16_SNORM:
    case Image_Format_R16G16B16A16_USCALED:
    case Image_Format_R16G16B16A16_SSCALED:
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16A16_SFLOAT:
    case Image_Format_R32G32B32A32_UINT:
    case Image_Format_R32G32B32A32_SINT:
    case Image_Format_R32G32B32A32_SFLOAT:
    case Image_Format_R64G64B64A64_UINT:
    case Image_Format_R64G64B64A64_SINT:
    case Image_Format_R64G64B64A64_SFLOAT:
    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:
    case Image_Format_BC7_UNORM_BLOCK:
    case Image_Format_BC7_SRGB_BLOCK:return 4;

    case Image_Format_R5G6B5_UNORM_PACK16:
    case Image_Format_B5G6R5_UNORM_PACK16:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_SRGB:
    case Image_Format_B8G8R8_UNORM:
    case Image_Format_B8G8R8_SNORM:
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_B8G8R8_UINT:
    case Image_Format_B8G8R8_SINT:
    case Image_Format_B8G8R8_SRGB:
    case Image_Format_R16G16B16_UNORM:
    case Image_Format_R16G16B16_SNORM:
    case Image_Format_R16G16B16_USCALED:
    case Image_Format_R16G16B16_SSCALED:
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16B16_SFLOAT:
    case Image_Format_R32G32B32_UINT:
    case Image_Format_R32G32B32_SINT:
    case Image_Format_R32G32B32_SFLOAT:
    case Image_Format_R64G64B64_UINT:
    case Image_Format_R64G64B64_SINT:
    case Image_Format_R64G64B64_SFLOAT:
    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_BC6H_UFLOAT_BLOCK:
    case Image_Format_BC6H_SFLOAT_BLOCK:return 3;

    case Image_Format_R4G4_UNORM_PACK8:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_SRGB:
    case Image_Format_R16G16_UNORM:
    case Image_Format_R16G16_SNORM:
    case Image_Format_R16G16_USCALED:
    case Image_Format_R16G16_SSCALED:
    case Image_Format_R16G16_UINT:
    case Image_Format_R16G16_SINT:
    case Image_Format_R16G16_SFLOAT:
    case Image_Format_R32G32_UINT:
    case Image_Format_R32G32_SINT:
    case Image_Format_R32G32_SFLOAT:
    case Image_Format_R64G64_UINT:
    case Image_Format_R64G64_SINT:
    case Image_Format_R64G64_SFLOAT:
    case Image_Format_X8_D24_UNORM_PACK32:
    case Image_Format_D16_UNORM_S8_UINT:
    case Image_Format_D24_UNORM_S8_UINT:
    case Image_Format_D32_SFLOAT_S8_UINT:
    case Image_Format_BC5_UNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK:return 2;

    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_SRGB:
    case Image_Format_R16_UNORM:
    case Image_Format_R16_SNORM:
    case Image_Format_R16_USCALED:
    case Image_Format_R16_SSCALED:
    case Image_Format_R16_UINT:
    case Image_Format_R16_SINT:
    case Image_Format_R16_SFLOAT:
    case Image_Format_R32_UINT:
    case Image_Format_R32_SINT:
    case Image_Format_R32_SFLOAT:
    case Image_Format_R64_UINT:
    case Image_Format_R64_SINT:
    case Image_Format_R64_SFLOAT:
    case Image_Format_D16_UNORM:
    case Image_Format_D32_SFLOAT:
    case Image_Format_S8_UINT:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC4_SNORM_BLOCK:return 1;
    case Image_Format_UNDEFINED:return 0;

    default:LOGWARNINGF("channelCount: %s not handled", Image_Format_Name(fmt));
      return 0;
  }
}

//! Returns the number of channel bits
EXTERN_C inline bool Image_Format_ChannelBitWidth(enum Image_Format_t const fmt, int const channel_) {
  switch (fmt) {
    case Image_Format_R64_UINT:
    case Image_Format_R64_SINT:
    case Image_Format_R64_SFLOAT:
    case Image_Format_R64G64_UINT:
    case Image_Format_R64G64_SINT:
    case Image_Format_R64G64_SFLOAT:
    case Image_Format_R64G64B64_UINT:
    case Image_Format_R64G64B64_SINT:
    case Image_Format_R64G64B64_SFLOAT:
    case Image_Format_R64G64B64A64_UINT:
    case Image_Format_R64G64B64A64_SINT:
    case Image_Format_R64G64B64A64_SFLOAT:return 64;
    case Image_Format_R32_UINT:
    case Image_Format_R32_SINT:
    case Image_Format_R32_SFLOAT:
    case Image_Format_R32G32_UINT:
    case Image_Format_R32G32_SINT:
    case Image_Format_R32G32_SFLOAT:
    case Image_Format_R32G32B32_UINT:
    case Image_Format_R32G32B32_SINT:
    case Image_Format_R32G32B32_SFLOAT:
    case Image_Format_R32G32B32A32_UINT:
    case Image_Format_R32G32B32A32_SINT:
    case Image_Format_R32G32B32A32_SFLOAT:
    case Image_Format_D32_SFLOAT:return 32;
    case Image_Format_X8_D24_UNORM_PACK32:
      if (channel_ == 1) { return 24; }
      else { return 8; }
    case Image_Format_D16_UNORM_S8_UINT:
      if (channel_ == 0) { return 16; }
      else { return 8; }
    case Image_Format_D24_UNORM_S8_UINT:
      if (channel_ == 0) { return 24; }
      else { return 8; }
    case Image_Format_D32_SFLOAT_S8_UINT:
      if (channel_ == 0) { return 32; }
      else { return 8; }

    case Image_Format_R16_UNORM:
    case Image_Format_R16_SNORM:
    case Image_Format_R16_USCALED:
    case Image_Format_R16_SSCALED:
    case Image_Format_R16_UINT:
    case Image_Format_R16_SINT:
    case Image_Format_R16_SFLOAT:
    case Image_Format_R16G16_UNORM:
    case Image_Format_R16G16_SNORM:
    case Image_Format_R16G16_USCALED:
    case Image_Format_R16G16_SSCALED:
    case Image_Format_R16G16_UINT:
    case Image_Format_R16G16_SINT:
    case Image_Format_R16G16_SFLOAT:
    case Image_Format_R16G16B16_UNORM:
    case Image_Format_R16G16B16_SNORM:
    case Image_Format_R16G16B16_USCALED:
    case Image_Format_R16G16B16_SSCALED:
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16B16_SFLOAT:
    case Image_Format_R16G16B16A16_UNORM:
    case Image_Format_R16G16B16A16_SNORM:
    case Image_Format_R16G16B16A16_USCALED:
    case Image_Format_R16G16B16A16_SSCALED:
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16A16_SFLOAT:
    case Image_Format_D16_UNORM:return 16;
//			case GenericTextureFormat::B10G11R11_UFLOAT_PACK32:
//				if(channel_ == 0) return 10;
//				else return 11;
    case Image_Format_A2R10G10B10_UNORM_PACK32:
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
    case Image_Format_A2B10G10R10_UNORM_PACK32:
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
      if (channel_ == 0) { return 2; }
      else { return 10; }
//			case GenericTextureFormat::E5B9G9R9_UFLOAT_PACK32:
//				if(channel_ == 0) return 5;
//				else return 9;
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_SRGB:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_SRGB:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_SRGB:
    case Image_Format_B8G8R8_UNORM:
    case Image_Format_B8G8R8_SNORM:
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_B8G8R8_UINT:
    case Image_Format_B8G8R8_SINT:
    case Image_Format_B8G8R8_SRGB:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_SRGB:
    case Image_Format_B8G8R8A8_UNORM:
    case Image_Format_B8G8R8A8_SNORM:
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_B8G8R8A8_UINT:
    case Image_Format_B8G8R8A8_SINT:
    case Image_Format_B8G8R8A8_SRGB:
    case Image_Format_A8B8G8R8_UNORM_PACK32:
    case Image_Format_A8B8G8R8_SNORM_PACK32:
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_A8B8G8R8_UINT_PACK32:
    case Image_Format_A8B8G8R8_SINT_PACK32:
    case Image_Format_A8B8G8R8_SRGB_PACK32:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC4_SNORM_BLOCK:
    case Image_Format_BC5_UNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK:
    case Image_Format_BC6H_SFLOAT_BLOCK:
    case Image_Format_BC6H_UFLOAT_BLOCK:
    case Image_Format_S8_UINT:return 8;
// BC7 is variable between 4-7 bits for colour and 0-8 bits alpha
    case Image_Format_BC7_UNORM_BLOCK:
    case Image_Format_BC7_SRGB_BLOCK:return 6;
    case Image_Format_R5G5B5A1_UNORM_PACK16:
    case Image_Format_B5G5R5A1_UNORM_PACK16:
      if (channel_ == 4) { return 1; }
      else { return 5; }
    case Image_Format_A1R5G5B5_UNORM_PACK16:
      if (channel_ == 0) { return 1; }
      else { return 5; }
    case Image_Format_B5G6R5_UNORM_PACK16:
    case Image_Format_R5G6B5_UNORM_PACK16:
    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:return 5;
    case Image_Format_R4G4_UNORM_PACK8:
    case Image_Format_R4G4B4A4_UNORM_PACK16:
    case Image_Format_B4G4R4A4_UNORM_PACK16:return 4;
    case Image_Format_UNDEFINED:return 0;

      // unknown
    default: LOGWARNINGF("channelBitWidth: %s not handled", Image_Format_Name(fmt));
      return 0;
  }
}

/// \fn	uint32_t Image_Format_BitWidth ( Image_Format_t fmt )
/// \brief	Returns the number of bits in the Image_Format_t
/// \details BCx are block compressed the value is the amount per texel even tho never accessed
/// 		 like that, so BC1 take 4 bits to store a texel but can't be accessed 1 pixel at at time
/// \param	fmt	Describes the format to use.
/// \return	The bit width of the format.
EXTERN_C inline uint32_t Image_Format_BitWidth(enum Image_Format_t const fmt) {
  switch (fmt) {
    case Image_Format_R64G64B64A64_UINT:
    case Image_Format_R64G64B64A64_SINT:
    case Image_Format_R64G64B64A64_SFLOAT:return 256;
    case Image_Format_R64G64B64_UINT:
    case Image_Format_R64G64B64_SINT:
    case Image_Format_R64G64B64_SFLOAT:return 192;

    case Image_Format_R64G64_UINT:
    case Image_Format_R64G64_SINT:
    case Image_Format_R64G64_SFLOAT:
    case Image_Format_R32G32B32A32_UINT:
    case Image_Format_R32G32B32A32_SINT:
    case Image_Format_R32G32B32A32_SFLOAT:return 128;

    case Image_Format_R32G32B32_UINT:
    case Image_Format_R32G32B32_SINT:
    case Image_Format_R32G32B32_SFLOAT:return 96;
    case Image_Format_R64_UINT:
    case Image_Format_R64_SINT:
    case Image_Format_R64_SFLOAT:
    case Image_Format_R32G32_UINT:
    case Image_Format_R32G32_SINT:
    case Image_Format_R32G32_SFLOAT:
    case Image_Format_R16G16B16A16_UNORM:
    case Image_Format_R16G16B16A16_SNORM:
    case Image_Format_R16G16B16A16_USCALED:
    case Image_Format_R16G16B16A16_SSCALED:
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16A16_SFLOAT:return 64;
// best case 40 bits worse case 64, be a pessimist
    case Image_Format_D32_SFLOAT_S8_UINT:return 64;

    case Image_Format_R16G16B16_UNORM:
    case Image_Format_R16G16B16_SNORM:
    case Image_Format_R16G16B16_USCALED:
    case Image_Format_R16G16B16_SSCALED:
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16B16_SFLOAT:return 48;
    case Image_Format_R32_SINT:
    case Image_Format_R32_UINT:
    case Image_Format_R32_SFLOAT:
    case Image_Format_R16G16_SINT:
    case Image_Format_R16G16_SFLOAT:
    case Image_Format_R16G16_SNORM:
    case Image_Format_R16G16_UNORM:
    case Image_Format_R16G16_UINT:
    case Image_Format_R16G16_USCALED:
    case Image_Format_R16G16_SSCALED:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_SRGB:
    case Image_Format_B8G8R8A8_UNORM:
    case Image_Format_B8G8R8A8_SNORM:
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_B8G8R8A8_UINT:
    case Image_Format_B8G8R8A8_SINT:
    case Image_Format_B8G8R8A8_SRGB:
    case Image_Format_A8B8G8R8_UNORM_PACK32:
    case Image_Format_A8B8G8R8_SNORM_PACK32:
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_A8B8G8R8_UINT_PACK32:
    case Image_Format_A8B8G8R8_SINT_PACK32:
    case Image_Format_A8B8G8R8_SRGB_PACK32:
    case Image_Format_A2R10G10B10_UNORM_PACK32:
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
    case Image_Format_A2B10G10R10_UNORM_PACK32:
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
//			case GenericTextureFormat::B10G11R11_UFLOAT_PACK32:
//			case GenericTextureFormat::E5B9G9R9_UFLOAT_PACK32:
    case Image_Format_X8_D24_UNORM_PACK32:
    case Image_Format_D32_SFLOAT:
    case Image_Format_D24_UNORM_S8_UINT:return 32;
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_SRGB:
    case Image_Format_B8G8R8_UNORM:
    case Image_Format_B8G8R8_SNORM:
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_B8G8R8_UINT:
    case Image_Format_B8G8R8_SINT:
    case Image_Format_B8G8R8_SRGB:
    case Image_Format_D16_UNORM_S8_UINT:return 24;
    case Image_Format_R4G4B4A4_UNORM_PACK16:
    case Image_Format_B4G4R4A4_UNORM_PACK16:
    case Image_Format_R5G6B5_UNORM_PACK16:
    case Image_Format_B5G6R5_UNORM_PACK16:
    case Image_Format_R5G5B5A1_UNORM_PACK16:
    case Image_Format_B5G5R5A1_UNORM_PACK16:
    case Image_Format_A1R5G5B5_UNORM_PACK16:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_SRGB:
    case Image_Format_R16_UNORM:
    case Image_Format_R16_SNORM:
    case Image_Format_R16_USCALED:
    case Image_Format_R16_SSCALED:
    case Image_Format_R16_UINT:
    case Image_Format_R16_SINT:
    case Image_Format_R16_SFLOAT:
    case Image_Format_D16_UNORM:return 16;

    case Image_Format_R4G4_UNORM_PACK8:
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_SRGB:
    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:
    case Image_Format_BC5_UNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK:
    case Image_Format_BC6H_UFLOAT_BLOCK:
    case Image_Format_BC6H_SFLOAT_BLOCK:
    case Image_Format_BC7_UNORM_BLOCK:
    case Image_Format_BC7_SRGB_BLOCK:
    case Image_Format_S8_UINT:return 8;
    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC4_SNORM_BLOCK:return 4;
    default: LOGWARNINGF("bitWidth: %s not handled", Image_Format_Name(fmt));
      return 0;
  }
}

EXTERN_C inline double Image_Format_Max(enum Image_Format_t const fmt, int channel) {
  switch (fmt) {
    case Image_Format_R64G64B64A64_UINT:
    case Image_Format_R64G64B64_UINT:
    case Image_Format_R64G64_UINT:
    case Image_Format_R64_UINT:return (double) UINT64_MAX;

    case Image_Format_R64G64B64A64_SINT:
    case Image_Format_R64G64B64_SINT:
    case Image_Format_R64G64_SINT:
    case Image_Format_R64_SINT:return (double) INT64_MAX;

    case Image_Format_R64G64B64_SFLOAT:
    case Image_Format_R64G64B64A64_SFLOAT:
    case Image_Format_R64G64_SFLOAT:
    case Image_Format_R64_SFLOAT:return DBL_MAX;

    case Image_Format_R32G32B32A32_UINT:
    case Image_Format_R32G32B32_UINT:
    case Image_Format_R32G32_UINT:
    case Image_Format_R32_UINT:return UINT32_MAX;

    case Image_Format_R32G32B32A32_SINT:
    case Image_Format_R32G32B32_SINT:
    case Image_Format_R32G32_SINT:
    case Image_Format_R32_SINT:return INT32_MAX;

    case Image_Format_R32G32B32A32_SFLOAT:
    case Image_Format_R32G32B32_SFLOAT:
    case Image_Format_R32G32_SFLOAT:
    case Image_Format_R32_SFLOAT:
    case Image_Format_D32_SFLOAT:return FLT_MAX;

    case Image_Format_R16G16B16A16_UNORM:
    case Image_Format_R16G16B16_UNORM:
    case Image_Format_R16G16_UNORM:
    case Image_Format_R16_UNORM:
    case Image_Format_D16_UNORM:
    case Image_Format_R16G16B16A16_USCALED:
    case Image_Format_R16G16B16_USCALED:
    case Image_Format_R16G16_USCALED:
    case Image_Format_R16_USCALED:
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16_UINT:
    case Image_Format_R16_UINT:return UINT16_MAX;

    case Image_Format_R16G16B16A16_SNORM:
    case Image_Format_R16G16B16_SNORM:
    case Image_Format_R16G16_SNORM:
    case Image_Format_R16_SNORM:
    case Image_Format_R16G16B16A16_SSCALED:
    case Image_Format_R16G16B16_SSCALED:
    case Image_Format_R16G16_SSCALED:
    case Image_Format_R16_SSCALED:
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16_SINT:
    case Image_Format_R16_SINT:return INT16_MAX;

    case Image_Format_R16G16B16A16_SFLOAT:
    case Image_Format_R16G16B16_SFLOAT:
    case Image_Format_R16G16_SFLOAT:
    case Image_Format_R16_SFLOAT:
    case Image_Format_BC6H_UFLOAT_BLOCK:
    case Image_Format_BC6H_SFLOAT_BLOCK:return 0x1.ffcp15;

    case Image_Format_D32_SFLOAT_S8_UINT:
      if (channel == 0) { return FLT_MAX; }
      else { return UINT8_MAX; }

    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_B8G8R8A8_UNORM:
    case Image_Format_A8B8G8R8_UNORM_PACK32:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_B8G8R8_UNORM:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8_UNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8_USCALED:
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_B8G8R8A8_UINT:
    case Image_Format_A8B8G8R8_UINT_PACK32:
    case Image_Format_R8G8B8_UINT:
    case Image_Format_B8G8R8_UINT:
    case Image_Format_R8G8_UINT:
    case Image_Format_R8_UINT:
    case Image_Format_S8_UINT:
    case Image_Format_A8B8G8R8_SRGB_PACK32:
    case Image_Format_R8G8B8A8_SRGB:
    case Image_Format_B8G8R8A8_SRGB:
    case Image_Format_B8G8R8_SRGB:
    case Image_Format_R8G8B8_SRGB:
    case Image_Format_R8G8_SRGB:
    case Image_Format_R8_SRGB:return UINT8_MAX;

    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_B8G8R8A8_SNORM:
    case Image_Format_A8B8G8R8_SNORM_PACK32:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_B8G8R8_SNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_B8G8R8A8_SINT:
    case Image_Format_A8B8G8R8_SINT_PACK32:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_B8G8R8_SINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8_SINT:return INT8_MAX;

    case Image_Format_A2R10G10B10_UNORM_PACK32:
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
    case Image_Format_A2B10G10R10_UNORM_PACK32:
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
      if (channel == 0) { return 3.0; }
      else { return 1023.0; }

    case Image_Format_X8_D24_UNORM_PACK32:
      if (channel == 0) { return 255.0; }
      else { return (double) (1 << 24) - 1.0; };
    case Image_Format_D24_UNORM_S8_UINT:
      if (channel == 1) { return 255.0; }
      else {
        return (double) (1 << 24) - 1.0;
      }
    case Image_Format_D16_UNORM_S8_UINT:
      if (channel == 1) { return 255.0; }
      else { return 65535.0; }
    case Image_Format_R4G4B4A4_UNORM_PACK16:
    case Image_Format_B4G4R4A4_UNORM_PACK16:
    case Image_Format_R4G4_UNORM_PACK8:return 15.0;
    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_R5G6B5_UNORM_PACK16:
    case Image_Format_B5G6R5_UNORM_PACK16:
      if (channel == 1) { return 63.0; }
      else { return 31.0; }

    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_R5G5B5A1_UNORM_PACK16:
    case Image_Format_B5G5R5A1_UNORM_PACK16:
      if (channel == 3) { return 1.0; }
      else { return 31.0; }
    case Image_Format_A1R5G5B5_UNORM_PACK16:
      if (channel == 0) { return 1.0; }
      else { return 31.0; }
//			case GenericTextureFormat::B10G11R11_UFLOAT_PACK32:
//				return 65000.0;
//			case GenericTextureFormat::E5B9G9R9_UFLOAT_PACK32:
//				return 65472.0;

    case Image_Format_UNDEFINED:
    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC4_SNORM_BLOCK:
    case Image_Format_BC5_UNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK:
    case Image_Format_BC7_UNORM_BLOCK:
    case Image_Format_BC7_SRGB_BLOCK:return 255.0; // TODO
    default:LOGWARNINGF("max %s not handled", Image_Format_Name(fmt));
      return 0;
  }
}

EXTERN_C inline double Image_Format_Min(enum Image_Format_t const fmt, int channel) {
  switch (fmt) {
    case Image_Format_R64G64B64A64_UINT:
    case Image_Format_R64G64B64_UINT:
    case Image_Format_R64G64_UINT:
    case Image_Format_R64_UINT:return 0.0;

    case Image_Format_R64G64B64A64_SINT:
    case Image_Format_R64G64B64_SINT:
    case Image_Format_R64G64_SINT:
    case Image_Format_R64_SINT:return INT64_MIN;

    case Image_Format_R64G64B64_SFLOAT:
    case Image_Format_R64G64B64A64_SFLOAT:
    case Image_Format_R64G64_SFLOAT:
    case Image_Format_R64_SFLOAT:return FLT_MIN;

    case Image_Format_R32G32B32A32_UINT:
    case Image_Format_R32G32B32_UINT:
    case Image_Format_R32G32_UINT:
    case Image_Format_R32_UINT:return 0.0;

    case Image_Format_R32G32B32A32_SINT:
    case Image_Format_R32G32B32_SINT:
    case Image_Format_R32G32_SINT:
    case Image_Format_R32_SINT:return INT32_MIN;

    case Image_Format_R32G32B32A32_SFLOAT:
    case Image_Format_R32G32B32_SFLOAT:
    case Image_Format_R32G32_SFLOAT:
    case Image_Format_R32_SFLOAT:
    case Image_Format_D32_SFLOAT:return FLT_MIN;

    case Image_Format_R16G16B16A16_UNORM:
    case Image_Format_R16G16B16_UNORM:
    case Image_Format_R16G16_UNORM:
    case Image_Format_R16_UNORM:
    case Image_Format_D16_UNORM:
    case Image_Format_R16G16B16A16_USCALED:
    case Image_Format_R16G16B16_USCALED:
    case Image_Format_R16G16_USCALED:
    case Image_Format_R16_USCALED:
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16_UINT:
    case Image_Format_R16_UINT:return 0.0;

    case Image_Format_R16G16B16A16_SNORM:
    case Image_Format_R16G16B16_SNORM:
    case Image_Format_R16G16_SNORM:
    case Image_Format_R16_SNORM:
    case Image_Format_R16G16B16A16_SSCALED:
    case Image_Format_R16G16B16_SSCALED:
    case Image_Format_R16G16_SSCALED:
    case Image_Format_R16_SSCALED:
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16_SINT:
    case Image_Format_R16_SINT:return INT16_MIN;

    case Image_Format_R16G16B16A16_SFLOAT:
    case Image_Format_R16G16B16_SFLOAT:
    case Image_Format_R16G16_SFLOAT:
    case Image_Format_R16_SFLOAT:
    case Image_Format_BC6H_UFLOAT_BLOCK:
    case Image_Format_BC6H_SFLOAT_BLOCK: return 0x1.0p-14;

    case Image_Format_D32_SFLOAT_S8_UINT:
      if (channel == 0) { return FLT_MIN; }
      else { return 0; }

    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_B8G8R8A8_UNORM:
    case Image_Format_A8B8G8R8_UNORM_PACK32:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_B8G8R8_UNORM:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8_UNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8_USCALED:
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_B8G8R8A8_UINT:
    case Image_Format_A8B8G8R8_UINT_PACK32:
    case Image_Format_R8G8B8_UINT:
    case Image_Format_B8G8R8_UINT:
    case Image_Format_R8G8_UINT:
    case Image_Format_R8_UINT:
    case Image_Format_S8_UINT:
    case Image_Format_A8B8G8R8_SRGB_PACK32:
    case Image_Format_R8G8B8A8_SRGB:
    case Image_Format_B8G8R8A8_SRGB:
    case Image_Format_B8G8R8_SRGB:
    case Image_Format_R8G8B8_SRGB:
    case Image_Format_R8G8_SRGB:
    case Image_Format_R8_SRGB:return 0;

    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_B8G8R8A8_SNORM:
    case Image_Format_A8B8G8R8_SNORM_PACK32:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_B8G8R8_SNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_B8G8R8A8_SINT:
    case Image_Format_A8B8G8R8_SINT_PACK32:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_B8G8R8_SINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8_SINT:return INT8_MIN;

    case Image_Format_A2R10G10B10_UNORM_PACK32:
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
    case Image_Format_A2B10G10R10_UNORM_PACK32:
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
      if (channel == 0) { return 0.0; }
      else { return 0.0; }

    case Image_Format_X8_D24_UNORM_PACK32:
      if (channel == 0) { return 0.0; }
      else { return 0.0; };
    case Image_Format_D24_UNORM_S8_UINT:
      if (channel == 1) { return 0.0; }
      else {
        return 0.0;
      }
    case Image_Format_D16_UNORM_S8_UINT:
      if (channel == 1) { return 0.0; }
      else { return 0.0; }
    case Image_Format_R4G4B4A4_UNORM_PACK16:
    case Image_Format_B4G4R4A4_UNORM_PACK16:
    case Image_Format_R4G4_UNORM_PACK8:return 0.0;

    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_R5G6B5_UNORM_PACK16:
    case Image_Format_B5G6R5_UNORM_PACK16:
      if (channel == 1) { return 0.0; }
      else { return 0.0; }

    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_R5G5B5A1_UNORM_PACK16:
    case Image_Format_B5G5R5A1_UNORM_PACK16:
      if (channel == 3) { return 0.0; }
      else { return 0.0; }
    case Image_Format_A1R5G5B5_UNORM_PACK16:
      if (channel == 0) { return 0.0; }
      else { return 0.0; }

    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC5_UNORM_BLOCK:
    case Image_Format_BC7_UNORM_BLOCK:
    case Image_Format_BC7_SRGB_BLOCK: return 0.0;

    case Image_Format_BC4_SNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK: return INT8_MIN;

    case Image_Format_UNDEFINED: return 0.0;

    default:LOGWARNINGF("min %s not handled", Image_Format_Name(fmt));
      return 0.0;
  }
}

#endif //WYRD_IMAGE_FORMAT_CRACKER_H
