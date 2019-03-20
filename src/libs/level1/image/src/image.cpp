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
  image->nextType = Image_IT_None;
  image->nextImage = nullptr;

  return image;

}

EXTERN_C void Image_Destroy(Image_ImageHeader *image) {
  // recursively free next chain
  switch (image->nextType) {
    case Image_IT_MipMaps:
    case IMAGE_IT_Layers:
      if (image->nextImage != nullptr) {
        Image_Destroy(image->nextImage);
      }
      break;
    default:
    case Image_IT_None:break;
  }

  free(image);
}

EXTERN_C size_t Image_CalculateIndex(Image_ImageHeader const *image, uint32_t x, uint32_t y, uint32_t z, uint32_t slice) {
  ASSERT(image);

  ASSERT(x < image->width);
  ASSERT(y < image->height);
  ASSERT(z < image->depth);
  ASSERT(slice < image->slices);

  size_t const size1D = Image_Calculate1DSize(image);
  size_t const size2D = Image_Calculate2DSize(image);
  size_t const size3D = Image_Calculate3DSize(image);
  size_t const index = (slice * size3D) + (z * size2D) + (y * size1D) + x;

  return index;
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
  return Image_Create(width, height, 1, 6, format);
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, 6, format);
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArray(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t slices,
                                                  enum Image_Format format) {
  return Image_Create(width, height, 1, slices * 6, format);
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                         uint32_t height,
                                                         uint32_t slices,
                                                         enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, slices * 6, format);
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
  if(src == dst) return;

  ASSERT(dst->slices == src->slices);
  ASSERT(dst->depth == src->depth);
  ASSERT(dst->height == src->height);
  ASSERT(dst->width == src->width);

  for (auto w = 0u; w < src->slices; ++w) {
    for(auto z = 0u; z < src->depth; ++z) {
      for(auto y = 0u; y < src->height; ++y) {
        for(auto x = 0u; x < src->width; ++x) {
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
  if(dst == src) {
    ASSERT(dw != sw);
  }

  for(auto z = 0u; z < src->depth; ++z) {
    for(auto y = 0u; y < src->height; ++y) {
      for(auto x = 0u; x < src->width; ++x) {
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
  if(dst == src) {
    ASSERT(dz != sz || dw != sw);
  }

  for(auto y = 0u; y < src->height; ++y) {
    for(auto x = 0u; x < src->width; ++x) {
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
                            Image_ImageHeader const* src,
                            uint32_t sy, uint32_t sz, uint32_t sw) {
  ASSERT(dst->width == src->width);
  if(dst == src) {
    ASSERT(dy != sy || dz != sz || dw != sw);
  }

  for(auto x = 0u; x < src->width; ++x) {
    size_t const srcIndex = Image_CalculateIndex(src, x, sy, sz, sw);
    size_t const dstIndex = Image_CalculateIndex(src, x, dy, dz, dw);
    Image_PixelD pixel;
    Image_GetPixelAt(src, &pixel, srcIndex);
    Image_SetPixelAt(dst, &pixel, dstIndex);
  }
}

EXTERN_C void Image_CopyPixel(Image_ImageHeader *dst,
                              uint32_t dx, uint32_t dy, uint32_t dz, uint32_t dw,
                              Image_ImageHeader const* src,
                              uint32_t sx, uint32_t sy, uint32_t sz, uint32_t sw) {
    size_t const srcIndex = Image_CalculateIndex(src, sx, sy, sz, sw);
    size_t const dstIndex = Image_CalculateIndex(src, dx, dy, dz, dw);
    Image_PixelD pixel;
    Image_GetPixelAt(src, &pixel, srcIndex);
    Image_SetPixelAt(dst, &pixel, dstIndex);
}


// TODO optimise or have option for faster mipmap chain generation
EXTERN_C void Image_CreateMipMapChain(Image_ImageHeader* image, bool generateFromImage) {
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
  if (curWidth <= 1 || curHeight <= 1) return;

  Image_ImageHeader *doubleImage = nullptr;
  Image_ImageHeader *scratchImage = nullptr;
  if (generateFromImage) {
    doubleImage = Image_Create(image->width, image->height, image->depth, image->slices, Image_Format_R64G64B64A64_SFLOAT);
    Image_CopyImage(doubleImage, image);
    scratchImage = Image_Create(image->width / 2, image->height / 2, 1, 1, image->format);
  }

  do {
    curWidth = curWidth / 2;
    curHeight = curHeight / 2;

    Image_ImageHeader *newImage = Image_Create(curWidth, curHeight, 1, image->slices, image->format);

    if (generateFromImage) {
      double *const scratch = (double *const) Image_RawDataPtr(scratchImage);

      for (auto w = 0u; w < image->slices; ++w) {
        double const
            *origSlice = (double const *) (((uint8_t *) Image_RawDataPtr(image)) + w * Image_Calculate3DSize(image));
        hq_resample<double>(4,
                            origSlice, image->width, image->height,
                            scratch, curWidth, curHeight);

        Image_CopySlice(newImage, w, scratchImage, 0);
      }
    }

    curImage->nextImage = newImage;
    curImage->nextType = Image_IT_MipMaps;
    curImage = newImage;
  } while (curWidth > 1 || curHeight > 1);

  if(doubleImage) {
    Image_Destroy(doubleImage);
  }
  if(scratchImage) {
    Image_Destroy(scratchImage);
  }
}
