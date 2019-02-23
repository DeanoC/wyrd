
#pragma once
#ifndef WYRD_IMAGE_IMAGE_H
#define WYRD_IMAGE_IMAGE_H

#include "core/core.h"
#include "core/logger.h"
#include "math/math.h"
#include "image/pixel.h"
#include "image/format.h"

enum Image_Channel_t {
  Image_Red,
  Image_Green,
  Image_Blue,
  Image_Alpha,
};

EXTERN_C enum Image_Channel_t Image_Channel_Swizzle(enum Image_Format_t format, enum Image_Channel_t channel);

// Upto 3D image data, stored as packed formats but accessed as double
// upto 4 channels per pixel always RGBA (R = channel 0, A = channel 3)
// Support image arrays/slices
// the default is the generic texture format image (GIMG) but can be subclassed
// Image always requires to the first channel as R etc.
// this means that you ask for R and it will retrieve it from wherever
// it really is in the format (i.e. you don't worry about how its encoded)
// however this does leave a few formats a bit weird, i.e. X8D24 has X as
// R and D as G.. but that matches shaders generally anyway.

// the image data follows this header directly
typedef struct Image_Header_t {
  uint64_t dataSize;

  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t slices;

  enum Image_Format_t format;
} Image_Header_t;

// Image are fundementally 4D arrays however 'helper' function let you
// create and use them in more familar texture terms
EXTERN_C Image_Header_t *Image_Create(uint32_t width,
                                      uint32_t height,
                                      uint32_t depth,
                                      uint32_t slices,
                                      enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_CreateNoClear(uint32_t width,
                                             uint32_t height,
                                             uint32_t depth,
                                             uint32_t slices,
                                             enum Image_Format_t format);

// helpers
EXTERN_C Image_Header_t *Image_Create1D(uint32_t width, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create1DNoClear(uint32_t width, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create2DArray(uint32_t width,
                                             uint32_t height,
                                             uint32_t slices,
                                             enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create2DArrayNoClear(uint32_t width,
                                                    uint32_t height,
                                                    uint32_t slices,
                                                    enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create3DNoClear(uint32_t width,
                                               uint32_t height,
                                               uint32_t depth,
                                               enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create3DArray(uint32_t width,
                                             uint32_t height,
                                             uint32_t depth,
                                             uint32_t slices,
                                             enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_Create3DArrayNoClear(uint32_t width,
                                                    uint32_t height,
                                                    uint32_t depth,
                                                    uint32_t slices,
                                                    enum Image_Format_t format);

EXTERN_C Image_Header_t *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_CreateCubemapArray(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t slices,
                                                  enum Image_Format_t format);
EXTERN_C Image_Header_t *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                         uint32_t height,
                                                         uint32_t slices,
                                                         enum Image_Format_t format);

EXTERN_C void Image_Destroy(Image_Header_t *image);


EXTERN_C inline void *Image_RawDataPtr(Image_Header_t const *image) {
  ASSERT(image != NULL);
  return (void *) (image + 1);
}

EXTERN_C size_t Image_CalculateIndex(Image_Header_t const *image, uint32_t x, uint32_t y, uint32_t z, uint32_t slice);
EXTERN_C void Image_GetPixelAt(Image_Header_t const *image, Image_Pixel_t *pixel, size_t index);
EXTERN_C void Image_SetPixelAt(Image_Header_t const *image, Image_Pixel_t const *pixel, size_t index);
EXTERN_C double Image_GetChannelAt(Image_Header_t const *image, enum Image_Channel_t channel, size_t index);
EXTERN_C void Image_SetChannelAt(Image_Header_t const *image, enum Image_Channel_t channel, size_t index, double value);

#endif //WYRD_IMAGE_IMAGE_H
