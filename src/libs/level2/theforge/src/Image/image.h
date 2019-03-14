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

#ifndef COMMON_3_OS_IMAGE_IMAGE_H_
#define COMMON_3_OS_IMAGE_IMAGE_H_

#include "theforge/image_enums.hpp"
#include "tinystl/string.h"

#define ALL_MIPLEVELS 127

/************************************************************************************/
// Define some useful macros
#define MCHAR2(a, b) (a | (b << 8))
#define MAKE_CHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

/*************************************************************************************/
namespace TheForge {

typedef void *(*memoryAllocationFunc)(class Image *pImage, uint64_t memoryRequirement, void *pUserData);

class Image {
 public:
  Image();
  Image(const Image& img);

  unsigned char *
  Create(const ImageFormat fmt,
         const int w,
         const int h,
         const int d,
         const int mipMapCount,
         const int arraySize = 1);
  void RedefineDimensions(
      const ImageFormat fmt,
      const int w,
      const int h,
      const int d,
      const int mipMapCount,
      const int arraySize = 1);
  void Destroy();
  void Clear();

  unsigned char *GetPixels() const { return pData; }
  unsigned char *GetPixels(const uint32_t mipMapLevel) const;
  unsigned char *GetPixels(unsigned char *pDstData, const uint32_t mipMapLevel);
  unsigned char *GetPixels(const uint32_t mipMapLevel, const uint32_t arraySlice) const;

  void SetPixels(unsigned char *pixelData) {
    mOwnsMemory = false;
    pData = pixelData;
  }
  void SetName(const tinystl::string& name) { mLoadFileName = name; }

  uint32_t GetWidth() const { return mWidth; }
  uint32_t GetHeight() const { return mHeight; }
  uint32_t GetDepth() const { return mDepth; }
  uint32_t GetWidth(const int mipMapLevel) const;
  uint32_t GetHeight(const int mipMapLevel) const;
  uint32_t GetDepth(const int mipMapLevel) const;
  uint32_t GetMipMapCount() const { return mMipMapCount; }
  const tinystl::string& GetName() const { return mLoadFileName; }
  uint32_t GetMipMapCountFromDimensions() const;
  uint32_t GetArraySliceSize(const uint32_t mipMapLevel = 0, ImageFormat srcFormat = TheForge_IF_NONE) const;
  uint32_t GetNumberOfPixels(const uint32_t firstMipLevel = 0, uint32_t numMipLevels = ALL_MIPLEVELS) const;
  bool GetColorRange(float& min, float& max);
  bool Normalize();
  bool Uncompress();
  bool Unpack();

  bool Convert(const ImageFormat newFormat);
  bool GenerateMipMaps(const uint32_t mipMaps = ALL_MIPLEVELS);

  uint32_t GetArrayCount() const { return mArrayCount; }
  uint32_t GetMipMappedSize(
      const uint32_t firstMipLevel = 0,
      uint32_t numMipLevels = ALL_MIPLEVELS,
      ImageFormat srcFormat = TheForge_IF_NONE) const;

  ImageFormat getFormat() const { return mFormat; }

  void setFormat(const ImageFormat fmt) { mFormat = fmt; }
  bool Is1D() const { return (mDepth == 1 && mHeight == 1); }
  bool Is2D() const { return (mDepth == 1 && mHeight > 1); }
  bool Is3D() const { return (mDepth > 1); }
  bool IsArray() const { return (mArrayCount > 1); }
  bool IsCube() const { return (mDepth == 0); }
  bool IsRenderTarget() const { return mIsRendertarget; }

  // Image Format Loading from mData
  bool iLoadDDSFromMemory(
      const char *memory,
      uint32_t memsize,
      const bool useMipMaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
  bool iLoadPVRFromMemory(
      const char *memory,
      uint32_t memsize,
      const bool useMipmaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
  // #TODO: Implement this method
  //bool iLoadKTXFromMemory(const char* memory, uint32_t memsize, const bool useMipmaps, memoryAllocationFunc pAllocator = NULL, void* pUserData = NULL);
  bool iLoadSTBIFromMemory(
      const char *buffer,
      uint32_t memsize,
      const bool useMipmaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
  bool iLoadSTBIFP32FromMemory(
      const char *buffer,
      uint32_t memsize,
      const bool useMipmaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
  bool iLoadEXRFP32FromMemory(
      const char *buffer,
      uint32_t memsize,
      const bool useMipmaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
#if defined(ORBIS)
  bool iLoadGNFFromMemory(struct sce::Gnf::Header* outHeader, MemoryBuffer* mp);
#endif

  void loadFromMemoryXY(
      const void *mem,
      const int topLeftX,
      const int topLeftY,
      const int bottomRightX,
      const int bottomRightY,
      const int pitch);

  //load image
  bool loadImage(
      const char *fileName,
      bool useMipmaps,
      memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);
  bool loadFromMemory(
      void const *mem, uint32_t size, bool mipMapCount, char const *extension, memoryAllocationFunc pAllocator = NULL,
      void *pUserData = NULL);

  bool iSwap(const int c0, const int c1);

  // Image Format Saving
  bool iSaveDDS(const char *fileName);
  bool iSaveTGA(const char *fileName);
  bool iSaveBMP(const char *fileName);
  bool iSavePNG(const char *fileName);
  bool iSaveHDR(const char *fileName);
  bool iSaveJPG(const char *fileName);
  bool SaveImage(const char *fileName);

 protected:
  unsigned char *pData;
  tinystl::string mLoadFileName;
  uint32_t mWidth, mHeight, mDepth;
  uint32_t mMipMapCount;
  uint32_t mArrayCount;
  ImageFormat mFormat;
  int mAdditionalDataSize;
  unsigned char *pAdditionalData;
  bool mIsRendertarget;
  bool mOwnsMemory;

 public:
  typedef bool (Image::*ImageLoaderFunction)(
      const char *memory, uint32_t memSize, const bool useMipmaps, memoryAllocationFunc pAllocator, void *pUserData);
  static void AddImageLoader(const char *pExtension, ImageLoaderFunction pFunc);
};

static inline uint32_t CalculateImageFormatStride(ImageFormat format) {
  uint32_t result = 0;
  switch (format) {
    // 1 channel
    case R8: result = 1;
      break;
    case R16: result = 2;
      break;
    case R16F: result = 2;
      break;
    case R32UI: result = 4;
      break;
    case R32F: result = 4;
      break;
      // 2 channel
    case RG8: result = 2;
      break;
    case RG16: result = 4;
      break;
    case RG16F: result = 4;
      break;
    case RG32UI: result = 8;
      break;
    case RG32F: result = 8;
      break;
      // 3 channel
    case RGB8: result = 3;
      break;
    case RGB16: result = 6;
      break;
    case RGB16F: result = 6;
      break;
    case RGB32UI: result = 12;
      break;
    case RGB32F: result = 12;
      break;
      // 4 channel
    case BGRA8: result = 4;
      break;
    case RGBA8: result = 4;
      break;
    case RGBA16: result = 8;
      break;
    case RGBA16F: result = 8;
      break;
    case RGBA32UI: result = 16;
      break;
    case RGBA32F: result = 16;
      break;
      // Depth/stencil
    case D16: result = 0;
      break;
    case X8D24PAX32: result = 0;
      break;
    case D32F: result = 0;
      break;
    case S8: result = 0;
      break;
    case D16S8: result = 0;
      break;
    case D24S8: result = 0;
      break;
    case D32S8: result = 0;
      break;
    default: break;
  }
  return result;
}

static inline uint32_t CalculateImageFormatChannelCount(ImageFormat format) {
  //  uint32_t result = 0;
  if (format == BGRA8) {
    return 3;
  } else {
    return GetChannelCount(format);
  }
}

static inline uint32_t CalculateMipMapLevels(uint32_t width, uint32_t height) {
  if (width == 0 || height == 0) {
    return 0;
  }

  uint32_t result = 1;
  while (width > 1 || height > 1) {
    width >>= 1;
    height >>= 1;
    result++;
  }
  return result;
}

} // end namespace

#endif    // COMMON_3_OS_IMAGE_IMAGE_H_
