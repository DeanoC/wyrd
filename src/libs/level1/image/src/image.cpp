#include "core/core.h"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"

EXTERN_C Image_Header_t *Image_Create(uint32_t width,
                                      uint32_t height,
                                      uint32_t depth,
                                      uint32_t slices,
                                      enum Image_Format_t format) {
  Image_Header_t *image = Image_CreateNoClear(width, height, depth, slices, format);
  if (image) {
    memset(image + 1, 0, image->dataSize);
  }

  return image;
}

EXTERN_C Image_Header_t *Image_CreateNoClear(uint32_t width,
                                             uint32_t height,
                                             uint32_t depth,
                                             uint32_t slices,
                                             enum Image_Format_t format) {
  // block compression can't be less than 4x4
  if ((width < 4 || height < 4) && Image_Format_IsCompressed(format)) {
    return nullptr;
  }

  uint64_t const dataSize = (width *
      height *
      depth *
      slices *
      Image_Format_BitWidth(format)) / 8;

  auto *image = (Image_Header_t *) malloc(sizeof(Image_Header_t) + dataSize);
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

EXTERN_C void Image_Destroy(Image_Header_t *image) {
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

EXTERN_C size_t Image_CalculateIndex(Image_Header_t const *image, uint32_t x, uint32_t y, uint32_t z, uint32_t slice) {
  ASSERT(image);

  ASSERT(x < image->width);
  ASSERT(y < image->height);
  ASSERT(z < image->depth);
  ASSERT(slice < image->slices);

  size_t const size1D = image->width;
  size_t const size2D = image->width * image->height;
  size_t const size3D = image->width * image->height * image->depth;
  size_t const index = (slice * size3D) + (z * size2D) + (y * size1D) + x;

  return index;
}

EXTERN_C Image_Header_t *Image_Create1D(uint32_t width, enum Image_Format_t format) {
  return Image_Create(width, 1, 1, 1, format);
}
EXTERN_C Image_Header_t *Image_Create1DNoClear(uint32_t width, enum Image_Format_t format) {
  return Image_CreateNoClear(width, 1, 1, 1, format);
}
EXTERN_C Image_Header_t *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format_t format) {
  return Image_Create(width, 1, 1, slices, format);
}
EXTERN_C Image_Header_t *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format_t format) {
  return Image_CreateNoClear(width, 1, 1, slices, format);
}

EXTERN_C Image_Header_t *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format_t format) {
  return Image_Create(width, height, 1, 1, format);
}
EXTERN_C Image_Header_t *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, 1, 1, format);
}
EXTERN_C Image_Header_t *Image_Create2DArray(uint32_t width,
                                             uint32_t height,
                                             uint32_t slices,
                                             enum Image_Format_t format) {
  return Image_Create(width, height, 1, slices, format);
}
EXTERN_C Image_Header_t *Image_Create2DArrayNoClear(uint32_t width,
                                                    uint32_t height,
                                                    uint32_t slices,
                                                    enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, 1, slices, format);
}

EXTERN_C Image_Header_t *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format_t format) {
  return Image_Create(width, height, depth, 1, format);
}
EXTERN_C Image_Header_t *Image_Create3DNoClear(uint32_t width,
                                               uint32_t height,
                                               uint32_t depth,
                                               enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, depth, 1, format);
}
EXTERN_C Image_Header_t *Image_Create3DArray(uint32_t width,
                                             uint32_t height,
                                             uint32_t depth,
                                             uint32_t slices,
                                             enum Image_Format_t format) {
  return Image_Create(width, height, depth, slices, format);
}
EXTERN_C Image_Header_t *Image_Create3DArrayNoClear(uint32_t width,
                                                    uint32_t height,
                                                    uint32_t depth,
                                                    uint32_t slices,
                                                    enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, depth, slices, format);
}

EXTERN_C Image_Header_t *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format_t format) {
  return Image_Create(width, height, 1, 6, format);
}
EXTERN_C Image_Header_t *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, 1, 6, format);
}
EXTERN_C Image_Header_t *Image_CreateCubemapArray(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t slices,
                                                  enum Image_Format_t format) {
  return Image_Create(width, height, 1, slices * 6, format);
}
EXTERN_C Image_Header_t *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                         uint32_t height,
                                                         uint32_t slices,
                                                         enum Image_Format_t format) {
  return Image_CreateNoClear(width, height, 1, slices * 6, format);
}


// we include fetch after swizzle so hopefully the compiler will inline it...
EXTERN_C inline enum Image_Channel_t Image_Channel_Swizzle(enum Image_Format_t format, enum Image_Channel_t channel) {
  Image_Swizzle_t swizzler = Image_Format_Swizzle(format);
  return (enum Image_Channel_t) swizzler[channel];
}

#include "image_fetch.hpp"
#include "image_put.hpp"

EXTERN_C double Image_GetChannelAt(Image_Header_t const *image, enum Image_Channel_t channel, size_t index) {
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

EXTERN_C void Image_SetChannelAt(Image_Header_t const *image,
                                 enum Image_Channel_t channel,
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

EXTERN_C void Image_GetPixelAt(Image_Header_t const *image, Image_Pixel_t *pixel, size_t index) {
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

EXTERN_C void Image_SetPixelAt(Image_Header_t const *image, Image_Pixel_t const *pixel, size_t index) {
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
