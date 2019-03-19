#pragma once
#ifndef WYRD_THEFORGE_IMAGEENUMS_HPP
#define WYRD_THEFORGE_IMAGEENUMS_HPP

#include "core/core.h"
#include "theforge/image_enums.h"

namespace TheForge {

static const auto R8 = TheForge_IF_R8;
static const auto RG8 = TheForge_IF_RG8;
static const auto RGB8 = TheForge_IF_RGB8;
static const auto RGBA8 = TheForge_IF_RGBA8;
static const auto R16 = TheForge_IF_R16;
static const auto RG16 = TheForge_IF_RG16;
static const auto RGB16 = TheForge_IF_RGB16;
static const auto RGBA16 = TheForge_IF_RGBA16;
static const auto R8S = TheForge_IF_R8S;
static const auto RG8S = TheForge_IF_RG8S;
static const auto RGB8S = TheForge_IF_RGB8S;
static const auto RGBA8S = TheForge_IF_RGBA8S;
static const auto R16S = TheForge_IF_R16S;
static const auto RG16S = TheForge_IF_RG16S;
static const auto RGB16S = TheForge_IF_RGB16S;
static const auto RGBA16S = TheForge_IF_RGBA16S;
static const auto R16F = TheForge_IF_R16F;
static const auto RG16F = TheForge_IF_RG16F;
static const auto RGB16F = TheForge_IF_RGB16F;
static const auto RGBA16F = TheForge_IF_RGBA16F;
static const auto R32F = TheForge_IF_R32F;
static const auto RG32F = TheForge_IF_RG32F;
static const auto RGB32F = TheForge_IF_RGB32F;
static const auto RGBA32F = TheForge_IF_RGBA32F;
static const auto R16I = TheForge_IF_R16I;
static const auto RG16I = TheForge_IF_RG16I;
static const auto RGB16I = TheForge_IF_RGB16I;
static const auto RGBA16I = TheForge_IF_RGBA16I;
static const auto R32I = TheForge_IF_R32I;
static const auto RG32I = TheForge_IF_RG32I;
static const auto RGB32I = TheForge_IF_RGB32I;
static const auto RGBA32I = TheForge_IF_RGBA32I;
static const auto R16UI = TheForge_IF_R16UI;
static const auto RG16UI = TheForge_IF_RG16UI;
static const auto RGB16UI = TheForge_IF_RGB16UI;
static const auto RGBA16UI = TheForge_IF_RGBA16UI;
static const auto R32UI = TheForge_IF_R32UI;
static const auto RG32UI = TheForge_IF_RG32UI;
static const auto RGB32UI = TheForge_IF_RGB32UI;
static const auto RGBA32UI = TheForge_IF_RGBA32UI;
static const auto RGBE8 = TheForge_IF_RGBE8;
static const auto RGB9E5 = TheForge_IF_RGB9E5;
static const auto RG11B10F = TheForge_IF_RG11B10F;
static const auto RGB565 = TheForge_IF_RGB565;
static const auto RGBA4 = TheForge_IF_RGBA4;
static const auto RGB10A2 = TheForge_IF_RGB10A2;
static const auto D16 = TheForge_IF_D16;
static const auto D24 = TheForge_IF_D24;
static const auto D24S8 = TheForge_IF_D24S8;
static const auto D32F = TheForge_IF_D32F;
static const auto DXT1 = TheForge_IF_DXT1;
static const auto DXT3 = TheForge_IF_DXT3;
static const auto DXT5 = TheForge_IF_DXT5;
static const auto ATI1N = TheForge_IF_ATI1N;
static const auto ATI2N = TheForge_IF_ATI2N;
static const auto PVR_2BPP = TheForge_IF_PVR_2BPP;
static const auto PVR_2BPPA = TheForge_IF_PVR_2BPPA;
static const auto PVR_4BPP = TheForge_IF_PVR_4BPP;
static const auto PVR_4BPPA = TheForge_IF_PVR_4BPPA;
static const auto ETC1 = TheForge_IF_ETC1;
static const auto ATC = TheForge_IF_ATC;
static const auto ATCA = TheForge_IF_ATCA;
static const auto ATCI = TheForge_IF_ATCI;
static const auto GNF_BC1 = TheForge_IF_GNF_BC1;
static const auto GNF_BC2 = TheForge_IF_GNF_BC2;
static const auto GNF_BC3 = TheForge_IF_GNF_BC3;
static const auto GNF_BC4 = TheForge_IF_GNF_BC4;
static const auto GNF_BC5 = TheForge_IF_GNF_BC5;
static const auto GNF_BC6 = TheForge_IF_GNF_BC6;
static const auto GNF_BC7 = TheForge_IF_GNF_BC7;
static const auto BGRA8 = TheForge_IF_BGRA8;
static const auto X8D24PAX32 = TheForge_IF_X8D24PAX32;
static const auto S8 = TheForge_IF_S8;
static const auto D16S8 = TheForge_IF_D16S8;
static const auto D32S8 = TheForge_IF_D32S8;
static const auto PVR_2BPP_SRGB = TheForge_IF_PVR_2BPP_SRGB;
static const auto PVR_2BPPA_SRGB = TheForge_IF_PVR_2BPPA_SRGB;
static const auto PVR_4BPP_SRGB = TheForge_IF_PVR_4BPP_SRGB;
static const auto PVR_4BPPA_SRGB = TheForge_IF_PVR_4BPPA_SRGB;
static const auto I8 = TheForge_IF_I8;
static const auto IA8 = TheForge_IF_IA8;
static const auto I16 = TheForge_IF_I16;
static const auto IA16 = TheForge_IF_IA16;
static const auto I16F = TheForge_IF_I16F;
static const auto IA16F = TheForge_IF_IA16F;
static const auto I32F = TheForge_IF_I32F;
static const auto IA32F = TheForge_IF_IA32F;

static const auto BLOCK_SIZE_1x1 = TheForge_BLOCK_SIZE_1x1;
static const auto BLOCK_SIZE_4x4 = TheForge_BLOCK_SIZE_4x4;
static const auto BLOCK_SIZE_4x8 = TheForge_BLOCK_SIZE_4x8;

typedef TheForge_ImageFormat ImageFormat;
typedef TheForge_BlockSize BlockSize;

inline bool IsPlainFormat(const ImageFormat format) {
  return TheForge_IsPlainFormat(format);
}
inline bool IsCompressedFormat(const ImageFormat format) {
  return TheForge_IsCompressedFormat(format);
}
inline bool IsFloatFormat(const ImageFormat format) {
  return TheForge_IsFloatFormat(format);
}
inline bool IsSignedFormat(const ImageFormat format) {
  return TheForge_IsSignedFormat(format);
}
inline bool IsStencilFormat(const ImageFormat format) {
  return TheForge_IsStencilFormat(format);
}
inline bool IsDepthFormat(const ImageFormat format) {
  return TheForge_IsDepthFormat(format);
}
inline bool IsPackedFormat(const ImageFormat format) {
  return TheForge_IsPackedFormat(format);
}
inline bool IsIntegerFormat(const ImageFormat format) {
  return TheForge_IsIntegerFormat(format);
}
inline int GetChannelCount(const ImageFormat format) {
  return TheForge_GetChannelCount(format);
}
inline int GetBytesPerChannel(const ImageFormat format) {
  return TheForge_GetBytesPerChannel(format);
}
inline int GetBytesPerPixel(const ImageFormat format) {
  return TheForge_GetBytesPerPixel(format);
}
inline int GetBytesPerBlock(const ImageFormat format) {
  return TheForge_IsPlainFormat(format);
}
inline TheForge_BlockSize GetBlockSize(const ImageFormat format) {
  return TheForge_GetBlockSize(format);
}

inline const char *GetImageFormatString(const ImageFormat format) {
  return TheForge_GetImageFormatString(format);
}

inline ImageFormat GetImageFormatFromString(char *string) {
  return GetImageFormatFromString(string);
}


} // end namespace

#endif //WYRD_IMAGEENUMS_HPP
