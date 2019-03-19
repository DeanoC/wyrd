#pragma once
#include "core/core.h"
#include "core/logger.h"
#ifndef WYRD_THEFORGE_IMAGE_ENUMS_H
#define WYRD_THEFORGE_IMAGE_ENUMS_H

#if defined(ORBIS)
// Indicates the result of a GNF load operation
enum GnfError
{
    kGnfErrorNone = 0,                     // Operation was successful; no error
    kGnfErrorInvalidPointer = -1,          // Caller passed an invalid/NULL pointer to a GNF loader function
    kGnfErrorNotGnfFile = -2,              // Attempted to load a file that isn't a GNF file (bad magic number in header)
    kGnfErrorCorruptHeader = -3,           // Attempted to load a GNF file with corrupt header data
    kGnfErrorFileIsTooShort = -4,          // Attempted to load a GNF file whose size is smaller than the size reported in its header
    kGnfErrorVersionMismatch = -5,         // Attempted to load a GNF file created by a different version of the GNF code
    kGnfErrorAlignmentOutOfRange = -6,     // Attempted to load a GNF file with corrupt header data (surface alignment > 2^31 bytes)
    kGnfErrorContentsSizeMismatch = -7,    // Attempted to load a GNF file with corrupt header data (wrong size in GNF header contents)
    kGnfErrorCouldNotOpenFile = -8,        // Unable to open a file for reading
    kGnfErrorOutOfMemory = -9,             // Internal memory allocation failed
};
#endif
enum {
  TheForge_IF_NONE = 0,

  // Unsigned formats
  TheForge_IF_R8 = 1,
  TheForge_IF_RG8 = 2,
  TheForge_IF_RGB8 = 3,
  TheForge_IF_RGBA8 = 4,

  TheForge_IF_R16 = 5,
  TheForge_IF_RG16 = 6,
  TheForge_IF_RGB16 = 7,
  TheForge_IF_RGBA16 = 8,

  // Signed formats
      TheForge_IF_R8S = 9,
  TheForge_IF_RG8S = 10,
  TheForge_IF_RGB8S = 11,
  TheForge_IF_RGBA8S = 12,

  TheForge_IF_R16S = 13,
  TheForge_IF_RG16S = 14,
  TheForge_IF_RGB16S = 15,
  TheForge_IF_RGBA16S = 16,

  // Float formats
      TheForge_IF_R16F = 17,
  TheForge_IF_RG16F = 18,
  TheForge_IF_RGB16F = 19,
  TheForge_IF_RGBA16F = 20,

  TheForge_IF_R32F = 21,
  TheForge_IF_RG32F = 22,
  TheForge_IF_RGB32F = 23,
  TheForge_IF_RGBA32F = 24,

  // Signed integer formats
      TheForge_IF_R16I = 25,
  TheForge_IF_RG16I = 26,
  TheForge_IF_RGB16I = 27,
  TheForge_IF_RGBA16I = 28,

  TheForge_IF_R32I = 29,
  TheForge_IF_RG32I = 30,
  TheForge_IF_RGB32I = 31,
  TheForge_IF_RGBA32I = 32,

  // Unsigned integer formats
      TheForge_IF_R16UI = 33,
  TheForge_IF_RG16UI = 34,
  TheForge_IF_RGB16UI = 35,
  TheForge_IF_RGBA16UI = 36,

  TheForge_IF_R32UI = 37,
  TheForge_IF_RG32UI = 38,
  TheForge_IF_RGB32UI = 39,
  TheForge_IF_RGBA32UI = 40,

  // Packed formats
  TheForge_IF_RGBE8 = 41,
  TheForge_IF_RGB9E5 = 42,
  TheForge_IF_RG11B10F = 43,
  TheForge_IF_RGB565 = 44,
  TheForge_IF_RGBA4 = 45,
  TheForge_IF_RGB10A2 = 46,

  // Depth formats
  TheForge_IF_D16 = 47,
  TheForge_IF_D24 = 48,
  TheForge_IF_D24S8 = 49,
  TheForge_IF_D32F = 50,

  // Compressed formats
  TheForge_IF_DXT1 = 51,
  TheForge_IF_DXT3 = 52,
  TheForge_IF_DXT5 = 53,
  TheForge_IF_ATI1N = 54,
  TheForge_IF_ATI2N = 55,

  // PVR formats
  TheForge_IF_PVR_2BPP = 56,
  TheForge_IF_PVR_2BPPA = 57,
  TheForge_IF_PVR_4BPP = 58,
  TheForge_IF_PVR_4BPPA = 59,

  // compressed mobile forms
  TheForge_IF_ETC1 = 65,    //  RGB
  TheForge_IF_ATC = 66,     //  RGB
  TheForge_IF_ATCA = 67,    //  RGBA, explicit alpha
  TheForge_IF_ATCI = 68,    //  RGBA, interpolated alpha

  // BC1 == DXT1
  // BC2 == DXT2
  // BC3 == DXT4 / 5
  // BC4 == ATI1 == One color channel (8 bits)
  // BC5 == ATI2 == Two color channels (8 bits:8 bits)
  // BC6 == Three color channels (16 bits:16 bits:16 bits) in "half" floating point*
  // BC7 == Three color channels (4 to 7 bits per channel) with 0 to 8 bits of alpha
  TheForge_IF_GNF_BC1 = 72,
  TheForge_IF_GNF_BC2 = 73,
  TheForge_IF_GNF_BC3 = 74,
  TheForge_IF_GNF_BC4 = 75,
  TheForge_IF_GNF_BC5 = 76,
  TheForge_IF_GNF_BC6 = 77,
  TheForge_IF_GNF_BC7 = 78,
  // Reveser Form
  TheForge_IF_BGRA8 = 79,

  // Extend for DXGI
  TheForge_IF_X8D24PAX32 = 80,
  TheForge_IF_S8 = 81,
  TheForge_IF_D16S8 = 82,
  TheForge_IF_D32S8 = 83,

  // PVR SRGB extensions
  TheForge_IF_PVR_2BPP_SRGB = 84,
  TheForge_IF_PVR_2BPPA_SRGB = 85,
  TheForge_IF_PVR_4BPP_SRGB = 86,
  TheForge_IF_PVR_4BPPA_SRGB = 87,

  // Count identifier - not actually a format.
  TheForge_IF_COUNT,

  // Aliases
  TheForge_IF_I8 = TheForge_IF_R8,
  TheForge_IF_IA8 = TheForge_IF_RG8,
  TheForge_IF_I16 = TheForge_IF_R16,
  TheForge_IF_IA16 = TheForge_IF_RG16,
  TheForge_IF_I16F = TheForge_IF_R16F,
  TheForge_IF_IA16F = TheForge_IF_RG16F,
  TheForge_IF_I32F = TheForge_IF_R32F,
  TheForge_IF_IA32F = TheForge_IF_RG32F
};
typedef uint32_t TheForge_ImageFormat;

enum {
  TheForge_BLOCK_SIZE_1x1,
  TheForge_BLOCK_SIZE_4x4,
  TheForge_BLOCK_SIZE_4x8,
};
typedef uint8_t TheForge_BlockSize;

EXTERN_C inline bool TheForge_IsIntegerFormat(const TheForge_ImageFormat format) {
  return (format >= TheForge_IF_R16I && format <= TheForge_IF_RGBA32UI);
}

EXTERN_C inline bool TheForge_IsCompressedFormat(const TheForge_ImageFormat format) {
  return (
      ((format >= TheForge_IF_DXT1) && (format <= TheForge_IF_PVR_4BPPA)) ||
          ((format >= TheForge_IF_PVR_2BPP_SRGB) && (format <= TheForge_IF_PVR_4BPPA_SRGB)) ||
          ((format >= TheForge_IF_ETC1) && (format <= TheForge_IF_ATCI)) ||
          ((format >= TheForge_IF_GNF_BC1) && (format <= TheForge_IF_GNF_BC7)));
}

EXTERN_C inline bool TheForge_IsFloatFormat(const TheForge_ImageFormat format) {
  //	return (format >= R16F && format <= RGBA32F);
  return (format >= TheForge_IF_R16F && format <= TheForge_IF_RG11B10F) || (format == TheForge_IF_D32F);
}

EXTERN_C inline bool TheForge_IsSignedFormat(const TheForge_ImageFormat format) {
  return ((format >= TheForge_IF_R8S) && (format <= TheForge_IF_RGBA16S)) ||
      ((format >= TheForge_IF_R16I) && (format <= TheForge_IF_RGBA32I));
}

EXTERN_C inline bool TheForge_IsStencilFormat(const TheForge_ImageFormat format) {
  return (format == TheForge_IF_D24S8) || (format >= TheForge_IF_X8D24PAX32 && format <= TheForge_IF_D32S8);
}

EXTERN_C inline bool TheForge_IsDepthFormat(const TheForge_ImageFormat format) {
  return (format >= TheForge_IF_D16 && format <= TheForge_IF_D32F) || (format == TheForge_IF_X8D24PAX32)
      || (format == TheForge_IF_D16S8) || (format == TheForge_IF_D32S8);
}

EXTERN_C inline bool TheForge_IsPackedFormat(const TheForge_ImageFormat format) {
  return (format >= TheForge_IF_RGBE8 && format <= TheForge_IF_RGB10A2);
}

EXTERN_C inline bool TheForge_IsPlainFormat(const TheForge_ImageFormat format) {
  return (format <= TheForge_IF_RGBA32UI) || (format == TheForge_IF_BGRA8);
}

EXTERN_C inline int TheForge_GetBytesPerPixel(const TheForge_ImageFormat format) {
  // Does not accept compressed formats

  static const int bytesPP[] = {
      0, 1, 2, 3, 4,       //  8-bit unsigned
      2, 4, 6, 8,           // 16-bit unsigned
      1, 2, 3, 4,           //  8-bit signed
      2, 4, 6, 8,           // 16-bit signed
      2, 4, 6, 8,           // 16-bit float
      4, 8, 12, 16,          // 32-bit float
      2, 4, 6, 8,           // 16-bit unsigned integer
      4, 8, 12, 16,          // 32-bit unsigned integer
      2, 4, 6, 8,           // 16-bit signed integer
      4, 8, 12, 16,          // 32-bit signed integer
      4, 4, 4, 2, 2, 4,    // Packed
      2, 4, 4, 4,           // Depth
  };

  if (format == TheForge_IF_BGRA8) {
    return 4;
  }

  ASSERT(format <= TheForge_IF_D32F || format == TheForge_IF_BGRA8 || format == TheForge_IF_D32S8);

  return bytesPP[format];
}

EXTERN_C inline int TheForge_GetBytesPerBlock(const TheForge_ImageFormat format) {
  ASSERT(TheForge_IsCompressedFormat(format));
  switch (format) {
    // BC1 == DXT1
    // BC2 == DXT2
    // BC3 == DXT4 / 5
    // BC4 == ATI1 == One color channel (8 bits)
    // BC5 == ATI2 == Two color channels (8 bits:8 bits)
    // BC6 == Three color channels (16 bits:16 bits:16 bits) in "half" floating point*
    // BC7 == Three color channels (4 to 7 bits per channel) with 0 to 8 bits of alpha
    case TheForge_IF_DXT1:         //  4x4
    case TheForge_IF_ATI1N:        //  4x4
    case TheForge_IF_GNF_BC1:      //  4x4
    case TheForge_IF_ETC1:         //  4x4
    case TheForge_IF_ATC:          //  4x4
    case TheForge_IF_PVR_4BPP:     //  4x4
    case TheForge_IF_PVR_4BPPA:    //  4x4
    case TheForge_IF_PVR_2BPP:     //  4x8
    case TheForge_IF_PVR_2BPPA:    //  4x8
    case TheForge_IF_PVR_4BPP_SRGB:     //  4x4
    case TheForge_IF_PVR_4BPPA_SRGB:    //  4x4
    case TheForge_IF_PVR_2BPP_SRGB:     //  4x8
    case TheForge_IF_PVR_2BPPA_SRGB:    //  4x8
      return 8;

    case TheForge_IF_DXT3:       //  4x4
    case TheForge_IF_DXT5:       //  4x4
    case TheForge_IF_GNF_BC3:    //  4x4
    case TheForge_IF_GNF_BC5:    //  4x4
    case TheForge_IF_ATI2N:      //  4x4
    case TheForge_IF_ATCA:       //  4x4
    case TheForge_IF_ATCI:       //  4x4
    case TheForge_IF_GNF_BC6:    //  4x4
    case TheForge_IF_GNF_BC7:    //  4x4
      return 16;

    default: return 0;
  }
}

EXTERN_C inline int TheForge_GetBytesPerChannel(const TheForge_ImageFormat format) {
  // Accepts only plain formats
  static const int bytesPC[] = {
      1,    //  8-bit unsigned
      2,    // 16-bit unsigned
      1,    //  8-bit signed
      2,    // 16-bit signed
      2,    // 16-bit float
      4,    // 32-bit float
      2,    // 16-bit unsigned integer
      4,    // 32-bit unsigned integer
      2,    // 16-bit signed integer
      4,    // 32-bit signed integer
  };

  ASSERT(format <= TheForge_IF_RGBA32UI);

  return bytesPC[(format - 1) >> 2];
}

EXTERN_C inline TheForge_BlockSize TheForge_GetBlockSize(const TheForge_ImageFormat format) {
  switch (format) {
    case TheForge_IF_PVR_2BPP_SRGB:     //  4x8
    case TheForge_IF_PVR_2BPPA_SRGB:    //  4x8
    case TheForge_IF_PVR_2BPP:          //  4x8
    case TheForge_IF_PVR_2BPPA:         //  4x8
      return TheForge_BLOCK_SIZE_4x8;

    case TheForge_IF_DXT1:         //  4x4
    case TheForge_IF_ATI1N:        //  4x4
    case TheForge_IF_GNF_BC1:      //  4x4
    case TheForge_IF_ETC1:         //  4x4
    case TheForge_IF_ATC:          //  4x4
    case TheForge_IF_PVR_4BPP:     //  4x4
    case TheForge_IF_PVR_4BPPA:    //  4x4
    case TheForge_IF_DXT3:         //  4x4
    case TheForge_IF_DXT5:         //  4x4
    case TheForge_IF_GNF_BC3:      //  4x4
    case TheForge_IF_GNF_BC5:      //  4x4
    case TheForge_IF_ATI2N:        //  4x4
    case TheForge_IF_ATCA:         //  4x4
    case TheForge_IF_ATCI:         //  4x4
    case TheForge_IF_GNF_BC6:    //  4x4
    case TheForge_IF_GNF_BC7:    //  4x4
      return TheForge_BLOCK_SIZE_4x4;

    default: return TheForge_BLOCK_SIZE_1x1;
  }
}
EXTERN_C const char *TheForge_GetImageFormatString(TheForge_ImageFormat format);
EXTERN_C TheForge_ImageFormat TheForge_GetFormatFromString(char const *string);

EXTERN_C inline int TheForge_GetChannelCount(const TheForge_ImageFormat format)
{
  // #REMOVE
  if (format == TheForge_IF_BGRA8)
    return 4;

  static const int channelCount[] = {
      0, 1, 2, 3, 4,         //  8-bit unsigned
      1, 2, 3, 4,            // 16-bit unsigned
      1, 2, 3, 4,            //  8-bit signed
      1, 2, 3, 4,            // 16-bit signed
      1, 2, 3, 4,            // 16-bit float
      1, 2, 3, 4,            // 32-bit float
      1, 2, 3, 4,            // 16-bit signed integer
      1, 2, 3, 4,            // 32-bit signed integer
      1, 2, 3, 4,            // 16-bit unsigned integer
      1, 2, 3, 4,            // 32-bit unsigned integer
      3, 3, 3, 3, 4, 4,      // Packed
      1, 1, 2, 1,            // Depth
      3, 4, 4, 1, 2,         // Compressed
      3, 4, 3, 4,            // PVR
      1,                     //  INTZ
      3, 4, 3, 4,            //  XBox front buffer formats
      3, 3, 4, 4,            //  ETC, ATC
      1, 1,                  //  RAWZ, DF16
      3, 4, 4, 1, 2, 3, 3,   // GNF_BC1~GNF_BC7
      3, 4, 3, 4,            // PVR sRGB
  };

  if (format >= sizeof(channelCount) / sizeof(int))
  {
    LOGERRORF("Fail to find Channel in format : %s", TheForge_GetImageFormatString(format));
    return 0;
  }

  return channelCount[format];
}

#endif // WYRD_THEFORGE_IMAGE_ENUMS_H
/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

