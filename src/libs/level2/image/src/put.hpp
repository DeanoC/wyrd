#pragma once
#ifndef WYRD_PUT_HPP
#define WYRD_PUT_HPP

#include "core/core.h"
#include "core/logger.h"
#include "math/math.hpp"
#include "fetch.hpp"
#include <numeric>

namespace Image {

template<typename type_>
auto PutRaw(uint8_t *ptr_, type_ const value_) -> void {
  type_ const v = Math::Clamp(value_, std::numeric_limits<type_>::min(), std::numeric_limits<type_>::max());

  *((type_ *) ptr_) = v;
}

template<typename type_>
auto PutHomoChannel(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  PutRaw<type_>(ptr_ + (sizeof(type_) * channel_), (type_) value_);
}

template<typename type_>
auto PutHomoChannel_NORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  PutHomoChannel<type_>(channel_, ptr_, value_ * (double) std::numeric_limits<type_>::max());
}

template<typename type_>
auto PutHomoChannel_sRGB(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  PutHomoChannel<type_>(channel_, ptr_, Math_Float2SRGB((float) value_));
}

auto PutHomoChannel_nibble(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  // fetch the byte to merge the nibble into
  uint8_t bite = ptr_[channel_ / 2];
  uint8_t nibble = (uint8_t) Math_ClampD(value_, 0.0, 15.0);
  bite = (channel_ & uint8_t(0x1u)) ?
         (bite & uint8_t(0xF0u)) | (nibble << 0u) :
         (bite & uint8_t(0x0Fu)) | (nibble << 4u);
  ptr_[channel_ / 2] = bite;

}

auto PutHomoChannel_nibble_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  PutHomoChannel_nibble(channel_, ptr_, value_ * 15.0);
}

auto PutChannel_R5G6B5_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint16_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0x07FF)) | (uint16_t) v << 11u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 63.0, 0.0, 63.0);
    pixel = (pixel & uint16_t(0xF81Fu)) | (uint16_t) v << 5u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0xFFE0u)) | (uint16_t) v << 0u;
  } else {
    ASSERT(channel_ < 3);
  }

  PutRaw(ptr_, pixel);
}

auto PutChannel_R5G5B5A1_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint16_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0x07FFu)) | ((uint16_t) v) << 11u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0xF83Fu)) | ((uint16_t) v) << 6u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0xFFC1u)) | ((uint16_t) v) << 1u;
  } else if (channel_ == 3) {
    double const v = Math_ClampD(value_, 0.0, 1.0);
    pixel = (pixel & uint16_t(0xFFFEu)) | (uint16_t) v << 0u;
  } else {
    assert(channel_ < 4);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_A1R5G5B5_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint16_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_, 0.0, 1.0);
    pixel = (pixel & uint16_t(0x7FFFu)) | (uint16_t) v << 15u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0x83FFu)) | (uint16_t) v << 10u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0xFC1Fu)) | (uint16_t) v << 5u;
  } else if (channel_ == 3) {
    double const v = Math_ClampD(value_ * 31.0, 0.0, 31.0);
    pixel = (pixel & uint16_t(0xFFE0u)) | (uint16_t) v << 0u;
  } else {
    assert(channel_ < 4);
  }
  PutRaw(ptr_, pixel);
}

auto PutHomoChannel_FP16(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  // clamp half?
  uint16_t h = Math_Float2Half((float) value_);
  PutRaw(ptr_ + (sizeof(uint16_t) * channel_), h);
}

auto PutChannel_A2R10G10B10(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_, 0.0, 3.0);
    pixel = (pixel & 0x3FFFFFFFu) | (uint32_t) v << 30u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xC00FFFFFu) | (uint32_t) v << 20u;
  } else if (channel_ == 2) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xFFF003FFu) | (uint32_t) v << 10u;
  } else if (channel_ == 3) {
    double const v = Math_ClampD(value_, 0.0, 1023.0);
    pixel = (pixel & 0xFFFFFC00u) | (uint32_t) v << 0u;
  } else {
    assert(channel_ < 4);
  }
  PutRaw(ptr_, pixel);

}

auto PutChannel_A2R10G10B10_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  if (channel_ == 0) { PutChannel_A2R10G10B10(channel_, ptr_, value_ * 3.0); }
  else { PutChannel_A2R10G10B10(channel_, ptr_, value_ * 1023.0); }
}

auto PutChannel_X8D24_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 255.0, 0.0, 255.0);
    pixel = (pixel & 0x00FFFFFFu) | (uint32_t) v << 24u;
  } else if (channel_ == 1) {
    static const double Max24Bit = double(1 << 24) - 1.0;
    double const v = Math_ClampD(value_ * Max24Bit, 0.0, Max24Bit);
    pixel = (pixel & 0xFF000000u) | (uint32_t) v << 0u;
  } else {
    assert(channel_ < 2);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_D24X8_UNORM(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  auto pixel = FetchRaw<uint32_t>(ptr_);
  if (channel_ == 0) {
    static const double Max24Bit = double(1u << 24u) - 1.0;
    double const v = Math_ClampD(value_ * Max24Bit, 0.0, Max24Bit);
    pixel = (pixel & 0x000000FFu) | (uint32_t) v << 8u;
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_ * 255.0, 0.0, 255.0);
    pixel = (pixel & 0xFFFFFF00u) | (uint32_t) v << 0u;
  } else {
    assert(channel_ < 2);
  }
  PutRaw(ptr_, pixel);
}

auto PutChannel_D16S8_UNORM_UINT(uint8_t channel_, uint8_t *ptr_, double const value_) -> void {
  if (channel_ == 0) {
    double const v = Math_ClampD(value_ * 65535.0, 0.0, 65535.0);
    PutHomoChannel<uint16_t>(0, ptr_, (uint16_t) v);
  } else if (channel_ == 1) {
    double const v = Math_ClampD(value_, 0.0, 255.0);
    PutHomoChannel<uint16_t>(0, ptr_ + 2, (uint8_t) v);
  } else {
    assert(channel_ < 2);
  }

}

auto BitWidth256SetChannelAt(enum Image_Channel const channel_,
                             enum Image_Format const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R64G64B64A64_UINT:PutHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64B64A64_SINT:PutHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64B64A64_SFLOAT:PutHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }
}

auto BitWidth192SetChannelAt(enum Image_Channel const channel_,
                             enum Image_Format const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R64G64B64_UINT:PutHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64B64_SINT:PutHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64B64_SFLOAT:PutHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }
}

auto BitWidth128SetChannelAt(enum Image_Channel const channel_,
                             enum Image_Format const fmt_,
                             uint8_t *ptr_,
                             double const value_) -> void {

  switch (fmt_) {
    case Image_Format_R64G64_UINT:PutHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64_SINT:PutHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R64G64_SFLOAT:PutHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32B32A32_UINT:PutHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32B32A32_SINT:PutHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32B32A32_SFLOAT:PutHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }
}

auto BitWidth96SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R32G32B32_UINT:PutHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32B32_SINT:PutHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32B32_SFLOAT:PutHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }

}

auto BitWidth64SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R64_UINT:
      PutHomoChannel<uint64_t>(Image_Channel_Swizzle(fmt_, channel_),
                               ptr_,
                               value_); // potentially lossy!
      break;
    case Image_Format_R64_SINT:
      PutHomoChannel<int64_t>(Image_Channel_Swizzle(fmt_, channel_),
                              ptr_,
                              value_); // potentially lossy!
      break;
    case Image_Format_R64_SFLOAT: PutHomoChannel<double>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32_UINT: PutHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32_SINT: PutHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32G32_SFLOAT: PutHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16B16A16_UNORM:
      PutHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                    ptr_,
                                    value_);
      break;
    case Image_Format_R16G16B16A16_SNORM:
      PutHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case Image_Format_R16G16B16A16_UINT:
    case Image_Format_R16G16B16A16_USCALED:
      PutHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                               ptr_,
                               value_);
      break;
    case Image_Format_R16G16B16A16_SINT:
    case Image_Format_R16G16B16A16_SSCALED:
      PutHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                              ptr_,
                              value_);
      break;
    case Image_Format_R16G16B16A16_SFLOAT: PutHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_D32_SFLOAT_S8_UINT:
      if (channel_ == Image_Red) {
        PutHomoChannel<float>(0, ptr_, value_);
      } else {
        PutHomoChannel<uint8_t>(0, ptr_ + 4, value_);
      }
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }

}

auto BitWidth48SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R16G16B16_UNORM:
      PutHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                    ptr_,
                                    value_);
      break;
    case Image_Format_R16G16B16_SNORM:
      PutHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case Image_Format_R16G16B16_UINT:
    case Image_Format_R16G16B16_USCALED: PutHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16B16_SINT:
    case Image_Format_R16G16B16_SSCALED: PutHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16B16_SFLOAT: PutHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }
}

auto BitWidth32SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R32_UINT: PutHomoChannel<uint32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R32_SINT: PutHomoChannel<int32_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_D32_SFLOAT:
    case Image_Format_R32_SFLOAT: PutHomoChannel<float>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16_UNORM: PutHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16_SNORM: PutHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16_UINT:
    case Image_Format_R16G16_USCALED: PutHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16_SINT:
    case Image_Format_R16G16_SSCALED: PutHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16G16_SFLOAT: PutHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8A8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8A8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8A8_SRGB:
      if (channel_ == Image_Alpha) {
        PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      } else {
        PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      }
      break;
    case Image_Format_B8G8R8A8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8A8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8A8_USCALED:
    case Image_Format_B8G8R8A8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8A8_SSCALED:
    case Image_Format_B8G8R8A8_SINT: PutHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8A8_SRGB:
      if (channel_ == Image_Alpha) {
        PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      } else {
        PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      }
      break;
    case Image_Format_A8B8G8R8_UNORM_PACK32:
      PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case Image_Format_A8B8G8R8_SNORM_PACK32:
      PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_),
                                  ptr_,
                                  value_);
      break;
    case Image_Format_A8B8G8R8_USCALED_PACK32:
    case Image_Format_A8B8G8R8_UINT_PACK32:
      PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_),
                              ptr_,
                              value_);
      break;
    case Image_Format_A8B8G8R8_SSCALED_PACK32:
    case Image_Format_A8B8G8R8_SINT_PACK32: PutHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_A8B8G8R8_SRGB_PACK32:
      if (channel_ == Image_Alpha) {
        PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      } else {
        PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      }
      break;
    case Image_Format_A2R10G10B10_UNORM_PACK32:
      PutChannel_A2R10G10B10_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case Image_Format_A2R10G10B10_USCALED_PACK32:
    case Image_Format_A2R10G10B10_UINT_PACK32:
      PutChannel_A2R10G10B10(Image_Channel_Swizzle(fmt_, channel_),
                             ptr_,
                             value_);
      break;
    case Image_Format_A2B10G10R10_UNORM_PACK32:
      PutChannel_A2R10G10B10_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                   ptr_,
                                   value_);
      break;
    case Image_Format_A2B10G10R10_USCALED_PACK32:
    case Image_Format_A2B10G10R10_UINT_PACK32:
      PutChannel_A2R10G10B10(Image_Channel_Swizzle(fmt_, channel_),
                             ptr_,
                             value_);
      break;
    case Image_Format_X8_D24_UNORM_PACK32: PutChannel_X8D24_UNORM(channel_, ptr_, value_);
      break;
    case Image_Format_D24_UNORM_S8_UINT: PutChannel_D24X8_UNORM(channel_, ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }
}

auto BitWidth24SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R8G8B8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8B8_SRGB: PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;

    case Image_Format_B8G8R8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8_USCALED:
    case Image_Format_B8G8R8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8_SSCALED:
    case Image_Format_B8G8R8_SINT: PutHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B8G8R8_SRGB: PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;

    case Image_Format_D16_UNORM_S8_UINT: PutChannel_D16S8_UNORM_UINT(channel_, ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }

}

auto BitWidth16SetChannelAt(enum Image_Channel const channel_,
                            enum Image_Format const fmt_,
                            uint8_t *ptr_,
                            double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R4G4B4A4_UNORM_PACK16:
      PutHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                  ptr_,
                                  value_);
      break;
    case Image_Format_B4G4R4A4_UNORM_PACK16:
      PutHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                  ptr_,
                                  value_);
      break;
    case Image_Format_R5G6B5_UNORM_PACK16: PutChannel_R5G6B5_UNORM(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_B5G6R5_UNORM_PACK16: PutChannel_R5G6B5_UNORM(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R5G5B5A1_UNORM_PACK16:
      PutChannel_R5G5B5A1_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                ptr_,
                                value_);
      break;
    case Image_Format_B5G5R5A1_UNORM_PACK16:
      PutChannel_R5G5B5A1_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                ptr_,
                                value_);
      break;
    case Image_Format_A1R5G5B5_UNORM_PACK16:
      PutChannel_A1R5G5B5_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                ptr_,
                                value_);
      break;

    case Image_Format_R8G8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SINT: PutHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8G8_SRGB: PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_D16_UNORM:
    case Image_Format_R16_UNORM: PutHomoChannel_NORM<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16_SNORM: PutHomoChannel_NORM<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16_UINT:
    case Image_Format_R16_USCALED: PutHomoChannel<uint16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16_SINT:
    case Image_Format_R16_SSCALED: PutHomoChannel<int16_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R16_SFLOAT: PutHomoChannel_FP16(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }

}

auto BitWidth8SetChannelAt(enum Image_Channel const channel_,
                           enum Image_Format const fmt_,
                           uint8_t *ptr_,
                           double const value_) -> void {
  switch (fmt_) {
    case Image_Format_R4G4_UNORM_PACK8:
      PutHomoChannel_nibble_UNORM(Image_Channel_Swizzle(fmt_, channel_),
                                  ptr_,
                                  value_);
      break;
    case Image_Format_R8_UNORM: PutHomoChannel_NORM<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8_SNORM: PutHomoChannel_NORM<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_S8_UINT:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_UINT: PutHomoChannel<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SINT: PutHomoChannel<int8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    case Image_Format_R8_SRGB: PutHomoChannel_sRGB<uint8_t>(Image_Channel_Swizzle(fmt_, channel_), ptr_, value_);
      break;
    default:LOGERRORF("%s not handled", Image_Format_Name(fmt_));
  }

}

} // end Image namespace

#endif //WYRD_PUT_HPP
