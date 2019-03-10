#pragma once
#ifndef WYRD_IMAGE_FETCH_HPP
#define WYRD_IMAGE_FETCH_HPP

#include "core/core.h"
#include "core/logger.h"
#include <numeric>

namespace Image {

template<typename type_>
auto FetchRaw(uint8_t const *ptr_) -> type_ {
  return *((type_ const *) ptr_);
}

template<typename type_>
auto FetchHomoChannel(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  return (double) FetchRaw<type_>(ptr_ + (sizeof(type_) * channel_));
}

template<typename type_>
auto FetchHomoChannel_NORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  return FetchHomoChannel<type_>(channel_, ptr_) / (double) std::numeric_limits<type_>::max();
}

template<typename type_>
auto FetchHomoChannel_sRGB(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  return Math_SRGB2Float(FetchRaw<type_>(ptr_ + sizeof(type_) * channel_));
}

auto FetchHomoChannel_nibble(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  uint8_t bite = *(ptr_ + ((int) channel_ / 2));
  return (double) (channel_ & 0x1) ?
         ((bite >> 0) & 0xF) :
         ((bite >> 4) & 0xF);
}

auto FetchHomoChannel_nibble_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  return FetchHomoChannel_nibble(channel_, ptr_) / 15.0;
}

auto FetchChannel_R5G6B5_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  uint16_t pixel = FetchRaw<uint16_t>(ptr_);
  if (channel_ == 0) { return ((double) ((pixel >> 11) & 0x1F)) / 31.0; }
  else if (channel_ == 1) { return ((double) ((pixel >> 5) & 0x3F)) / 63.0; }
  else if (channel_ == 2) { return ((double) ((pixel >> 0) & 0x1F)) / 31.0; }
  else {
    ASSERT((int) channel_ < 4);
    return 0.0;
  }
}

auto FetchChannel_R5G5B5A1_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  uint16_t pixel = FetchRaw<uint16_t>(ptr_);
  uint32_t x = 0;
  if (channel_ == 0) { x = (pixel >> 11) & 0x1F; }
  else if (channel_ == 1) { x = (pixel >> 6) & 0x1F; }
  else if (channel_ == 2) { x = (pixel >> 1) & 0x1F; }
  else if (channel_ == 3) { return ((double) ((pixel >> 0) & 0x1)); }
  else { ASSERT((int) channel_ < 4); }
  return ((double) (x)) / 31.0;
}

auto FetchChannel_A1R5G5B5_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  uint16_t pixel = FetchRaw<uint16_t>(ptr_);

  uint32_t x = 0;
  if (channel_ == 0) { return ((double) ((pixel >> 15) & 0x1)); }
  else if (channel_ == 1) { x = (pixel >> 10) & 0x1F; }
  else if (channel_ == 2) { x = (pixel >> 5) & 0x1F; }
  else if (channel_ == 3) { x = (pixel >> 0) & 0x1F; }
  else { ASSERT((int) channel_ < 4); }
  return ((double) (x)) / 31.0;
}

auto FetchHomoChannel_FP16(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  return (double) Math_Half2Float(FetchRaw<uint16_t>(ptr_ + (sizeof(uint16_t) * channel_)));
}

auto FetchChannel_A2R10G10B10(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  // fake fetch a single 32 bit format
  uint32_t pixel = FetchRaw<uint32_t>(ptr_);

  uint32_t x = 0;
  if (channel_ == 0) { return ((double) ((pixel >> 30) & 0x3)); }
  else if (channel_ == 1) { x = (pixel >> 20) & 0x3FF; }
  else if (channel_ == 2) { x = (pixel >> 10) & 0x3FF; }
  else if (channel_ == 3) { x = (pixel >> 0) & 0x3FF; }
  else { ASSERT((int) channel_ < 4); }
  return ((double) (x));
}

auto FetchChannel_A2R10G10B10_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  if (channel_ == 0) {
    return FetchChannel_A2R10G10B10(Image_Red, ptr_) / 3.0;
  } else {
    return FetchChannel_A2R10G10B10(channel_, ptr_) / 1023.0;
  }
}

auto FetchChannel_X8D24_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  // fake fetch a single 32 bit format
  uint32_t pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) { return ((double) ((pixel & 0xFF000000) >> 24) / 255.0); }
  else { return ((double) (pixel & 0x00FFFFFF) / 16777215.0); }
}

auto FetchChannel_D24X8_UNORM(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  uint32_t pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) { return ((double) (pixel & 0x000000FF) / 255.0); }
  else { return ((double) ((pixel & 0xFFFFFF00) >> 8) / 16777215.0); }
}

auto FetchChannel_D16S8_UNORM_UINT(enum Image_Channel_t channel_, uint8_t const *ptr_) -> double {
  if (channel_ == 0) {
    return FetchHomoChannel_NORM<uint16_t>(channel_, ptr_);
  } else {
    return FetchHomoChannel<uint8_t>(Image_Green, ptr_);
  }
}

auto CompressedChannelAt(Image_Header_t const *image, enum Image_Channel_t channel_, size_t index_) -> double {
  ASSERT(false);
  return 0.0;
}

auto BitWidth256ChannelAt(enum Image_Channel_t const channel_,
                          enum Image_Format_t const fmt_,
                          uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R64G64B64A64_UINT:
      return FetchHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                        ptr_); // potentially lossy!
    case Image_Format_R64G64B64A64_SINT:
      return FetchHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_); // potentially lossy!
    case Image_Format_R64G64B64A64_SFLOAT:return FetchHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth256ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth192ChannelAt(enum Image_Channel_t const channel_,
                          enum Image_Format_t const fmt_,
                          uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R64G64B64_UINT:
      return FetchHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                        ptr_); // potentially lossy!
    case Image_Format_R64G64B64_SINT:
      return FetchHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_); // potentially lossy!
    case Image_Format_R64G64B64_SFLOAT:return FetchHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth192ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth128ChannelAt(enum Image_Channel_t const channel_,
                          enum Image_Format_t const fmt_,
                          uint8_t const *ptr_) -> double {

  switch (fmt_) {
    case Image_Format_R64G64_UINT:
      return FetchHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                        ptr_); // potentially lossy!
    case Image_Format_R64G64_SINT:
      return FetchHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_); // potentially lossy!
    case Image_Format_R64G64_SFLOAT:return FetchHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32B32A32_UINT:return FetchHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32B32A32_SINT:return FetchHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32B32A32_SFLOAT:return FetchHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth128ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth96ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R32G32B32_UINT:return FetchHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32B32_SINT:return FetchHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32B32_SFLOAT:return FetchHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth96ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth64ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R64_UINT:
      return FetchHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                        ptr_); // potentially lossy!
    case Image_Format_R64_SINT:
      return FetchHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_); // potentially lossy!
    case Image_Format_R64_SFLOAT:return FetchHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32_UINT:return FetchHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32_SINT:return FetchHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32G32_SFLOAT:return FetchHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16B16A16_UNORM:
      return FetchHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                             ptr_);
    case Image_Format_R16G16B16A16_SNORM:
      return FetchHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                            ptr_);
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16A16_USCALED:
      return FetchHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                        ptr_);
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16A16_SSCALED:
      return FetchHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_);
    case Image_Format_R16G16B16A16_SFLOAT:return FetchHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_D32_SFLOAT_S8_UINT:
      if (channel_ == Image_Red) {
        return FetchHomoChannel<float>(Image_Red, ptr_);
      } else {
        return FetchHomoChannel<uint8_t>(Image_Red, ptr_ + 4);
      }
    default:LOGERRORF("%s not handled by bitWidth64ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth48ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R16G16B16_UNORM:
      return FetchHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                             ptr_);
    case Image_Format_R16G16B16_SNORM:
      return FetchHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                            ptr_);
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16B16_USCALED:return FetchHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16B16_SSCALED:return FetchHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16B16_SFLOAT:return FetchHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth48ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth32ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R32_UINT:return FetchHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R32_SINT:return FetchHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_D32_SFLOAT:
    case Image_Format_R32_SFLOAT:return FetchHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16_UNORM:return FetchHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16_SNORM:return FetchHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16_UINT:
    case Image_Format_R16G16_USCALED:return FetchHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16_SINT:
    case Image_Format_R16G16_SSCALED:return FetchHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16G16_SFLOAT:return FetchHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8A8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8A8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8A8_SRGB:
      if (channel_ == Image_Alpha) {
        return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      } else {
        return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      }
    case Image_Format_B8G8R8A8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8A8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_B8G8R8A8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_B8G8R8A8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8A8_SRGB:
      if (channel_ == Image_Alpha) {
        return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      } else {
        return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      }
    case Image_Format_A8B8G8R8_UNORM_PACK32:
      return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_),
                                            ptr_);
    case Image_Format_A8B8G8R8_SNORM_PACK32:
      return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_),
                                           ptr_);
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_A8B8G8R8_UINT_PACK32:
      return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_),
                                       ptr_);
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_A8B8G8R8_SINT_PACK32:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_A8B8G8R8_SRGB_PACK32:
      if (channel_ == Image_Alpha) {
        return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      } else {
        return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
      }
    case Image_Format_A2R10G10B10_UNORM_PACK32:
      return FetchChannel_A2R10G10B10_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                            ptr_);
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
      return FetchChannel_A2R10G10B10(Image_Channel_Swizzle(fmt_, channel_),
                                      ptr_);
    case Image_Format_A2B10G10R10_UNORM_PACK32:
      return FetchChannel_A2R10G10B10_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                            ptr_);
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
      return FetchChannel_A2R10G10B10(Image_Channel_Swizzle(fmt_, channel_),
                                      ptr_);
    case Image_Format_X8_D24_UNORM_PACK32:return FetchChannel_X8D24_UNORM(channel_, ptr_);
    case Image_Format_D24_UNORM_S8_UINT:return FetchChannel_D24X8_UNORM(channel_, ptr_);

      // TODO decoders for these exotic formats
//		case GenericTextureFormat::B10G11R11_UFLOAT_PACK32:
//		case GenericTextureFormat::E5B9G9R9_UFLOAT_PACK32:
    default:LOGERRORF("%s not handled by bitWidth32ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth24ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R8G8B8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8B8_SRGB:return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);

    case Image_Format_B8G8R8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_B8G8R8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_B8G8R8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B8G8R8_SRGB:return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);

    case Image_Format_D16_UNORM_S8_UINT:return FetchChannel_D16S8_UNORM_UINT(channel_, ptr_);

    default:LOGERRORF("%s not handled by bitWidth24ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth16ChannelAt(enum Image_Channel_t const channel_,
                         enum Image_Format_t const fmt_,
                         uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R4G4B4A4_UNORM_PACK16:
      return FetchHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                           ptr_);
    case Image_Format_B4G4R4A4_UNORM_PACK16:
      return FetchHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                           ptr_);
    case Image_Format_R5G6B5_UNORM_PACK16:return FetchChannel_R5G6B5_UNORM(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_B5G6R5_UNORM_PACK16:return FetchChannel_R5G6B5_UNORM(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R5G5B5A1_UNORM_PACK16:
      return FetchChannel_R5G5B5A1_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                         ptr_);
    case Image_Format_B5G5R5A1_UNORM_PACK16:
      return FetchChannel_R5G5B5A1_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                         ptr_);
    case Image_Format_A1R5G5B5_UNORM_PACK16:
      return FetchChannel_A1R5G5B5_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                         ptr_);

    case Image_Format_R8G8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8G8_SRGB:return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_D16_UNORM:
    case Image_Format_R16_UNORM:return FetchHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16_SNORM:return FetchHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16_UINT:
    case Image_Format_R16_USCALED:return FetchHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16_SINT:
    case Image_Format_R16_SSCALED:return FetchHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R16_SFLOAT:return FetchHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    default:LOGERRORF("%s not handled by bitWidth16ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

auto BitWidth8ChannelAt(enum Image_Channel_t const channel_,
                        enum Image_Format_t const fmt_,
                        uint8_t const *ptr_) -> double {
  switch (fmt_) {
    case Image_Format_R4G4_UNORM_PACK8:
      return FetchHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                           ptr_);
    case Image_Format_R8_UNORM:return FetchHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8_SNORM:return FetchHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_S8_UINT:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_UINT:return FetchHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SINT:return FetchHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);
    case Image_Format_R8_SRGB:return FetchHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_);

    default:LOGERRORF("%s not handled by bitWidth8ChannelAt", Image_Format_Name(fmt_));
      return 0.0;
  }
}

} // end image namespace

#endif //WYRD_IMAGE_FETCH_HPP
