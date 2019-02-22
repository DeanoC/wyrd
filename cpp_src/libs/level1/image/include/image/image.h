
#pragma once
#ifndef WYRD_IMAGE_IMAGE_H
#define WYRD_IMAGE_IMAGE_H

#include "core/core.h"
#include "math/math.h"
#include "image/pixel.h"
#include "image/format.h"

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

  Image_Format_t format;
} Image_Header_t;

#endif //WYRD_IMAGE_IMAGE_H
