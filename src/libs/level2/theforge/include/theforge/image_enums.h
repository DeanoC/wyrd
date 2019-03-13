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

#pragma once

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

  //http://aras-p.info/texts/D3D9GPUHacks.html
  TheForge_IF_INTZ = 60,    //  NVidia hack. Supported on all DX10+ HW

  // compressed mobile forms
  TheForge_IF_ETC1 = 65,    //  RGB
  TheForge_IF_ATC = 66,     //  RGB
  TheForge_IF_ATCA = 67,    //  RGBA, explicit alpha
  TheForge_IF_ATCI = 68,    //  RGBA, interpolated alpha

  //http://aras-p.info/texts/D3D9GPUHacks.html
  TheForge_IF_RAWZ = 69,           //depth only, Nvidia (requires recombination of data) //FIX IT: PS3 as well?
  TheForge_IF_DF16 = 70,           //depth only, Intel/AMD
  TheForge_IF_STENCILONLY = 71,    // stencil ony usage

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

enum TheForge_Image_BlockSize {
  TheForge_BLOCK_SIZE_1x1,
  TheForge_BLOCK_SIZE_4x4,
  TheForge_BLOCK_SIZE_4x8,
};
