#pragma once
#ifndef WYRD_IMAGE_IMAGE_H
#define WYRD_IMAGE_IMAGE_H

#include "core/core.h"
#include "math/math.h"
#include "image/pixel.h"
#include "image/format.h"
#include "image/format_cracker.h"

typedef enum Image_Channel {
  Image_Red,
  Image_Green,
  Image_Blue,
  Image_Alpha,
} Image_Channel;

// give a format and a channel you want, returns the actually channel its stored in
EXTERN_C enum Image_Channel Image_Channel_Swizzle(enum Image_Format format, enum Image_Channel channel);

// Images can have a chain of related images, this type declares what if any
// the next pointer are. Image_IT_None means no next images
// The user is responsible to setting the next type and alloc the next
// chains. Destroy will free the entire chain.
// MipMaps + Layers in the same image is not supported
typedef enum Image_NextType {
  Image_IT_None,
  Image_IT_MipMaps,
  IMAGE_IT_Layers
} Image_NextType;

typedef enum Image_FlagBits {
  Image_Flag_Cubemap = 0x0,
  Image_Flag_HeaderOnly = 0x1,
} Image_FlagBits;
typedef uint16_t Image_Flags;

// Upto 4D (3D Arrays_ image data, stored as packed formats but
// accessed as double upto 4 channels per pixel in RGBA
// Support image arrays/slices
// Image always requires to the first channel as R etc.
// this means that you ask for R and it will retrieve it from wherever
// it really is in the format (i.e. you don't worry about how its encoded)
// however this does leave a few formats a bit weird, i.e. X8D24 has X as
// R and D as G.. but that matches shaders generally anyway.

// the image data follows this header directly
typedef struct Image_ImageHeader {
  uint64_t dataSize;

  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t slices;

  union {
    uint32_t fmtSizer;
    Image_Format format; //< type Image_Format
  };

  Image_Flags flags;
  uint8_t nextType; ///< Image_NextType
  uint8_t pad8;

  union {
    uint32_t pad[2];
    struct Image_ImageHeader *nextImage;
  };

} Image_ImageHeader;

// Image are fundamentally 4D arrays
// 'helper' functions in create.h let you
// create and use them in more familar texture terms
EXTERN_C Image_ImageHeader *Image_Create(uint32_t width,
                                         uint32_t height,
                                         uint32_t depth,
                                         uint32_t slices,
                                         enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateNoClear(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format);
EXTERN_C void Image_Destroy(Image_ImageHeader *image);

// if you want to use the calculation fields without an actual image
// this will fill in a valid header with no data or allocation
EXTERN_C void Image_FillHeader(uint32_t width,
                               uint32_t height,
                               uint32_t depth,
                               uint32_t slices,
                               enum Image_Format format,
                               Image_ImageHeader * header);


EXTERN_C inline void *Image_RawDataPtr(Image_ImageHeader const *image) {
  ASSERT(image != NULL);
  ASSERT((image->flags & Image_Flag_HeaderOnly) == 0)
  return (void *) (image + 1);
}

EXTERN_C void Image_GetPixelAt(Image_ImageHeader const *image, Image_PixelD *pixel, size_t index);
EXTERN_C void Image_SetPixelAt(Image_ImageHeader const *image, Image_PixelD const *pixel, size_t index);
EXTERN_C double Image_GetChannelAt(Image_ImageHeader const *image, enum Image_Channel channel, size_t index);
EXTERN_C void Image_SetChannelAt(Image_ImageHeader const *image,
                                 enum Image_Channel channel,
                                 size_t index,
                                 double value);
EXTERN_C void Image_CopyImage(Image_ImageHeader const *dst,
                              Image_ImageHeader const *src);

EXTERN_C void Image_CopyPage(Image_ImageHeader const *dst,
                             uint32_t dz, uint32_t dw,
                             Image_ImageHeader const *src,
                             uint32_t sz, uint32_t sw);

EXTERN_C void Image_CopySlice(Image_ImageHeader const *dst,
                              uint32_t dw,
                              Image_ImageHeader const *src,
                              uint32_t sw);

EXTERN_C void Image_CopyRow(Image_ImageHeader *dst,
                            uint32_t dy, uint32_t dz, uint32_t dw,
                            Image_ImageHeader const *src,
                            uint32_t sy, uint32_t sz, uint32_t sw);

EXTERN_C void Image_CopyPixel(Image_ImageHeader *dst,
                              uint32_t dx, uint32_t dy, uint32_t dz, uint32_t dw,
                              Image_ImageHeader const *src,
                              uint32_t sx, uint32_t sy, uint32_t sz, uint32_t sw);


EXTERN_C inline size_t Image_PixelCountPerRowOf(Image_ImageHeader const *image) {
  return image->width;
}
EXTERN_C inline size_t Image_PixelCountPerPageOf(Image_ImageHeader const *image) {
  return image->width * image->height;
}
EXTERN_C inline size_t Image_PixelCountPerSliceOf(Image_ImageHeader const *image) {
  return image->width * image->height * image->depth;
}
EXTERN_C inline size_t Image_PixelCountOf(Image_ImageHeader const *image) {
  return image->width * image->height * image->depth * image->slices;
}
EXTERN_C inline size_t Image_PixelCountOfImageChainOf(Image_ImageHeader const *image);

EXTERN_C inline size_t Image_CalculateIndex(Image_ImageHeader const *image,
                                            uint32_t x,
                                            uint32_t y,
                                            uint32_t z,
                                            uint32_t slice) {
  ASSERT(image);
  ASSERT(x < image->width);
  ASSERT(y < image->height);
  ASSERT(z < image->depth);
  ASSERT(slice < image->slices);
  size_t const size1D = Image_PixelCountPerRowOf(image);
  size_t const size2D = Image_PixelCountPerPageOf(image);
  size_t const size3D = Image_PixelCountPerSliceOf(image);
  size_t const index = (slice * size3D) + (z * size2D) + (y * size1D) + x;
  return index;
}

EXTERN_C inline size_t Image_ByteCountPerRowOf(Image_ImageHeader const *image) {
  return (Image_PixelCountPerRowOf(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_ByteCountPerPageOf(Image_ImageHeader const *image) {
  return (Image_PixelCountPerPageOf(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_ByteCountPerSlice(Image_ImageHeader const *image) {
  return (Image_PixelCountPerSliceOf(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_ByteCountOf(Image_ImageHeader const *image) {
  return (Image_PixelCountOf(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_ByteCountOfImageChain(Image_ImageHeader const *image) {

  size_t total = Image_ByteCountOf(image);

  switch (image->nextType) {
    case Image_IT_MipMaps:
    case IMAGE_IT_Layers:
      if (image->nextImage != NULL) {
        total += Image_ByteCountOfImageChain(image->nextImage);
      }
      break;
    default:
    case Image_IT_None:break;
  }

  return total;
}

EXTERN_C size_t Image_BytesRequiredForMipMapsOf(Image_ImageHeader const *image);

EXTERN_C size_t Image_LinkedCountOf(Image_ImageHeader const *image);
EXTERN_C Image_ImageHeader const *Image_LinkedImageOf(Image_ImageHeader const *image, size_t const index);

EXTERN_C inline bool Image_Is1D(Image_ImageHeader const *image) {
  return image->height == 1 && image->depth == 1;
}
EXTERN_C inline bool Image_Is2D(Image_ImageHeader const *image) {
  return image->height != 1 && image->depth == 1;
}
EXTERN_C inline bool Image_Is3D(Image_ImageHeader const *image) {
  return image->depth != 1;
}
EXTERN_C inline bool Image_IsArray(Image_ImageHeader const *image) {
  return image->slices != 1;
}
EXTERN_C inline bool Image_IsCubemap(Image_ImageHeader const *image) {
  return image->flags & Image_Flag_Cubemap;
}

#endif //WYRD_IMAGE_IMAGE_H
