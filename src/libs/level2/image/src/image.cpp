#include "core/core.h"
#include "core/logger.h"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"
#include "hq_resample.hpp"

EXTERN_C Image_ImageHeader *Image_Create(uint32_t width,
                                         uint32_t height,
                                         uint32_t depth,
                                         uint32_t slices,
                                         enum Image_Format format) {
  Image_ImageHeader *image = Image_CreateNoClear(width, height, depth, slices, format);
  if (image) {
    memset(image + 1, 0, image->dataSize);
  }

  return image;
}

EXTERN_C Image_ImageHeader *Image_CreateNoClear(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format) {
  // block compression can't be less than 4x4
  if ((width < 4 || height < 4) && Image_Format_IsCompressed(format)) {
    return nullptr;
  }

  uint64_t const dataSize = (width *
      height *
      depth *
      slices *
      Image_Format_BitWidth(format)) / 8;

  auto *image = (Image_ImageHeader *) malloc(sizeof(Image_ImageHeader) + dataSize);
  image->dataSize = dataSize;
  image->width = width;
  image->height = height;
  image->depth = depth;
  image->slices = slices;
  image->format = format;
  image->flags = 0;
  image->nextType = Image_IT_None;
  image->nextImage = nullptr;

  return image;

}

EXTERN_C void Image_Destroy(Image_ImageHeader *image) {
  // recursively free next chain
  switch (image->nextType) {
    case Image_IT_MipMaps:
    case IMAGE_IT_Layers: {
      if(image->nextImage != nullptr) {
        Image_Destroy(image->nextImage);
      }
    } break;
    default:
    case Image_IT_None:
      break;
  }
  free(image);
}

EXTERN_C Image_ImageHeader *Image_Create1D(uint32_t width, enum Image_Format format) {
  return Image_Create(width, 1, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DNoClear(uint32_t width, enum Image_Format format) {
  return Image_CreateNoClear(width, 1, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format format) {
  return Image_Create(width, 1, 1, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format format) {
  return Image_CreateNoClear(width, 1, 1, slices, format);
}

EXTERN_C Image_ImageHeader *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format format) {
  return Image_Create(width, height, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t slices,
                                                enum Image_Format format) {
  return Image_Create(width, height, 1, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t slices,
                                                       enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, slices, format);
}

EXTERN_C Image_ImageHeader *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format format) {
  return Image_Create(width, height, depth, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DNoClear(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t depth,
                                                  enum Image_Format format) {
  return Image_CreateNoClear(width, height, depth, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format) {
  return Image_Create(width, height, depth, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t depth,
                                                       uint32_t slices,
                                                       enum Image_Format format) {
  return Image_CreateNoClear(width, height, depth, slices, format);
}

EXTERN_C Image_ImageHeader *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format format) {
  Image_ImageHeader* image = Image_Create(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;}
EXTERN_C Image_ImageHeader *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format format) {
  Image_ImageHeader* image = Image_CreateNoClear(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArray(uint32_t width,
                                                     uint32_t height,
                                                     uint32_t slices,
                                                     enum Image_Format format) {
  Image_ImageHeader* image = Image_Create(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                            uint32_t height,
                                                            uint32_t slices,
                                                            enum Image_Format format) {
  Image_ImageHeader* image = Image_CreateNoClear(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}


// we include fetch after swizzle so hopefully the compiler will inline it...
EXTERN_C inline enum Image_Channel Image_Channel_Swizzle(enum Image_Format format, enum Image_Channel channel) {
  Image_Swizzle swizzler = Image_Format_Swizzle(format);
  return (enum Image_Channel) swizzler[channel];
}

#include "fetch.hpp"
#include "put.hpp"

EXTERN_C double Image_GetChannelAt(Image_ImageHeader const *image, enum Image_Channel channel, size_t index) {
  ASSERT(image);

  using namespace Image;
  // seperate out the block compressed format first
  if (Image_Format_IsCompressed(image->format)) {
    return Image::CompressedChannelAt(image, channel, index);
  }

  // split into bit width grouped formats
  ASSERT(Image_Format_BitWidth(image->format) >= 8);

  uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
      index * (Image_Format_BitWidth(image->format) / 8);

  switch (Image_Format_BitWidth(image->format)) {
    case 256:return BitWidth256ChannelAt(channel, image->format, pixelPtr);
    case 192:return BitWidth192ChannelAt(channel, image->format, pixelPtr);
    case 128:return BitWidth128ChannelAt(channel, image->format, pixelPtr);
    case 96:return BitWidth96ChannelAt(channel, image->format, pixelPtr);
    case 64:return BitWidth64ChannelAt(channel, image->format, pixelPtr);
    case 48:return BitWidth48ChannelAt(channel, image->format, pixelPtr);
    case 32:return BitWidth32ChannelAt(channel, image->format, pixelPtr);
    case 24:return BitWidth24ChannelAt(channel, image->format, pixelPtr);
    case 16:return BitWidth16ChannelAt(channel, image->format, pixelPtr);
    case 8:return BitWidth8ChannelAt(channel, image->format, pixelPtr);
    default:LOGERROR("Bitwidth of format not supported");
      return 0.0;
  }

}
EXTERN_C size_t Image_LinkedCountOf(Image_ImageHeader const *image) {
  size_t count = 1;

  while(image && image->nextImage != nullptr) {
    count++;
    image = image->nextImage;
  }

  return count;
}

EXTERN_C Image_ImageHeader const* Image_LinkedImageOf(Image_ImageHeader const* image, size_t const index) {
  size_t count = 0;
  while(image && image->nextImage != nullptr) {
    if(count == index) {
      return image;
    }
    count++;
    image = image->nextImage;
  }

  return nullptr;
}

EXTERN_C void Image_SetChannelAt(Image_ImageHeader const *image,
                                 enum Image_Channel channel,
                                 size_t index,
                                 double value) {
  using namespace Image;

  // block compressed not handled ye
  ASSERT(!Image_Format_IsCompressed(image->format));

  // split into bit width grouped formats
  uint32_t const pixelSize = Image_Format_BitWidth(image->format);
  ASSERT(pixelSize >= 8);
  uint8_t *pixelPtr = (uint8_t *) Image_RawDataPtr(image) + (index * pixelSize / 8);

  switch (pixelSize) {
    case 256:BitWidth256SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 192:BitWidth192SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 128:BitWidth128SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 96:BitWidth96SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 64:BitWidth64SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 48:BitWidth48SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 32:BitWidth32SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 24:BitWidth24SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 16:BitWidth16SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    case 8:BitWidth8SetChannelAt(channel, image->format, pixelPtr, value);
      break;
    default:LOGERRORF("Bitwidth %i from %s not supported",
                      Image_Format_BitWidth(image->format),
                      Image_Format_Name(image->format));
  }

}

EXTERN_C void Image_GetPixelAt(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  // intentional fallthrough on this switch statement
  switch (Image_Format_ChannelCount(image->format)) {
    case 4:pixel->a = Image_GetChannelAt(image, Image_Alpha, index);
    case 3:pixel->b = Image_GetChannelAt(image, Image_Blue, index);
    case 2:pixel->g = Image_GetChannelAt(image, Image_Green, index);
    case 1:pixel->r = Image_GetChannelAt(image, Image_Red, index);
      break;
    default:ASSERT(Image_Format_ChannelCount(image->format) <= 4);
      break;
  }
}

EXTERN_C void Image_SetPixelAt(Image_ImageHeader const *image, Image_PixelD const *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  // intentional fallthrough on this switch statement
  switch (Image_Format_ChannelCount(image->format)) {
    case 4: Image_SetChannelAt(image, Image_Alpha, index, pixel->a);
    case 3: Image_SetChannelAt(image, Image_Blue, index, pixel->b);
    case 2: Image_SetChannelAt(image, Image_Green, index, pixel->g);
    case 1: Image_SetChannelAt(image, Image_Red, index, pixel->r);
      break;
    default:ASSERT(Image_Format_ChannelCount(image->format) <= 4);
      break;
  }
}

EXTERN_C void Image_CopyImage(Image_ImageHeader const *dst,
                              Image_ImageHeader const *src) {
  if (src == dst) { return; }

  ASSERT(dst->slices == src->slices);
  ASSERT(dst->depth == src->depth);
  ASSERT(dst->height == src->height);
  ASSERT(dst->width == src->width);

  for (auto w = 0u; w < src->slices; ++w) {
    for (auto z = 0u; z < src->depth; ++z) {
      for (auto y = 0u; y < src->height; ++y) {
        for (auto x = 0u; x < src->width; ++x) {
          size_t const index = Image_CalculateIndex(src, x, y, z, w);
          Image_PixelD pixel;
          Image_GetPixelAt(src, &pixel, index);
          Image_SetPixelAt(dst, &pixel, index);
        }
      }
    }
  }
}
EXTERN_C void Image_CopySlice(Image_ImageHeader const *dst,
                              uint32_t dw,
                              Image_ImageHeader const *src,
                              uint32_t sw) {
  ASSERT(dst->depth == src->depth);
  ASSERT(dst->height == src->height);
  ASSERT(dst->width == src->width);
  if (dst == src) {
    ASSERT(dw != sw);
  }

  for (auto z = 0u; z < src->depth; ++z) {
    for (auto y = 0u; y < src->height; ++y) {
      for (auto x = 0u; x < src->width; ++x) {
        size_t const srcIndex = Image_CalculateIndex(src, x, y, z, sw);
        size_t const dstIndex = Image_CalculateIndex(src, x, y, z, dw);
        Image_PixelD pixel;
        Image_GetPixelAt(src, &pixel, srcIndex);
        Image_SetPixelAt(dst, &pixel, dstIndex);
      }
    }
  }
}

EXTERN_C void Image_CopyPage(Image_ImageHeader const *dst,
                             uint32_t dz, uint32_t dw,
                             Image_ImageHeader const *src,
                             uint32_t sz, uint32_t sw) {
  ASSERT(dst->height == src->height);
  ASSERT(dst->width == src->width);
  if (dst == src) {
    ASSERT(dz != sz || dw != sw);
  }

  for (auto y = 0u; y < src->height; ++y) {
    for (auto x = 0u; x < src->width; ++x) {
      size_t const srcIndex = Image_CalculateIndex(src, x, y, sz, sw);
      size_t const dstIndex = Image_CalculateIndex(src, x, y, dz, dw);
      Image_PixelD pixel;
      Image_GetPixelAt(src, &pixel, srcIndex);
      Image_SetPixelAt(dst, &pixel, dstIndex);
    }
  }
}

EXTERN_C void Image_CopyRow(Image_ImageHeader *dst,
                            uint32_t dy, uint32_t dz, uint32_t dw,
                            Image_ImageHeader const *src,
                            uint32_t sy, uint32_t sz, uint32_t sw) {
  ASSERT(dst->width == src->width);
  if (dst == src) {
    ASSERT(dy != sy || dz != sz || dw != sw);
  }

  for (auto x = 0u; x < src->width; ++x) {
    size_t const srcIndex = Image_CalculateIndex(src, x, sy, sz, sw);
    size_t const dstIndex = Image_CalculateIndex(src, x, dy, dz, dw);
    Image_PixelD pixel;
    Image_GetPixelAt(src, &pixel, srcIndex);
    Image_SetPixelAt(dst, &pixel, dstIndex);
  }
}

EXTERN_C void Image_CopyPixel(Image_ImageHeader *dst,
                              uint32_t dx, uint32_t dy, uint32_t dz, uint32_t dw,
                              Image_ImageHeader const *src,
                              uint32_t sx, uint32_t sy, uint32_t sz, uint32_t sw) {
  size_t const srcIndex = Image_CalculateIndex(src, sx, sy, sz, sw);
  size_t const dstIndex = Image_CalculateIndex(src, dx, dy, dz, dw);
  Image_PixelD pixel;
  Image_GetPixelAt(src, &pixel, srcIndex);
  Image_SetPixelAt(dst, &pixel, dstIndex);
}


// TODO optimise or have option for faster mipmap chain generation
EXTERN_C void Image_CreateMipMapChain(Image_ImageHeader *image, bool generateFromImage) {
  // start from the image provided and create successive mip images
  ASSERT(image->nextType == Image_IT_None);
  ASSERT(Math_IsPowerOf2U32(image->width));
  ASSERT(Math_IsPowerOf2U32(image->height));
  // need to think about mip mapped volume textures...
  ASSERT(image->depth == 1);

  using namespace Image;

  Image_ImageHeader *curImage = image;
  uint32_t curWidth = image->width;
  uint32_t curHeight = image->height;
  if (curWidth <= 1 || curHeight <= 1) { return; }

  Image_ImageHeader *doubleImage = nullptr;
  Image_ImageHeader *scratchImage = nullptr;

  uint32_t const numChans = Image_Format_ChannelCount(image->format);
  if (generateFromImage) {
    Image_Format dblFmt = Image_Format_R32G32B32A32_SFLOAT;
    switch (numChans) {
      case 1:dblFmt = Image_Format_R32_SFLOAT;
        break;
      case 2:dblFmt = Image_Format_R32G32_SFLOAT;
        break;
      case 3:dblFmt = Image_Format_R32G32B32_SFLOAT;
        break;
      case 4:dblFmt = Image_Format_R32G32B32A32_SFLOAT;
        break;
      default:
      case 0: {
        ASSERT(false);
      }
    }
    doubleImage = Image_Create(image->width, image->height,
                               image->depth, image->slices,
                               dblFmt);
    Image_CopyImage(doubleImage, image);

    scratchImage = Image_Create(image->width / 2, image->height / 2,
                                1, 1, dblFmt);
  }

  do {
    curWidth = curWidth / 2;
    curHeight = curHeight / 2;

    Image_ImageHeader *newImage = Image_Create(curWidth, curHeight, 1, image->slices, image->format);

    if (generateFromImage) {
      float *const scratch = (float *const) Image_RawDataPtr(scratchImage);

      for (auto w = 0u; w < image->slices; ++w) {
        float const *origSlice = (float const *)
            (((uint8_t *) Image_RawDataPtr(image)) + w * Image_ByteCountPerSlice(image));

        hq_resample<float>(numChans,
                           origSlice, image->width, image->height,
                           scratch, curWidth, curHeight);

        Image_CopySlice(newImage, w, scratchImage, 0);
      }
    }

    curImage->nextImage = newImage;
    curImage->nextType = Image_IT_MipMaps;
    curImage = newImage;
  } while (curWidth > 1 || curHeight > 1);

  if (doubleImage) {
    Image_Destroy(doubleImage);
  }
  if (scratchImage) {
    Image_Destroy(scratchImage);
  }
}
/*

bool Image::Convert(const ImageFormat newFormat) {
  uint8_t *newPixels;
  uint32_t nPixels = GetNumberOfPixels(0, mMipMapCount) * mArrayCount;

  if (mFormat == RGBE8 && (newFormat == RGB32F || newFormat == RGBA32F)) {
    newPixels = (uint8_t *) malloc(sizeof(uint8_t) * GetMipMappedSize(0, mMipMapCount, newFormat) * mArrayCount);
    float *dest = (float *) newPixels;

    bool writeAlpha = (newFormat == RGBA32F);
    uint8_t *src = pData;
    do {
      *((vec3_t *) dest) = Math_RGBEToRGB(src);
      if (writeAlpha) {
        dest[3] = 1.0f;
        dest += 4;
      } else {
        dest += 3;
      }
      src += 4;
    } while (--nPixels);
  } else {
    if (!IsPlainFormat(mFormat) || !(IsPlainFormat(newFormat) || newFormat == RGB10A2 ||
        newFormat == RGBE8 || newFormat == RGB9E5)) {
      LOGERRORF(
          "Image: %s fail to convert from  %s  to  %s", mLoadFileName.c_str(), GetImageFormatString(mFormat),
          GetImageFormatString(newFormat));
      return false;
    }
    if (mFormat == newFormat) {
      return true;
    }

    uint8_t *src = pData;
    uint8_t *dest = newPixels = (uint8_t *) malloc(
        sizeof(uint8_t) * GetMipMappedSize(0, mMipMapCount, newFormat) * mArrayCount);

    if (mFormat == RGB8 && newFormat == RGBA8) {
      // Fast path for RGB->RGBA8
      do {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = 255;
        dest += 4;
        src += 3;
      } while (--nPixels);
    } else if (mFormat == RGBA8 && newFormat == BGRA8) {
      // Fast path for RGBA8->BGRA8 (just swizzle)
      do {
        dest[0] = src[2];
        dest[1] = src[1];
        dest[2] = src[0];
        dest[3] = src[3];
        dest += 4;
        src += 4;
      } while (--nPixels);
    } else {
      int srcSize = GetBytesPerPixel(mFormat);
      int nSrcChannels = GetChannelCount(mFormat);

      int destSize = GetBytesPerPixel(newFormat);
      int nDestChannels = GetChannelCount(newFormat);

      do {
        float rgba[4];

        if (IsFloatFormat(mFormat)) {
          if (mFormat <= RGBA16F) {
            for (int i = 0; i < nSrcChannels; i++) {
              rgba[i] = Math_Half2Float(((uint16_t *) src)[i]);
            }
          } else {
            for (int i = 0; i < nSrcChannels; i++) {
              rgba[i] = ((float *) src)[i];
            }
          }
        } else if (mFormat >= I16 && mFormat <= RGBA16) {
          for (int i = 0; i < nSrcChannels; i++) {
            rgba[i] = ((uint16_t *) src)[i] * (1.0f / 65535.0f);
          }
        } else {
          for (int i = 0; i < nSrcChannels; i++) {
            rgba[i] = src[i] * (1.0f / 255.0f);
          }
        }
        if (nSrcChannels < 4) {
          rgba[3] = 1.0f;
        }
        if (nSrcChannels == 1) {
          rgba[2] = rgba[1] = rgba[0];
        }

        if (nDestChannels == 1) {
          rgba[0] = 0.30f * rgba[0] + 0.59f * rgba[1] + 0.11f * rgba[2];
        }

        if (IsFloatFormat(newFormat)) {
          if (newFormat <= RGBA32F) {
            if (newFormat <= RGBA16F) {
              for (int i = 0; i < nDestChannels; i++) {
                ((uint16_t *) dest)[i] = Math_Float2Half(rgba[i]);
              }
            } else {
              for (int i = 0; i < nDestChannels; i++) {
                ((float *) dest)[i] = rgba[i];
              }
            }
          } else {
            if (newFormat == RGBE8) {
              *(uint32_t *) dest = Math_FloatRGBToRGBE8(rgba[0], rgba[1], rgba[2]);
            } else {
              *(uint32_t *) dest = Math_FloatRGBToRGB9E5(rgba[0], rgba[1], rgba[2]);
            }
          }
        } else if (newFormat >= I16 && newFormat <= RGBA16) {
          for (int i = 0; i < nDestChannels; i++) {
            ((uint16_t *) dest)[i] = (uint16_t) (65535 * Math_SaturateF(rgba[i]) + 0.5f);
          }
        } else if (newFormat == RGB10A2) {
          *(uint32_t *) dest =
              (uint32_t(1023.0f * Math_SaturateF(rgba[0]) + 0.5f) << 22) |
                  (uint32_t(1023.0f * Math_SaturateF(rgba[1]) + 0.5f) << 12) |
                  (uint32_t(1023.0f * Math_SaturateF(rgba[2]) + 0.5f) << 2) |
                  (uint32_t(3.0f * Math_SaturateF(rgba[3]) + 0.5f));
        } else {
          for (int i = 0; i < nDestChannels; i++) {
            dest[i] = (unsigned char) (255 * Math_SaturateF(rgba[i]) + 0.5f);
          }
        }

        src += srcSize;
        dest += destSize;
      } while (--nPixels);
    }
  }
  free(pData);
  pData = newPixels;
  mFormat = newFormat;

  return true;
}

template<typename T>
void buildMipMap(T *dst, const T *src, const uint32_t w, const uint32_t h, const uint32_t d, const uint32_t c) {
  uint32_t xOff = (w < 2) ? 0 : c;
  uint32_t yOff = (h < 2) ? 0 : c * w;
  uint32_t zOff = (d < 2) ? 0 : c * w * h;

  for (uint32_t z = 0; z < d; z += 2) {
    for (uint32_t y = 0; y < h; y += 2) {
      for (uint32_t x = 0; x < w; x += 2) {
        for (uint32_t i = 0; i < c; i++) {
          *dst++ =
              (src[0] + src[xOff] + src[yOff] + src[yOff + xOff] + src[zOff] + src[zOff + xOff] + src[zOff + yOff] +
                  src[zOff + yOff + xOff]) /
                  8;
          src++;
        }
        src += xOff;
      }
      src += yOff;
    }
    src += zOff;
  }
}

bool Image::GenerateMipMaps(const uint32_t mipMaps) {
  if (IsCompressedFormat(mFormat)) {
    return false;
  }
  if (!(mWidth) || !Math_IsPowerOf2U32(mHeight) || !Math_IsPowerOf2U32(mDepth)) {
    return false;
  }

  uint32_t actualMipMaps = Math_MinU32(mipMaps, GetMipMapCountFromDimensions());

  if (mMipMapCount != actualMipMaps) {
    int size = GetMipMappedSize(0, actualMipMaps);
    if (mArrayCount > 1) {
      uint8_t *newPixels = (uint8_t *) malloc(sizeof(uint8_t) * size * mArrayCount);

      // Copy top mipmap of all array slices to new location
      int firstMipSize = GetMipMappedSize(0, 1);
      int oldSize = GetMipMappedSize(0, mMipMapCount);

      for (uint32_t i = 0; i < mArrayCount; i++) {
        memcpy(newPixels + i * size, pData + i * oldSize, firstMipSize);
      }

      free(pData);
      pData = newPixels;
    } else {
      pData = (uint8_t *) realloc(pData, size);
    }
    mMipMapCount = actualMipMaps;
  }

  int nChannels = GetChannelCount(mFormat);

  int n = IsCube() ? 6 : 1;

  for (uint32_t arraySlice = 0; arraySlice < mArrayCount; arraySlice++) {
    uint8_t *src = GetPixels(0, arraySlice);
    uint8_t *dst = GetPixels(1, arraySlice);

    for (uint32_t level = 1; level < mMipMapCount; level++) {
      uint32_t w = GetWidth(level - 1);
      uint32_t h = GetHeight(level - 1);
      uint32_t d = GetDepth(level - 1);

      uint32_t srcSize = GetMipMappedSize(level - 1, 1) / n;
      uint32_t dstSize = GetMipMappedSize(level, 1) / n;

      for (uint32_t i = 0; i < n; i++) {
        if (IsPlainFormat(mFormat)) {
          if (IsFloatFormat(mFormat)) {
            buildMipMap((float *) dst, (float *) src, w, h, d, nChannels);
          } else if (mFormat >= I16) {
            buildMipMap((uint16_t *) dst, (uint16_t *) src, w, h, d, nChannels);
          } else {
            buildMipMap(dst, src, w, h, d, nChannels);
          }
        }
        src += srcSize;
        dst += dstSize;
      }
    }
  }

  return true;
}

bool Image::iSwap(const int c0, const int c1) {
  if (!IsPlainFormat(mFormat)) {
    return false;
  }

  unsigned int nPixels = GetNumberOfPixels(0, mMipMapCount) * mArrayCount;
  unsigned int nChannels = GetChannelCount(mFormat);

  if (mFormat <= RGBA8) {
    swapPixelChannels((uint8_t *) pData, nPixels, nChannels, c0, c1);
  } else if (mFormat <= RGBA16F) {
    swapPixelChannels((uint16_t *) pData, nPixels, nChannels, c0, c1);
  } else {
    swapPixelChannels((float *) pData, nPixels, nChannels, c0, c1);
  }

  return true;
}

*/

EXTERN_C size_t Image_BytesRequiredForMipMapsOf(Image_ImageHeader const *image) {

  int const maxMipLevels =
      Math_Log2(Math_MaxI32(image->depth,
                            Math_MaxI32(image->width, image->height)));
  uint32_t minWidth = 1;
  uint32_t minHeight = 1;
  uint32_t minDepth = 1;
  if (Image_Format_IsCompressed(image->format)) {
    minWidth = Image_Format_WidthOfBlock(image->format);
    minHeight = Image_Format_HeightOfBlock(image->format);
  }

  switch (image->format) {
    case Image_Format_PVR_4BPP_BLOCK:
    case Image_Format_PVR_4BPPA_BLOCK:
    case Image_Format_PVR_4BPP_SRGB_BLOCK:
    case Image_Format_PVR_4BPPA_SRGB_BLOCK:
      minWidth = 8;
      minHeight = 8;
      break;
    case Image_Format_PVR_2BPP_BLOCK:
    case Image_Format_PVR_2BPPA_BLOCK:
    case Image_Format_PVR_2BPP_SRGB_BLOCK:
    case Image_Format_PVR_2BPPA_SRGB_BLOCK:
      minWidth = 16;
      minHeight = 8;
      break;
    default:break;
  }

  int size = 0;
  int level = maxMipLevels;

  Image_ImageHeader scratch;
  scratch.format = image->format;
  scratch.width = image->width;
  scratch.height = image->height;
  scratch.depth = image->depth;
  scratch.slices = image->slices;

  while (level > 0) {
    size += Image_ByteCountOf(&scratch);

    scratch.width >>= 1;
    scratch.height >>= 1;
    scratch.depth >>= 1;

    if (scratch.width + scratch.height + scratch.depth == 0) {
      break;
    }
    scratch.width = Math_MaxI32(scratch.width, minWidth);
    scratch.height = Math_MaxI32(scratch.height, minHeight);
    scratch.depth = Math_MaxI32(scratch.depth, minDepth);

    level--;
  }

  return size;

}
