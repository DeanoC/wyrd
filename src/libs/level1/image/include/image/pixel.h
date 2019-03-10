#pragma once
#ifndef WYRD_IMAGE_PIXEL_H
#define WYRD_IMAGE_PIXEL_H

#include "core/core.h"
#include "math/math.h"

typedef struct Image_Pixel_t {
  double r;
  double g;
  double b;
  double a;
} Image_Pixel_t;

EXTERN_C inline void Image_PixelClamp(Image_Pixel_t *pixel, double const min[4], double const max[4]) {
  pixel->r = Math_ClampD(pixel->r, min[0], max[0]);
  pixel->g = Math_ClampD(pixel->g, min[1], max[1]);
  pixel->b = Math_ClampD(pixel->b, min[2], max[2]);
  pixel->a = Math_ClampD(pixel->a, min[3], max[3]);
}

#endif //WYRD_IMAGE_PIXEL_H
