#pragma once
#ifndef WYRD_SYOYO_TINY_EXR_HPP
#define WYRD_SYOYO_TINY_EXR_HPP

#include "syoyo/tiny_exr.h"

namespace tinyexr {

using EXRVersion = TinyExr_EXRVersion;
using EXRAttribute = TinyExr_EXRAttribute;
using EXRChannelInfo = TinyExr_EXRChannelInfo;
using EXRTile = TinyExr_EXRTile;
using EXRHeader = TinyExr_EXRHeader;
using EXRMultiPartHeader = TinyExr_EXRMultiPartHeader;
using EXRImage = TinyExr_EXRImage;
using EXRMultiPartImage = TinyExr_EXRMultiPartImage;
using DeepImage = TinyExr_DeepImage;

inline void InitEXRHeader(EXRHeader *exr_header) {
  TinyExr_InitEXRHeader(exr_header);
}
inline void InitEXRImage(EXRImage *exr_image) {
  TinyExr_InitEXRImage(exr_image);
}
inline int FreeEXRHeader(EXRHeader *exr_header) {
  return TinyExr_FreeEXRHeader(exr_header);
}
inline int FreeEXRImage(EXRImage *exr_image) {
  return TinyExr_FreeEXRImage(exr_image);
}

inline int ParseEXRVersion(EXRVersion *version, VFile_Handle handle) {
  return TinyExr_ParseEXRVersion(version, handle);
}
inline int ParseEXRHeader(EXRHeader *header, const EXRVersion *version, VFile_Handle handle) {
  return TinyExr_ParseEXRHeader(header, version, handle);
}

inline int ParseEXRMultipartHeader(EXRHeader ***headers,
                                    int *num_headers,
                                    const EXRVersion *version,
                                    VFile_Handle handle) {
  return TinyExr_ParseEXRMultipartHeader(headers, num_headers, version, handle);
}
inline int LoadEXRImage(EXRImage *image, const EXRHeader *header,VFile_Handle handle) {
  return TinyExr_LoadEXRImage(image, header, handle);
}

inline int LoadEXRMultipartImage(EXRImage *images,
                                  const EXRHeader **headers,
                                  unsigned int num_parts,
                                 VFile_Handle handle) {
  return TinyExr_LoadEXRMultipartImage(images, headers, num_parts, handle);
}

inline int SaveEXRImage(const EXRImage *image,
                       const EXRHeader *exr_header, VFile_Handle handle) {
  return TinyExr_SaveEXRImage(image, exr_header, handle);
}

inline int LoadDeepEXR(TinyExr_DeepImage *out_image, VFile_Handle handle) {
  return TinyExr_LoadDeepEXR(out_image, handle);
}
}

#endif //WYRD_TINY_EXR_HPP
