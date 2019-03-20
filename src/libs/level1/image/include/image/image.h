
#pragma once
#ifndef WYRD_IMAGE_IMAGE_H
#define WYRD_IMAGE_IMAGE_H

#include "core/core.h"
#include "core/logger.h"
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

  Image_Format format;

  Image_NextType nextType;
  Image_ImageHeader *nextImage;

} Image_ImageHeader;

// Image are fundementally 4D arrays however 'helper' function let you
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

// helpers
EXTERN_C Image_ImageHeader *Image_Create1D(uint32_t width, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DNoClear(uint32_t width, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t slices,
                                                enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t slices,
                                                       enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DNoClear(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t depth,
                                                  enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t depth,
                                                       uint32_t slices,
                                                       enum Image_Format format);

EXTERN_C Image_ImageHeader *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapArray(uint32_t width,
                                                     uint32_t height,
                                                     uint32_t slices,
                                                     enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                            uint32_t height,
                                                            uint32_t slices,
                                                            enum Image_Format format);

EXTERN_C void Image_CreateMipMapChain(Image_ImageHeader *image, bool generateFromImage);

EXTERN_C void Image_Destroy(Image_ImageHeader *image);


EXTERN_C inline void *Image_RawDataPtr(Image_ImageHeader const *image) {
  ASSERT(image != NULL);
  return (void *) (image + 1);
}

EXTERN_C size_t Image_CalculateIndex(Image_ImageHeader const *image,
                                     uint32_t x,
                                     uint32_t y,
                                     uint32_t z,
                                     uint32_t slice);
EXTERN_C inline size_t Image_Calculate1DSize(Image_ImageHeader const *image) {
  return image->width;
}
EXTERN_C inline size_t Image_Calculate2DSize(Image_ImageHeader const *image) {
  return image->width * image->height;
}
EXTERN_C inline size_t Image_Calculate3DSize(Image_ImageHeader const *image) {
  return image->width * image->height * image->depth;
}

EXTERN_C inline size_t Image_Calculate1DPitch(Image_ImageHeader const *image) {
  return (Image_Calculate1DSize(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_Calculate2DPitch(Image_ImageHeader const *image) {
  return (Image_Calculate2DSize(image) * Image_Format_BitWidth(image->format)) / 8;
}
EXTERN_C inline size_t Image_Calculate3DPitch(Image_ImageHeader const *image) {
  return (Image_Calculate3DSize(image) * Image_Format_BitWidth(image->format)) / 8;
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
                            Image_ImageHeader const* src,
                            uint32_t sy, uint32_t sz, uint32_t sw);

EXTERN_C void Image_CopyPixel(Image_ImageHeader *dst,
                                     uint32_t dx, uint32_t dy, uint32_t dz, uint32_t dw,
                                     Image_ImageHeader const* src,
                                     uint32_t sx, uint32_t sy, uint32_t sz, uint32_t sw);





#endif //WYRD_IMAGE_IMAGE_H
